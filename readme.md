## EmbeddedSoftwareMCU
This project uses makefile as linker and arm-gcc as compiler to develop MCU (AT32, STM32).

### How to use
install gcc-arm-none-eabi

```
wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -O gcc-arm-none-eabi.tar.bz2

mkdir ./Tools/gcc-arm-none-eabi

tar jxvf gcc-arm-none-eabi.tar.bz2 -C ./Tools/gcc-arm-none-eabi  --strip-components 1

rm -rf gcc-arm-none-eabi.tar.bz2
```
install JLink

```
wget https://www.segger.com/downloads/jlink/JLink_Linux_V768_x86_64.deb

sudo apt install ./JLink_Linux_V768_x86_64.deb

rm ./JLink_Linux_V768_x86_64.deb

echo 'export PATH=$PATH:/opt/SEGGER/JLink/' >> ~/.bashrc
```

compile project

```
cd Project/TestAssistant

make 
```

burn download

```
cd Project/TestAssistant

JLinkExe -device AT32F407VGT7 -if SWD -speed 4000 -CommanderScript flash.jlink
```