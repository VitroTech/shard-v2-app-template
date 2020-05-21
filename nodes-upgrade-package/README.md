## Building upgrade package

> NOTE: The script requires firmware binary files `app_a/b.bin` placed in
> `../bin` directory.

1. Run script:

    ```
    $ ./build_swu_image.sh
    ```

    Exemplary output:

    ```
    Firmware version is: 1.0.0
    Setting version in sw-description file...
    Setting version in upgrade_package.json file...

    Building SWU image...

    sw-description
    app_a.bin
    app_b.bin
    upgrade_package.json
    trigger_nodes_upgrade.sh
    323 blocks
    ```

    Output `swu` file will be available in `./bin` directory.