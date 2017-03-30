# bare-bones

![banner](./img/logo.png)

> Single-file, JSON based datalogger

The previous datalogger build had three microcontrollers that each held separate roles. 
This configuration attempts to combine three into one while eliminating the need for an RTC.
This assumes that all systems will be networked when installed and that the depth of the sensor
doesn't top the limits of I2C.

Likewise, the configuration file uses JSON, much like the messages to the server, to remmove complexity in communication.
All messages, both remote and local, now use JSON to remove the need for parsing and writing CSV files.

## Table of Contents

- [Parts](#parts)
- [Install](#install)
- [Usage](#usage)
- [Contribute](#contribute)
- [License](#license)

## Parts

```
Huzzah ESP8266
Micro SD breakout
MCP9808
```

## Install

Flash the `writeConfig.ino` file to your Huzzah ESP8266 with the SD breakout connected.
You'll need to edit any of the meta data as you see fit. Once you see the correct JSON data
being written to Serial the SD card is configured.

Open the `DataLogger.ino` file in `src` and add your wireless SSID and password. Change the hostname
to whatever host you want your datalogger to send its measurements to. If necessary change the time zone.

Once all changes are made, flash this file to the ESP8266 chip.

## Usage

Every 10 minutes, or number set in the configuration file, the datalogger reports the average of 5 temperature
measurements to the configured host.

## Contribute

PRs accepted.

## License

MIT © Ben Carothers
