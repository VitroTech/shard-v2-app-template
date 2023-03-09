# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/) and this
project adheres to [Semantic Versioning](https://semver.org/).

<!--

TEMPLATE

## [x.y.z] YYYY-MM-DD

### Added

- New features/things that were not available
- ABC

### Changed

- Old features/things that now look/work in different way

### Fixed

- Old features/things that now work as they should

### Removed

- Removed features/things

-->

## [1.4.1] - 2023-03-09

### Changed

- SDK updated to v2.0.1.
- All scripts stored in repository now needs argument that points to application
  which given script uses. It is mandatory that application code will be stored
  inside `Applications/<APPLICATION_NAME>` subdirectory.
- CI job now works on tag pushes and also produce the `swu` update image and
  pushes it to the same bucket as Shard images which is
  [dev-vitro-shard-applications](https://s3.console.aws.amazon.com/s3/buckets/dev-vitro-shard-applications?region=eu-west-1&tab=objects).
- CI now build only application which name ist stored in PROJECT variable inside
  of ci.yml file, now it is only `IoTBlock-test` application.

## [1.4.0] - 2022-10-21

### Changed

- SDK updated to v2.0.0
- Bootloader updated to v1.0.2

## [1.3.0] - 2022-07-06

## Changed

- SDK updated to v1.8.2

## [1.2.0] - 2022-05-16

## Changed

- SDK updated to v1.8.0

## [1.1.2] - 2022-02-28

### Changed

- Updated references to bootloader binary repository

### Fixed

- SWU package builder script

## [1.1.1] - 2021-10-19

### Changed

- Update SDK to v1.5.4

### Fixed

- Now OTA update do not override firmware on Shard with different fwID

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

[Unreleased]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.4.1...HEAD
[1.4.1]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.4.0...v1.4.1
[1.4.0]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.3.0...v1.4.0
[1.3.0]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.1.2...v1.2.0
[1.1.2]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.1.1...v1.1.2
[1.1.1]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.1.0...v1.1.1
[1.1.0]: https://github.com/VitroTech/shard-v2-app-template/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/VitroTech/shard-v2-app-template/compare/b9c98589b7e6e711d9c1a72bfc582d45c8447fc7...v1.0.0
