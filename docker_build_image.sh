#!/bin/bash

# Stop the script for errors
set -o errexit
set -o pipefail

# Check for the argument
_argument_check() {

    if [ ! -d ./Applications/$1 ]
    then
        echo "Specify application to build. It must be located in the Application folder."
        exit 1
    fi

}

# Check if the command is supported
_command_exists() {

    command -v "$@" > /dev/null 2>&1

}

# Check if a file exists
_file_exists(){

    if [ ! -f ./"$@" ]
    then
        echo "File $@ is missing!"
        echo "Aborting..."
        exit 1
    fi

}

# Run the Docker image
_run_docker(){

    _file_exists $build_script
    _argument_check $1

    if _command_exists docker
    then
        docker run --rm -it --mount=type=bind,source="$(pwd)",destination=/var/mbed -w /var/mbed ghcr.io/armmbed/mbed-os-env /bin/bash -c "./$build_script $1"
    else
        echo "Docker is not installed."
        echo "Aborting ..."
        exit 1
    fi

}

build_script="build_image.sh"
_run_docker $1
