#!/bin/bash

# Check for the argument
_argument_check() {

    if [ ! -d $SCRIPT_PATH/../Applications/$1 ]
    then
        echo "Specify application to build. It must be located in the Application folder."
        exit 1
    fi

}

PRODUCT_NAME="$1"
SCRIPT_PATH="$(dirname -- "${BASH_SOURCE[0]}")"
WORK_DIR="$SCRIPT_PATH/bin/$PRODUCT_NAME"
APP_BUILD_DIR="$SCRIPT_PATH/../bin/$PRODUCT_NAME"
SW_DESCRIPTION="sw-description"
SW_DESCRIPTION_TEMPLATE="sw-description-template"
APP_A_NAME="app_a.bin"
APP_B_NAME="app_b.bin"
UPGRADE_PACKAGE_JSON="upgrade_package.json"
UPGRADE_PACKAGE_JSON_TEMPLATE="upgrade_package_template.json"
SHELLSCRIPT="trigger_nodes_upgrade.sh"

_argument_check $PRODUCT_NAME

eval $(sed -n 's/^#define  *\([^ ]*\)  *\(.*\) *$/export \1=\2/p' $SCRIPT_PATH/../common/global_consts.h)
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

echo -e "\nFirmware id is: ${VITRIOIO_TEMPLATE_FIRMWARE_ID}"
echo -e "\nFirmware version is: ${FIRMWARE_VERSION}\n"

[ -d $WORK_DIR ] || mkdir -p $WORK_DIR

cp "$SCRIPT_PATH/$SW_DESCRIPTION_TEMPLATE" "$WORK_DIR/$SW_DESCRIPTION"

echo "Setting id in ${SW_DESCRIPTION} file..."
sed -e "s/id = \"x\"/id = \"${VITRIOIO_TEMPLATE_FIRMWARE_ID}\"/" -i "$WORK_DIR/$SW_DESCRIPTION"

echo "Setting version in ${SW_DESCRIPTION} file..."
sed -e "s/.*version =.*/\tversion = \"${FIRMWARE_VERSION}\";/" -i "$WORK_DIR/$SW_DESCRIPTION"

cp "$SCRIPT_PATH/$UPGRADE_PACKAGE_JSON_TEMPLATE" "$WORK_DIR/$UPGRADE_PACKAGE_JSON"

echo "Setting id in ${UPGRADE_PACKAGE_JSON} file..."
sed -e "s/\"id\" : \"x\"/\"id\": \"${VITRIOIO_TEMPLATE_FIRMWARE_ID}\"/" -i "$WORK_DIR/$UPGRADE_PACKAGE_JSON"

echo "Setting version in ${UPGRADE_PACKAGE_JSON} file..."
sed -e "s/.*version.*/    \"version\": \"${FIRMWARE_VERSION}\",/" -i "$WORK_DIR/$UPGRADE_PACKAGE_JSON"

cp "$APP_BUILD_DIR/$APP_A_NAME" "$WORK_DIR"
cp "$APP_BUILD_DIR/$APP_B_NAME" "$WORK_DIR"
cp "$SCRIPT_PATH/$SHELLSCRIPT" "$WORK_DIR"

cd $WORK_DIR
echo -e "\nBuilding SWU image...\n"
FILES="${SW_DESCRIPTION} ${APP_A_NAME} ${APP_B_NAME} ${UPGRADE_PACKAGE_JSON} ${SHELLSCRIPT}"
for i in $FILES;do
        echo "$i";done | cpio -ov -H crc > ${PRODUCT_NAME}_${FIRMWARE_VERSION}.swu

for i in $FILES;do
        rm "$i";
done
