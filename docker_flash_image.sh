#!/bin/bash

docker run --rm -it --privileged -v $PWD:/home/build vitrotech/docker-mbed-cli /bin/bash -c ./flash_image.sh
