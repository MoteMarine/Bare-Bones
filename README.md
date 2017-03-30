# bare-bones

![banner](./img/logo.png)

> Single-file, JSON based datalogger

The previous datalogger build had three microcontrollers that held separate
roles. This system combines those three into one and eliminates the need for an RTC
because we assume the builds will be networked when installed.

Likewise, this system moves from a CSV configuration to a JSON one. Now communication,
both local and remote, will be standardized.

## Table of Contents

- [Background](#background)
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
