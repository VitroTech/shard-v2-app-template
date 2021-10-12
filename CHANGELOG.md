# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/) and this
project adheres to [Semantic Versioning](https://semver.org/).

## [1.1.0] - 2021-10-12

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

## [1.0.0] - 2021-03-04

### Added

- Building and flushing scripts
- mbed OS setup
- vitroio-sdk and shard-v2-bootloader-image added as submodules
- Template for an application main.cpp file
    - It contains code that sends dummy IoT Block every 5s

[1.1.0]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/VitroTech/shard-v2-app-template/compare/b9c98589b7e6e711d9c1a72bfc582d45c8447fc7...v1.0.0
