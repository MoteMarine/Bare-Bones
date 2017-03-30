#include <ESP8266WiFiMulti.h>
#include "Adafruit_MCP9808.h"
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <SD.h>

int redPin = 0;
int purplePin = 2;
#define SD_PIN 15
const int timeZone = -4;
static const char ntpServerName[] = "us.pool.ntp.org";
Adafruit_MCP9808 temperatureSensor = Adafruit_MCP9808();
unsigned int localPort = 8888;
ESP8266WiFiMulti WiFiMulti;
WiFiClient Client;
WiFiUDP Udp;

struct configs {
  int number;
  int model;
  int serial;
  String location;
  float lat;
  float lon;
  float timeout;
  String file;
  String date;
  String time;
};

struct configs configuration;

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT); 
  pinMode(purplePin, OUTPUT);

  WiFiMulti.addAP("NSA Surveillance Van", "O3fntvrpDc9GDI1NkIKxH");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    blink(redPin);
    delay(500);
  }

  Serial.println("WiFi Connected");
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

  while (!temperatureSensor.begin()) {
    blink(redPin);
    Serial.println("Couldn't find MCP9808!");
  }

  while (!SD.begin(SD_PIN)) {
    blink(redPin);
    Serial.println("Card failed, or not present");
  }

  configuration = setConfigVariables();
}

void loop() {
  if (connectedToHost()) {
    getDateAndTime(&configuration);
    String temperature = String(askForTemperature(configuration.number));
    String json = buildJSON(temperature, &configuration);
    updateReading(&configuration, json);
    cycleOff(configuration.timeout);
  }
}

void blink(int ledPin){
  digitalWrite(ledPin, HIGH); 
  delay(250); 
  digitalWrite(ledPin, LOW);
  delay(250); 
}

float askForTemperature(int numberOfReadings) {
  double sum = 0;
  for (int readingIndex = 0; readingIndex < numberOfReadings; readingIndex++) {
    double reading = readTemperature();
    sum = (sum + reading);
    readingIndex = reading == 0.00 ? readingIndex-- : readingIndex;
  }
  double average = sum / numberOfReadings;
  delay(15);
  return average;
}

double readTemperature() {
  temperatureSensor.shutdown_wake(0);
  delay(250);
  double temperature = temperatureSensor.readTempC();
  temperatureSensor.shutdown_wake(1);
  blink(purplePin);
  return temperature;
}

void updateReading(struct configs* configuration, String data) {
  writeDataToDisk(configuration->file, data);
  Client.print(data);
}

void cycleOff(float timeout) {
  Client.stop();
  delay(timeout * 60 * 1000);
  //ESP.deepSleep(timeout * 60 * 1000);
}

void writeDataToDisk(String fileName, String dataString) {
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.print("error opening " + fileName);
  }
}

String buildJSON (String data, struct configs* configuration) {
  StaticJsonBuffer<375> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& systems = jsonBuffer.createObject();
  JsonObject& sensors = jsonBuffer.createObject();
  JsonObject& gps = jsonBuffer.createObject();

  systems["ModelNumber"] = configuration->model;
  systems["SerialNumber"] = configuration->serial;
  systems["Location"] = configuration->location;
  gps["Lat"] = configuration->lat;
  gps["Lon"] = configuration->lon;
  systems["Date"] = configuration->date;
  systems["Time"] = configuration->time;
  sensors["Water_Temperature"] = data;

  JsonArray& System = root.createNestedArray("System");
  JsonArray& GPS = systems.createNestedArray("GPS");
  JsonArray& Sensor = root.createNestedArray("Sensors");

  System.add(systems);
  GPS.add(gps);
  Sensor.add(sensors);

  char json[350];
  root.printTo(json, sizeof(json));
  root.prettyPrintTo(Serial);
  String jsonString = json;
  jsonString.replace("[", "");
  jsonString.replace("]", "");
  return jsonString;
}

struct configs setConfigVariables() {
  struct configs currentConfigs;
  File configFile = SD.open("_config.txt");
  const size_t bufferSize = JSON_OBJECT_SIZE(7) + 130;
  DynamicJsonBuffer configBuffer(bufferSize);
  char charBuffer[bufferSize];

  if (configFile.available()) {
    String configVariables = configFile.readStringUntil('\n');
    configVariables.trim();
    configVariables.toCharArray(charBuffer, bufferSize);
    JsonObject& root = configBuffer.parseObject(configVariables);
    currentConfigs.lat = root["Latitude"].as<float>();
    currentConfigs.lon = root["Longitude"].as<float>();
    currentConfigs.model = root["ModelNumber"].as<int>();
    currentConfigs.serial = root["SerialNumber"].as<int>();
    currentConfigs.location = root["Location"].as<String>();
    currentConfigs.timeout = root["DelayTime"].as<float>();
    currentConfigs.number = root["NumberOfReadings"];
    configFile.close();
  } else {
    Serial.println("error opening _config.txt");
  }
  return currentConfigs;
}

void getDateAndTime(struct configs* configuration) {
  configuration->file = (String(month()) + "_" + String(year()) + ".txt");
  configuration->date = (String(month()) + "/" + String(day()) + "/" + String(year()));
  configuration->time =  (String(hour()) + ":" + String(minute()) + ":" + String(second()));
}

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

time_t getNtpTime() {
  IPAddress ntpServerIP;
  while (Udp.parsePacket() > 0) ;
  Serial.println("Transmit NTP Request");
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response...");
  return 0;
}

void sendNTPpacket(IPAddress &address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

boolean connectedToHost() {
  const char * host = "clouddev.mote.org";
  const uint16_t port = 4001;
  while (!Client.connect(host, port)) {
    delay(5000);
    Serial.print(".");
  }
  return true;
}

