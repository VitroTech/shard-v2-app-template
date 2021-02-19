#!/bin/bash

TOOLCHAIN="GCC_ARM-RELEASE"
TARGET="NUCLEO_L486RG"
FLASH_SIZE="1024"

BOOTLOADER_OFFSET="0"   # 0x00000
BOOTLOADER_SIZE="65536" # 0x10000
ENV_OFFSET="65536"      # 0x10000
ENV_SIZE="2048"         # 0x00800
APP_A_OFFSET="67584"    # 0x10800
APP_A_SIZE="151552"     # 0x25000
APP_B_OFFSET="219136"   # 0x35800
APP_B_SIZE="151552"     # 0x25000



# Compile bootloader, app a and app b

[ -d bin ] || mkdir bin
#"bootloader"
APPS=( "app_a" "app_b")
for i in "${APPS[@]}"
do
   cd $i
   ./build.sh
   cd -

   cp $i/BUILD/$TARGET/$TOOLCHAIN/$i.bin ./bin/

done

OUT_BIN="./bin/image.bin"

# create padded file
dd if=/dev/zero ibs=1k count=$FLASH_SIZE | tr "\000" "\377" > "$OUT_BIN"

dd if=./bootloader/bootloader.bin of="$OUT_BIN" bs=1 count=$BOOTLOADER_SIZE seek=$BOOTLOADER_OFFSET conv=notrunc
dd if=./bin/app_a.bin of="$OUT_BIN" bs=1 count=$APP_A_SIZE seek=$APP_A_OFFSET conv=notrunc
dd if=./bin/app_b.bin of="$OUT_BIN" bs=1 count=$APP_B_SIZE seek=$APP_B_OFFSET
