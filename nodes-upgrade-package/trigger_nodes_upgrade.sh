#!/bin/bash


UPGRADE_PACKAGE_PATH="/tmp/upgrade_package"
UPGRADE_NODES_SERVICE="vitroio-upgrade-nodes.service"


do_preinst()
{
        if [ -d "${UPGRADE_PACKAGE_PATH}" ]; then
                echo "${UPGRADE_PACKAGE_PATH} directory exists"
                echo "${UPGRADE_PACKAGE_PATH} directory cleanup..."
                rm -rf ${UPGRADE_PACKAGE_PATH}/*
        else
                echo "${UPGRADE_PACKAGE_PATH} directory does not exist"
                echo "Creating ${UPGRADE_PACKAGE_PATH} directory..."
                mkdir ${UPGRADE_PACKAGE_PATH}
        fi
        exit 0
}

do_postinst()
{
        # oneshot service to trigger /nodeGateway method via D-Bus
        echo "Starting ${UPGRADE_NODES_SERVICE}..."
        systemctl start ${UPGRADE_NODES_SERVICE}
        exit 0

}

echo $0 $1 > /dev/ttyO0

case "$1" in
preinst)
        echo "call do_preinst function"
        do_preinst
        ;;
postinst)
        echo "call do_postinst function"
        do_postinst
        ;;
*)
        echo "default"
        exit 1
        ;;
esac