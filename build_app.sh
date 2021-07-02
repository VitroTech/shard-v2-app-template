#!/bin/bash

if [ $# -eq 0 ] 
then
    echo "Specify application to build."
    exit 1
fi

application="Applications/$1"
echo "$application"

if [ ! -d $application ]
then 
    echo "No directory named after given application."
    exit 1
fi

# Do a backup, just in case
mv app_src/* app_src_backup
cp -r $application/. app_src/
# ./docker_build_image.sh