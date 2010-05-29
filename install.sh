#!/bin/bash

export APES_CC_FLAGS="-Wall -Werror -Wno-format -std=c99"
export MJPEG_CC_FLAGS="-DNB_DECODER=2 -DDISPATCH_TIME -DINFO -DVERBOSE"

APES_CC_OPTIMIZATIONS="-g -mlittle-endian -O3 -mfpu=fpa -march=armv6"
APES_CC_OPTIMIZATIONS+=" -DDNA_ENABLE_LOG=INFO_LEVEL"
export APES_CC_OPTIMIZATIONS

export APES_COMPILER="arm-sls-dnaos-gcc"
export APES_LINKER="arm-sls-dnaos-gcc"
export APES_LINKER_FLAGS="-mfpu=fpa -mlittle-endian -march=armv6 -T$PWD/ldscript"
