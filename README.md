# shard-v2-app-template

If you want to build an application, be sure to initialize the submodules.

```
git submodule update --init
```

Demo applications are placed in `Applications` directory. To copy, and build,
selected application to `app_src` directory (from when application is being
build) run the following commad: 

```bash
./build_app.sh <APPLICATION>
``` 

> Note that the `app_src` directory is empy after cloning the repo, as its
> content is ignored by git. For first use, invoke the command listed above.

With the name of directory with application in place of `<APPLICATION>`.
for example: `./build_app.sh IoTBlock-test`.

To build the application, docker is recommended be installed on your PC. In this
way way you don't need to install the entire toolchain, you can just run the
ready-to-use script. If you don't have it installed and you use Ubuntu follow
this instruction: https://docs.docker.com/engine/install/ubuntu/

To build template application run docker_build_image.sh:

```
./docker_build_image.sh
```

Now the application is ready to be flashed on the Vitro Shard. The compiled
application binary is named `image.bin` and can be found in the `bin/` folder.
It can be done by using `docker_flash_image.sh` script.
