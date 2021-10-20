#!/bin/bash

eval $(sed -n 's/^#define  *\([^ ]*\)  *\(.*\) *$/export \1=\2/p' ../common/global_consts.h)
FIRMWARE_VERSION=($VITROIO_TEMPLATE_VERSION_MAJOR.$VITROIO_TEMPLATE_VERSION_MINOR.$VITROIO_TEMPLATE_VERSION_PATCH.$VITROIO_TEMPLATE_VERSION_RC)

if [ $VITRIOIO_TEMPLATE_FIRMWARE_ID -gt 1023 ]; then
    echo "ERROR: VITRIOIO_TEMPLATE_FIRMWARE_ID out of range"
    exit 1
fi

if [ $VITROIO_TEMPLATE_VERSION_MAJOR -gt 15 ]; then
    echo "ERROR: VITROIO_TEMPLATE_VERSION_MAJOR out of range"
    exit 1
fi

if [ $VITROIO_TEMPLATE_VERSION_MINOR -gt 127 ]; then
    echo "ERROR: VITROIO_TEMPLATE_VERSION_MINOR out of range"
    exit 1
fi

if [ $VITROIO_TEMPLATE_VERSION_PATCH -gt 127 ]; then
    echo "ERROR: VITROIO_TEMPLATE_VERSION_PATCH out of range"
    exit 1
fi

if [ $VITROIO_TEMPLATE_VERSION_RC -gt 15 ]; then
    echo "ERROR: VITROIO_TEMPLATE_VERSION_RC out of range"
    exit 1
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

echo -e "\nFirmware id is: ${VITRIOIO_TEMPLATE_FIRMWARE_ID}"
echo -e "\nFirmware version is: ${FIRMWARE_VERSION}\n"

[ -d $WORK_DIR ] || mkdir $WORK_DIR

cp $SW_DESCRIPTION_TEMPLATE  "$WORK_DIR/$SW_DESCRIPTION"

echo "Setting id in ${SW_DESCRIPTION} file..."
sed -e "s/id = \"x\"/id = \"${VITRIOIO_TEMPLATE_FIRMWARE_ID}\"/" -i "$WORK_DIR/$SW_DESCRIPTION"

echo "Setting version in ${SW_DESCRIPTION} file..."
sed -e "s/.*version =.*/\tversion = \"${FIRMWARE_VERSION}\";/" -i "$WORK_DIR/$SW_DESCRIPTION"

cp $UPGRADE_PACKAGE_JSON_TEMPLATE  "$WORK_DIR/$UPGRADE_PACKAGE_JSON"

echo "Setting id in ${UPGRADE_PACKAGE_JSON} file..."
sed -e "s/\"id\" : \"x\"/\"id\": \"${VITRIOIO_TEMPLATE_FIRMWARE_ID}\"/" -i "$WORK_DIR/$UPGRADE_PACKAGE_JSON"

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
