#!/bin/bash

st-flash erase && st-flash write ./bin/image.bin 0x8000000