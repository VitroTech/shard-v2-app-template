SET _path=//%cd:\=/%
SET _path2=%_path::=%
SET _path3=%_path2://C=//c%
docker run --rm -it -v %_path3%:/home/build vitrotech/docker-mbed-cli /bin/bash -c ./build_swu.sh