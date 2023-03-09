#!/bin/bash

# Stop the script for errors
set -o errexit
set -o pipefail

# Check if the command is supported
_command_exists() {

    command -v "$@" > /dev/null 2>&1

}

# Check for the argument
_argument_check() {

    if [ ! -d ./../Applications/$1 ]
    then
        echo "Specify application to build. It must be located in the Application folder."
        exit 1
    fi

}

# Check if the user didn't delete the folder
_folder_check() {

    if [ ! -d "$application_root" ]
    then
        echo "The Application folder does not exist."
        echo "Do you wish to create it [y/n]?"

        while true; do

            read c

            if [[ $c == [yY] || $c == [yY][eE][sS] ]]
            then

                mkdir $application_root
                break

            elif [[ $c == [nN] || $c == [nN][oO] || $c == '' ]]
            then
                echo "Aborting the procedure..."
                exit 1

            else

                echo "Please type y for yes, and n for no."

            fi

        done
    fi

}

# Check if the target folder is empty or not
_empty_folder(){

    if [ ! "$(ls -A $application_root)" ]
    then
        echo "The Application folder is empty. An application must be in the folder."
        echo "Aborting..."
        exit 1
    fi

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

# Check if the application exist
_app_check() {

    application="$application_root/$@"
    echo "$application"

    if [ ! -d $application ]
    then
        cat >&2 <<-'EOF'
		Error: The application called does not exist in the Application folder.
		Aborting...
		EOF

        exit 1
    fi

}

# Check if the Docker is installed correctly
_docker_check() {

    if ! _command_exists docker
    then
        cat >&2 <<-'EOF'
		Error: The script requires Docker to run properlly.
		Aborting...
		EOF

        exit 1
    fi

    #Check if docker group exist and if user is on this group
    local BELONGING_GROUPS="$(groups)"

    if [[ ! $BELONGING_GROUPS == *"docker"* ]]; then
        echo "Docker is not properly configured."
        echo "By proceeding, the docker user will be in the docker group. Proceed [y/n]?"

        while true; do

            read c

            if [[ $c == [yY] || $c == [yY][eE][sS] ]]
            then

                echo "Proceeding with the script."
                break

            elif [[ $c == [nN] || $c == [nN][oO] || $c == '' ]]
            then
                echo "Aborting the procedure..."
                exit 1

            else

                echo "Please type y for yes, and n for no."

            fi

        done

        #Either we do not belong to docker group or group does not exist
        local ALL_GROUPS="$(cut -d : -f 1 /etc/group)"
        if [[ $ALL_GROUPS == *"docker"* ]]; then
            echo "Docker group exist but you are not part of it...Including you in docker group"
            $sh_c 'usermod -aG docker $USER'
            echo "DONE"
            return 1
        else
            echo "Docker group does not exist"
            $sh_c 'groupadd docker'
            echo "Docker group created...Adding you to docker group"
            $sh_c 'usermod -aG docker $USER'
            echo "DONE"
            return 1
        fi

    else
        return 0
    fi

}

# Do a backup of the previous , just in case
_do_backup() {

    local BACKUP_DIR=app_src_backup

    # Check if the directory exist, if not, create it
    if [ ! -d "$BACKUP_DIR" ]
    then
        mkdir $BACKUP_DIR
    fi

    # Create the backup folder
    local NEW_DIR_NAME="backup_"$(date +%s)
    mkdir $BACKUP_DIR/$NEW_DIR_NAME

    if [ "$(ls -A app_src -I ".gitkeep")" ]
    then
        mv app_src/* $BACKUP_DIR/$NEW_DIR_NAME
    fi

}

_move_app(){

    cp -r $application/. app_src/

}

# Reboot the system
_reboot() {

    echo "The system must be rebooted for the changes to take effect."
    echo "Do you wish to reboot your system now [y/n]?"

    while true; do

        read c

        if [[ $c == [yY] || $c == [yY][eE][sS] ]]
        then

            echo "Rebooting..."
            $sh_c 'reboot'
            break

        elif [[ $c == [nN] || $c == [nN][oO] || $c == '' ]]
        then
            echo "Aborting the procedure..."
            exit 1

        else

            echo "Please type y for yes, and n for no."

        fi

    done

}

# Go to the next script
_next_step(){

    echo "BUILDING IMAGE"
    ./$next_script $1

}

# Main function
_main() {

    application_root="Applications"
    next_script="docker_build_image.sh"

    _argument_check $1
    _folder_check
    _empty_folder
    _root_user
    _app_check $1
    if _docker_check
    then
        _do_backup
        _move_app
        _next_step $1
    else
        _reboot
    fi
}

_main "$@"
