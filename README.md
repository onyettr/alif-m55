ALIF M55 HE port
=================
Targetting ALIF Ensemble REV_A1 device.

This is based on the bare-minimum port


## Installation

- Download Micropython package
- Download alif-m55 from GitHub

## Building
```
$ cd ports/alif-m55
$ make (V=1 if you want more details)
```

The output will be`build/firmware.elf` (and also corresponding `.bin` and `.dfu` files).  

## Cygwin build Issues
py/mkenv.mk add SHELL=bash if you see Permission Denied errors
