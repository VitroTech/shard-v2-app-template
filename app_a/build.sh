#!/bin/bash

TOOLCHAIN=GCC_ARM
TARGET=NUCLEO_L486RG

[ -d mbed-os ] || mbed deploy
mbed compile --profile ./mbed-os/tools/profiles/release.json \
             -t $TOOLCHAIN \
             -m $TARGET \
             --source=. --source=../app_src --source=../common \
             --source=../libs/vitroio-sdk
