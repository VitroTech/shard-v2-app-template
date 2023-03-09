## Building upgrade package

> NOTE: The script requires firmware binary files `app_a/b.bin` placed in
  `../bin/<APPLICATION>` directory, where APPLICATION is the type of previously
  build application.

1. Run script:

    ```
    $ ./build_swu_image.sh IoTBlock-test
    ```

    Expected output:

    ```
    Firmware id is: 0

    Firmware version is: 1.4.1.0

    Setting id in sw-description file...
    Setting version in sw-description file...
    Setting id in upgrade_package.json file...
    Setting version in upgrade_package.json file...

    Building SWU image...

    sw-description
    app_a.bin
    app_b.bin
    upgrade_package.json
    trigger_nodes_upgrade.sh
    380 blocks
    ```

    Output `swu` file will be available in `./bin/<APPLICATION>` directory.
