# bare-bones


![banner](./img/logo.png)

> Single-file, JSON based datalogger

TODO: Fill out this long description.

## Table of Contents

- [Background](#background)
- [Install](#install)
- [Usage](#usage)
- [Contribute](#contribute)
- [License](#license)

## Background

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

Small note: If editing the README, please conform to the [standard-readme](https://github.com/RichardLitt/standard-readme) specification.

## License

MIT © Ben Carothers
