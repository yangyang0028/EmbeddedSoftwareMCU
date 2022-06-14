#!/bin/bash

EmbeddedSoftwareMCU=$(pwd)
Tools="/Tools"
ArmGccUrl="https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2"
ArmGcc=$EmbeddedSoftwareMCU$Tools

cd $ArmGcc

echo "gcc-arm-none-eabi download to $(pwd)"

rm -rf gcc-arm-none*

wget $ArmGccUrl -O gcc-arm-none-eabi.tar.bz2

mkdir ./gcc-arm-none-eabi 

tar jxvf gcc-arm-none-eabi.tar.bz2 -C ./gcc-arm-none-eabi  --strip-components 1

rm -rf gcc-arm-none-eabi.tar.bz2

chmod +x ./gcc-arm-none-eabi -R
