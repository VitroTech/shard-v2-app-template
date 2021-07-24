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

#Check if docker group exist and if user is on this group
BELONGING_GROUPS="$(groups)"

if [[ $BELONGING_GROUPS == *"docker"* ]]; then
  # We already belong to docker group
  echo "BUILDING IMAGE"
  ./docker_build_image.sh
else
  #Either we do not belong to docker group or group does not exist
  ALL_GROUPS="$(cut -d : -f 1 /etc/group)"
  if [[ $ALL_GROUPS == *"docker"* ]]; then
    echo "Docker group exist but you are not part of it...Including you in docker group"
    sudo usermod -aG docker $USER
    echo "DONE"
    echo "You need to reboot for the changes to take effect"
  else
    echo "Docker group does not exist"
    sudo groupadd docker
    echo "Docker group created...Adding you to docker group"
    sudo usermod -aG docker $USER
    echo "DONE"
    echo "You need to reboot for the changes to take effect"
  fi
fi
