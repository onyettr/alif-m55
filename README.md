ALIF M55 HE port
=================
Target: ALIF Ensemble REV_A1 device.

This is based on the bare-arm port (which defaults to stm32)

On execution, this port has two *pre-canned* demos included. 

## Evaluation Board
- UART2 is used for output from the Evaluation board. (Baurd rate = 115200)
- GPIO14 is enabled when the program starts and is disabled before exit. 

## Installation

- Download Micropython package
    - ZIP / TAR file [micropython.org.download](https://micropython.org/download/micropython-1.19.1.tar.xz)
    - git clone git@github.com:micropython/micropython.git
- Download alif-m55 from GitHub (onyettr)

### Prerequisites
- CMSIS (0.4.1) Packs need to be installed
- gcc version 11.3.1 20220712 (Arm GNU Toolchain 11.3.Rel1)
- SERAM V0.63.0

## Building
This has been built on
- cygwin
- Fedora
- Ubuntu

### Compilation
```
$ cd micropython-1.19.1
$ cd ports/alif-m55
$ make 
$ OR
$ make V=1     (if you want more details)
```
This will create `build/firmware.elf` (and also corresponding `.bin` and `.dfu` files). 

This ELF image can now be loaded using SEGGER Ozone to the M55_1 (HE) CPU. 

### MRAM build
TCM loading is used for this example

```
$ make install
$ cd ../app-release-exe
$ app-gen-toc -f build/config/m55-upython.json
$ app-write-mram 
```
## Running
Evaluation board uses UART4

## Limitations
- CLANG / ARM-DS has not been tested
- CMSIS integration is rough

## Porting issues - Heap and stack size
Using the default sizes from the LD file caused a Hard Fault on execution. Increasing the size of the Heap and Stack made this go away 

No analysis has been done on what is the optimal size for these. 

## Cygwin Issues
py/mkenv.mk add SHELL=bash if you see Permission Denied errors