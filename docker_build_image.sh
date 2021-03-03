#!/bin/bash

docker run --rm -it -v $PWD:/home/build vitrotech/docker-mbed-cli /bin/bash -c ./build_image.sh
