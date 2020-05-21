# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/) and this
project adheres to [Semantic Versioning](https://semver.org/).

## [1.1.1] - 2019-03-07

Version released for test purposes only (functionally the same as 1.1.0)

## [1.1.0] - 2019-02-13

### Added:
- Implementation of sensors drivers/interfaces:
    - lm393 light detector
- Support for new protocol communication protocol - firmware ID handling.

### Changed:

- Support for new protocol communication protocol - firmware version handling.


## [1.0.1] - 2018-12-21

### Changed

-  changed position of parameters values in canframe

## [1.0.0] - 2018-12-17

### Added

- Initial implementation of sensors drivers/interfaces:
    - DHT22
- Initial implementation of target application:
    - reading parameters
    - sending parameters over CAN bus
    - initilization of communication with gateway
    - support for upgrades
- Initial implementation of bootloader
- LED indicator of status

[1.1.0]: https://github.com/vitroTV/vitroio-dht-demo-firmware/compare/v1.0.1...v1.1.0
[1.0.1]: https://github.com/vitroTV/vitroio-dht-demo-firmware/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/vitroTV/vitroio-dht-demo-firmware/compare/3de506f3c85b4f0e33f9c86f5c92713b2f61ebbb...v1.0.0
