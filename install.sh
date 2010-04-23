#!/bin/bash

#
# Configuration
#

export TARGET_CFLAGS="-Wall -Werror -Wno-format -std=c99"

TARGET_COPTS="-g -mlittle-endian -O3 -mfpu=fpa -march=armv6"
TARGET_COPTS+=" -DNB_DECODER=2 -DDISPATCH_TIME -DINFO"
TARGET_COPTS+=" -DDNA_ENABLE_LOG=INFO_LEVEL"
export TARGET_COPTS

export TARGET_CC="arm-sls-dnaos-gcc"
export TARGET_LD="arm-sls-dnaos-gcc"
export TARGET_LDFLAGS="-mfpu=fpa -mlittle-endian -march=armv6 -T$PWD/ldscript"

#
# Display information
#

echo "[Tool Chain settings]"
echo "| CC       : ${TARGET_CC}"
echo "| CFLAGS   : ${TARGET_CFLAGS}"
echo "| COPTS    : ${TARGET_COPTS}"
echo "| LD       : ${TARGET_LD}"
echo "| LDFLAGS  : ${TARGET_LDFLAGS}"
