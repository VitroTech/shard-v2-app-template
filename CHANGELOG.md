# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/) and this
project adheres to [Semantic Versioning](https://semver.org/).

## [1.1.0] - 2021-10-12

### Added

- Added few demo applications
    - **shard-v2-app-template** - very simple example, it initializes
      communication with Crystal over CAN bus, then keeps sending IoT blocks.
    - **IoTBlock-test** - similar to previous example, but sends IoT blocks with
      random size and data.
    - **USB_MSD** - presents Shard as mass storage device when connected using
      USB, in root directory there is `test.txt` file available, everything
      written into that file will be send encrypted as part of IoT block when
      USB gets disconnected.
    - **demo-dht22** - measures temperature and humidity using DHT22 sensor,
      results are reported both to UART and to AWS.

### Changed

- Updated EEPROM layout, now applications can have size up to 470 KiB each,
  (previously it was 148 KiB)

- Updated SDK to v1.5.3

- Added script to generate `.swu` files (OTA upgrade packages)

### Fixed

- Fixed bug causing Shard communication with Crystal to fail to initialize
  properly, breaking OTA updates.

- Fixed bug causing Shard to improperly detect version of firmware during
  upgrade.

- Removed duplicated source files and headers from apps that don't need them.

## [1.0.0] - 2021-03-04

### Added

- Building and flushing scripts
- mbed OS setup
- vitroio-sdk and shard-v2-bootloader-image added as submodules
- Template for an application main.cpp file
    - It contains code that sends dummy IoT Block every 5s

[1.1.0]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/VitroTech/shard-v2-app-template/compare/b9c98589b7e6e711d9c1a72bfc582d45c8447fc7...v1.0.0
