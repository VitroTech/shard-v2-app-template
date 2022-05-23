#!/bin/bash

# Stop the script for errors
set -o errexit
set -o pipefail

_declare_variables(){
    
    TOOLCHAIN="GCC_ARM-RELEASE"
    TARGET="NUCLEO_L486RG"
    FLASH_SIZE="1024"
    
    BOOTLOADER_OFFSET="0"   # 0x00000
    BOOTLOADER_SIZE="65536" # 0x10000
    ENV_OFFSET="65536"      # 0x10000
    ENV_SIZE="2048"         # 0x00800
    APP_A_OFFSET="67584"    # 0x10800
    APP_A_SIZE="489472"     # 0x77800
    APP_B_OFFSET="557056"   # 0x88000
    APP_B_SIZE="489472"     # 0x77800
    
}

# Check if a folder exist. If not, create it
_folder_check() {
    
    if [ ! -d ./$@ ]
    then
    	echo "The $@ folder doesn't exist."
    	return 1
    else
        return 0
    fi
    
}

# Check if a file exists
_file_exists(){
    
    if [ ! -f ./$@ ]
    then
        echo "File $@ is missing!"
        return 1
    else
    	return 0
    fi
    
}

# Check if the bin folder exists, if not, create it.
_bin_check(){

    bin_folder="bin"

    if ! _folder_check $bin_folder
    then
    	echo "The $bin_folder directory does not exist."
    	echo "Creating the $bin_folder directory"
    	mkdir ./$bin_folder
    fi

}

# Compile app a and app b
_compile_app(){
    
    APPS=( "app_a" "app_b")
    local compile_script="build.sh"
    
    for i in "${APPS[@]}"
    do
        cd $i
        ./$compile_script
        cd -
        
        cp $i/BUILD/$TARGET/$TOOLCHAIN/$i.bin ./$bin_folder/
        
    done
    
}

# create padded file
_output_file(){
    
    local bootloader_dir="./shard-v2-bootloader-image/bootloader.bin"
    local OUT_BIN="./$bin_folder/image.bin"
    local app_a_bin="./$bin_folder/${APPS[0]}.bin"
    local app_b_bin="./$bin_folder/${APPS[1]}.bin"
    
    if _file_exists $bootloader_dir
    then
        dd if=/dev/zero ibs=1k count=$FLASH_SIZE | tr "\000" "\377" > "$OUT_BIN"
        dd if=$bootloader_dir of="$OUT_BIN" bs=1 count=$BOOTLOADER_SIZE seek=$BOOTLOADER_OFFSET conv=notrunc
        if _file_exists $app_a_bin
        then
            dd if=$app_a_bin of="$OUT_BIN" bs=1 count=$APP_A_SIZE seek=$APP_A_OFFSET conv=notrunc
        else
            echo "Please check if there is a problem with the compiler."
            echo "Aborting..."
            exit 1
        fi
        if _file_exists $app_b_bin
        then
            dd if=$app_b_bin of="$OUT_BIN" bs=1 count=$APP_B_SIZE seek=$APP_B_OFFSET conv=notrunc
        else
            echo "Please check if there is a problem with the compiler."
            echo "Aborting..."
            exit 1
        fi
    else
    	echo "The bootloader file is missing! Please run:"
    	echo "git submodule update --init"
    	echo "Aborting..."
    	exit 1
    fi
}

_main(){
  
    _declare_variables
    _bin_check
    _compile_app
    _output_file
    
}

_main