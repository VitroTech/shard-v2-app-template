#!/bin/bash

# Stop the script for errors
set -o errexit
set -o pipefail

# Check if the command is supported
_command_exists() {
    
    command -v "$@" > /dev/null 2>&1
    
}

# Check for the root user
_root_user(){
    
    user="$(id -un 2>/dev/null || true)"
    
    sh_c='sh -c'
    
    if [ "$user" != 'root' ]; then
        
        if _command_exists sudo; then
            
            sh_c='sudo -E sh -c'
            
            elif _command_exists su; then
            
            sh_c='su -c'
            
        else
            
            cat >&2 <<-'EOF'
		Error: this script requires root privileges.
		Aborting...
		EOF
            
            exit 1
            
        fi
        
    fi
    
}

# Remove the binary files
_clean_bin(){
    
    for i in "${APPS[@]}"
    do
   	$sh_c "rm -f ./$bin_dir/$i.bin"
    done

    $sh_c "rm -f ./$bin_dir/image.bin"
    
}

# Remove the BUILD folder
_clean_build(){
    
    for i in "${APPS[@]}"
    do
   	$sh_c "rm -rf ./$i/BUILD"
    done
    
}

# Main function
_main(){

    APPS=( "app_a" "app_b")
    bin_dir="bin"

    _root_user
    _clean_build
    _clean_bin

}

_main