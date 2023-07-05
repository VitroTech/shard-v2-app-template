# shard-v2-app-template

This repository contains example application written with `Vitro Shard SDK`
published in public, [SDK](https://github.com/VitroTech/vitroio-sdk) repository.

## Building application

If you want to build an application, be sure to initialize the submodules.

```
git submodule update --init
```

To build the application, docker is recommended to be installed on your PC. This
way you don't need to install the entire toolchain, you can just run the
ready-to-use script. If you don't have it installed and you use Ubuntu, follow
[install instruction](https://docs.docker.com/engine/install/ubuntu/) and
[post-installation steps](https://docs.docker.com/engine/install/linux-postinstall/).

Demo applications are placed in `Applications` directory. To copy, and build,
selected application to `app_src` directory (from when application is being
build) run the following command with the name of application directory in place
of `<APPLICATION>`, for example: `./build_app.sh IoTBlock-test`.

```bash
./build_app.sh <APPLICATION>
```

> Note: that the `app_src` directory is empty after cloning the repo, as its
  content is ignored by git. For first use, invoke the command listed above.

> Note: if docker is installed on your system, the `build_app.sh` script will
  at the end run `docker_build_image.sh` script which will create image from
  compiled application.

Now the application is ready to be flashed on the Vitro Shard. The compiled
application binary is named `image.bin` and can be found in the `bin/` folder.
It can be done by using `docker_flash_image.sh` script.

> Note: this method needs ST-Link device connected to host PC and also a header
  populated on Shard.

## Building swu image

The `swu` image is and update image for Vitro Shard built with
[SWUpdate](https://sbabic.github.io/swupdate/swupdate.html) project. In order to
prepare such package, please follow the
[README](./nodes-upgrade-package/README.md) placed in `nodes-upgrade-package`
subdirectory.

## Notes on mbedOS 6.x
To reduce the code size Mbed introduced Minimal printf and snprintf. As of
Mbed OS 6.0 it is enabled by default. Floating point parameters are only
present when minimal-printf-enable-floating-point config is set to true
(disabled by default) as shown below:
```
"target_overrides": {
        "*": {
            "target.printf_lib": "minimal-printf",
            "platform.minimal-printf-enable-floating-point": true,
            "platform.minimal-printf-set-floating-point-max-decimals": 6,
            "platform.minimal-printf-enable-64-bit": false
        }
    }
```

If your application requires more advanced functionality (at the cost of
using more flash memory) you can switch to the standard printf library
configuring it in mbed_app.json file by overriding the parameter
target.printf_lib with the value std as shown below:
 ```
 "target_overrides": {
        "*": {
            "target.printf_lib": "std"
        }
    }
```
