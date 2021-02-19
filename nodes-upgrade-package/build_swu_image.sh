#!/bin/bash

if [ "$#" -lt 1 ]; then
    vmajor=$(grep "VITROIO_TEMPLATE_VERSION_MAJOR   " ../common/global_consts.h)
    MAJOR=("${vmajor: -1}")
    vminor=$(grep "VITROIO_TEMPLATE_VERSION_MINOR   " ../common/global_consts.h)
    MINOR=("${vminor: -1}")
    vpatch=$(grep "VITROIO_TEMPLATE_VERSION_PATCH   " ../common/global_consts.h)
    PATCH=("${vpatch: -1}")
    vrc=$(grep "VITROIO_TEMPLATE_VERSION_RC   " ../common/global_consts.h)
    RC=("${vrc: -1}")
    vid=$(grep "VITRIOIO_TEMPLATE_FIRMWARE_ID   " ../common/global_consts.h)
    FIRMWARE_ID=("${vid: -1}")
    FIRMWARE_VERSION=($MAJOR.$MINOR.$PATCH.$RC)
    echo $FIRMWARE_VERSION $FIRMWARE_ID
else
    FIRMWARE_VERSION=${1}
    FIRMWARE_ID=3
fi

if [ "$#" -eq 2 ]; then
    FIRMWARE_ID=${2}
fi

WORK_DIR="./bin"

SW_DESCRIPTION="sw-description"
SW_DESCRIPTION_TEMPLATE="sw-description-template"
APPS_PATH="../bin"
APP_A_NAME="app_a.bin"
APP_B_NAME="app_b.bin"
UPGRADE_PACKAGE_JSON="upgrade_package.json"
UPGRADE_PACKAGE_JSON_TEMPLATE="upgrade_package_template.json"
SHELLSCRIPT="trigger_nodes_upgrade.sh"

echo -e "\nFirmware id is: ${FIRMWARE_ID}"
echo -e "\nFirmware version is: ${FIRMWARE_VERSION}\n"

[ -d $WORK_DIR ] || mkdir $WORK_DIR

cp $SW_DESCRIPTION_TEMPLATE  "$WORK_DIR/$SW_DESCRIPTION"

echo "Setting id in ${SW_DESCRIPTION} file..."
sed -e "s/id = \"x\"/id = \"${FIRMWARE_ID}\"/" -i "$WORK_DIR/$SW_DESCRIPTION"

echo "Setting version in ${SW_DESCRIPTION} file..."
sed -e "s/.*version =.*/\tversion = \"${FIRMWARE_VERSION}\";/" -i "$WORK_DIR/$SW_DESCRIPTION"

cp $UPGRADE_PACKAGE_JSON_TEMPLATE  "$WORK_DIR/$UPGRADE_PACKAGE_JSON"

echo "Setting id in ${UPGRADE_PACKAGE_JSON} file..."
sed -e "s/\"id\" : \"x\"/\"id\": \"${FIRMWARE_ID}\"/" -i "$WORK_DIR/$UPGRADE_PACKAGE_JSON"

echo "Setting version in ${UPGRADE_PACKAGE_JSON} file..."
sed -e "s/.*version.*/    \"version\": \"${FIRMWARE_VERSION}\",/" -i "$WORK_DIR/$UPGRADE_PACKAGE_JSON"

cp "$APPS_PATH/$APP_A_NAME" "$WORK_DIR"
cp "$APPS_PATH/$APP_B_NAME" "$WORK_DIR"
cp "$SHELLSCRIPT" "$WORK_DIR" 

cd $WORK_DIR
echo -e "\nBuilding SWU image...\n"
PRODUCT_NAME="vitroio_template_nodes_upgrade_package"
FILES="${SW_DESCRIPTION} ${APP_A_NAME} ${APP_B_NAME} ${UPGRADE_PACKAGE_JSON} ${SHELLSCRIPT}"
for i in $FILES;do
        echo $i;done | cpio -ov -H crc > ${PRODUCT_NAME}_${FIRMWARE_VERSION}.swu

for i in $FILES;do
        rm $i;
done
