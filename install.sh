#!/bin/bash

export APES_CC_FLAGS="-Wall -Werror -Wno-format -std=c99"
export MJPEG_CC_FLAGS="-DNB_DECODER=2 -DDISPATCH_TIME -DINFO -DVERBOSE"

APES_CC_OPTIMIZATIONS="-g -mlittle-endian -O3 -mfpu=fpa -march=armv6zk"
export APES_CC_OPTIMIZATIONS

export APES_COMPILER="arm-sls-dnaos-gcc"
export APES_LINKER="arm-sls-dnaos-gcc"
export APES_LINKER_FLAGS="-mfpu=fpa -mlittle-endian -march=armv6zk -T$PWD/ldscript"

# export DNACORE_CC_FLAGS="-DDNA_ENABLE_LOG=VERBOSE_LEVEL"
unset DNACORE_CC_FLAGS
