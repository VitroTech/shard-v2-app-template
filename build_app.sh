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
#Check if folder is empty
DIR=app_src_backup
if [ -d "$DIR" ]
then
	if [ "$(ls -A $DIR)" ]; then
     echo "Take action $DIR is not Empty"
     DIR_NAME="backup_"$(date +%s)
     mkdir $DIR/$DIR_NAME
     mv "$DIR"/* $DIR/$DIR_NAME
	else
     mv app_src/* app_src_backup
     cp -r $application/. app_src/
     ./docker_build_image.sh
	fi
else
	echo "Directory $DIR not found."
fi
