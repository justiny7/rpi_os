# Bare metal OS on Raspberry Pi Zero

This is an ARMv6-based bare-metal OS for a Raspberry Pi Zero W! It started off as my codebase for labs in Stanford's CS140E/CS240LX classes, but I've developed more on my own since then.

## Installation

### Prerequisites
1. You'll need a Raspberry Pi and an SD card to upload kernels onto.
2. Make sure you have some cross compiler that can compile C code with no Linux libraries. I use musl's `arm-none-eabi` toolchain for Mac (which you can find [here](https://musl.cc/)).
3. (OPTIONAL) The Raspberry Pi Zero comes with a Broadcom VideoCore IV GPU that you can write custom kernels on. One assembler is vc4asm, which turns an assembly-like language into instruction encodings you can import into C programs. This codebase doesn't contain any `qasm` code, but has a kernel abstraction you can use to run it if you wish. You can install vc4asm with `brew install vc4asm`, or whatever works for your device.
    - Note: there is one issue I ran into when compiling vc4 code with not being able to find the built-in include files (`vc4.qinc`). To fix this, I updated the qasm rules in the Makefile (lines 58-60) with the absolute path to where my vc4 binary is stored. You may have to do the same.

### Clone
Clone the repo:
```bash
git clone git@github.com:justiny7/rpi_os.git
cd rpi_os
```

### Firmware
The Raspberry Pi comes with firmware to boot up and run kernels from the SD card.
1. [Format your SD card to FAT32](https://wiki.hacks.guide/wiki/Formatting_an_SD_card) so that it can run the firmware correctly.
2. Copy every file from [firmware/](firmware/) to the root directory of your SD card. You can also find these files (except `config.txt`) on the [official Raspberry Pi firmware page](https://github.com/raspberrypi/firmware/tree/master/boot).

### Bootloader
By default, the Raspberry Pi Zero firmware will look for a kernel image named `kernel.img` to start running. However, this means that any time you want to update the kernel, you'll have to take the SD card out, copy your new kernel, and re-insert it, which is pretty annoying. To fix this, this repo has a bootloader kernel and script that allows you upload kernels to the Pi directly without needing to take the SD card out. It does, however, require a serial USB connection to the Pi (there are modules you can buy for this). Otherwise you might be stuck just using the SD card. If you wish to use the bootloader, here is how to set it up:
1. Make the bootloader
```bash
cd bootloader
make
```
2. Copy `kernel.img` to your SD card
3. Add `bin/rpi-install` to your path

Now, you can install kernels with `rpi-install kernel.img`!


### Compile
Finally, you're ready to install the kernel! By default, it will run the code in `src/main.c`, which currently just prints "Hello world" through UART (again, requires serial connection).
```bash
make                    # (if your make version is too old, you might get an error - I used homebrew's gmake)
rpi-install kernel.img  # (or use your own bootloader / copy to SD card)
```
**Note**: The actual rpi_os `kernel.img`, which is created in the root of this project, is not to be confused with the bootloader `kernel.img`, which is created in `bootloader`. The bootloader kernel is the one in the SD card, and you use `rpi-install` to upload the actual kernel onto the Pi.

## Using this repo as a library
A couple of my other projects build off of this repository, using it as a library. You can reference my [Gaussian splatting](https://github.com/justiny7/pigs/tree/main) or [exposure fusion pipeline](https://github.com/justiny7/jankencamera/tree/main) projects to see how to do so - it just requires some minor tweaks in the Makefile!

