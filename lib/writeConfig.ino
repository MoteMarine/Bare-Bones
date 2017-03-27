#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  if (!SD.begin(15)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");

  writeConfiguration();
}

void loop() {
  readFile();
  delay(5000);
}

void writeConfiguration(){

  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["NumberOfReadings"] = 5;
  root["ModelNumber"] = 1;
  root["SerialNumber"] = 1;
  root["Location"] = "";
  root["Longitude"] = ;
  root["Latitude"] = ;
  root["DelayTime"] = 3;

  if (SD.exists("_config.txt")){
    SD.remove("_config.txt");
  }

  File configFile = SD.open("_config.txt", FILE_WRITE);
  Serial.println("Config");
  if (configFile) {
     char buffer[256];
     root.printTo(buffer, sizeof(buffer));
     Serial.println("Buffer:");
     Serial.println(buffer);
     configFile.println(buffer);
  }
  else {
    Serial.println("Error opening file...");
  }
  configFile.close();
}

void readFile(){
  File configFile = SD.open("_config.txt");
  if (configFile.available()) {
      String configVariables = configFile.readStringUntil('\n');
      configVariables.trim();
      Serial.println(configVariables);
      configFile.close();
  } else {
    Serial.println("error opening _config.txt");
  }
}

