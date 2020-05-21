#!/bin/bash
APPS=("bootloader" "app_a" "app_b")
for i in "${APPS[@]}"
do
   cd $i
   rm -rf ./BUILD
   cd - >/dev/null

   rm -f ./bin/$i.bin

done

rm -f ./bin/image.bin
