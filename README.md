# shard-v2-app-template

If you want to build an application, be sure to initialize the submodules.

```
git submodule update --init
```

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

Source code of the application is inside the `app_src/` directory. By default
app sends encrypted IoT Blocks with dummy values every 5 seconds:

```cpp
while(1)
{
    iotBlock.make(0, 0x132);
    iotBlock.send();
    ThisThread::sleep_for(5000);
}
```

Feel free to modify main.cpp and add your sensors interfaces and drivers inside
the `app_src/` directory.
