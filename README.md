# vitroio-usb-msd-demo-firmware

<!-- TOC -->
- [Introduction](#introduction)
- [Dependencies](#dependencies)
- [Preparation](#preparation)
- [Linux](#linux)
    - [Building linux](#building---linux)
    - [Flashing linux](#flashing-using-stlink-inside-docker-container)
- [Windows](#windows)
    - [Building Windows](#building---windows)
    - [Flashing Windows](#flashing-using-stlink-tools---windows)
- [Running](#running)

<!-- /TOC -->

## Introduction

`vitroio-usb-msd-demo-firmware` is a demo firmware for `Vitro Shard` module, based on 
[mbed-os](https://www.mbed.com/en/platform/mbed-os/)
and [vitro-shard-sdk](https://github.com/vitroTV/vitro-shard-sdk).

The main tasks are:

 - processing incoming USB connections
 - encryption of data and sending it over CAN bus, after USB was disconnected.

## Dependencies

The firmware requires `mbed-os` version `5.8.0` or later and
`vitro-shard-sdk` version `1.2.2` in static library form (already included
in `libs`).

## Preparation

1. Install [docker](https://docs.docker.com/engine/install/) for your platform.

2. After pulling repo, make sure to pull submodules as well. Invoke following
command:

```
$ git submodule update --init --recursive
```

## Linux

### Building - linux

1. In root repo directory run script:

   ```
   $ ./docker_build_image.sh
   ```

   Output binaries will be available in `bin` directory.

2. Optionally [prepare swu package](./nodes-upgrade-package/README.md)

### Flashing using stlink inside docker container

> Output binary file can be flashed using local CLI tool like stlink or GUI
  tool like STM32CubeProgrammer.

In root repo directory run script:

```
$ ./docker_flash_image.sh
```

> In some cases errors may appear while flashing. In this case, repeat the
  flashing.

## Windows

> :warning: To ensure the proper operation of the docker on Windows OS, repo
  directory should be place anywhere inside `C:\Users\`.

### Building - windows

1. In root repo directory run script:

    ```
    > docker_build_windows.bat
    ```

    Output binaries will be available in `bin` directory.

2. Optionally prepare swu package:

    ```
    > docker_prepare_swu_windows.bat
    ```
    Output swu file will be available in `nodes-upgrade-package/bin` directory.

### Flashing using stlink tools - windows

1. Install [stlink utilities](https://github.com/texane/stlink/releases).

2. Erase flash memory and program it.

    ```
    > st-flash erase && st-flash write <ROOT_REPO_DIR>/bin/image.bin 0x8000000
    ```

    Exemplary output:

    ```
    st-flash 1.4.0-36-g0af68c0
    2018-09-04T07:27:23 INFO common.c: Loading device parameters....
    2018-09-04T07:27:23 INFO common.c: Device connected is: L43x/L44x device, id 0x10016435
    2018-09-04T07:27:23 INFO common.c: SRAM size: 0xc000 bytes (48 KiB), Flash: 0x40000 bytes (256 KiB) in pages of 2048 bytes
    Mass erasing
    st-flash 1.4.0-36-g0af68c0
    2018-09-04T07:27:23 INFO common.c: Loading device parameters....
    2018-09-04T07:27:23 INFO common.c: Device connected is: L43x/L44x device, id 0x10016435
    2018-09-04T07:27:23 INFO common.c: SRAM size: 0xc000 bytes (48 KiB), Flash: 0x40000 bytes (256 KiB) in pages of 2048 bytes
    2018-09-04T07:27:23 INFO common.c: Attempting to write 237540 (0x39fe4) bytes to stm32 address: 134217728 (0x8000000)
    Flash page at addr: 0x08039800 erasedEraseFlash - Page:0x133 Size:0x800 
    2018-09-04T07:27:26 INFO common.c: Finished erasing 116 pages of 2048 (0x800) bytes
    2018-09-04T07:27:26 INFO common.c: Starting Flash write for F2/F4/L4
    2018-09-04T07:27:26 INFO flash_loader.c: Successfully loaded flash loader in sram
    size: 32768
    size: 32768
    size: 32768
    size: 32768
    size: 32768
    size: 32768
    size: 32768
    size: 8164
    2018-09-04T07:27:31 INFO common.c: Starting verification of write complete
    2018-09-04T07:27:33 INFO common.c: Flash written and verified! jolly good!
    ```

## Running

After connecting module to your PC with USB cable, it should be enumerated as
mass storage device, with about 66kb of space, and one file in it. Data to be
encrypted should be placed in this file (could be plain text, or binary data).
Name of this file, and its file type shouldn't be modified.
