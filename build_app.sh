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
BACKUP_DIR=app_src_backup
NEW_DIR_NAME="backup_"$(date +%s)
mkdir $BACKUP_DIR/$NEW_DIR_NAME

if [ "$(ls -A app_src -I ".gitkeep")" ]
then
  mv app_src/* $BACKUP_DIR/$NEW_DIR_NAME
  cp -r $application/. app_src/
else
   cp -r $application/. app_src/
fi

#SUDO privileges are required
sudo ./docker_build_image.sh
