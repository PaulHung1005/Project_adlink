#!/bin/bash
OPATH=$PATH
export WORKSPACE=$PWD
. $WORKSPACE/adlink-platforms/Platform/Ampere/ComHpcAltPkg/edk2_adlink-ampere-altra/tools/edk2.sh
make -f $WORKSPACE/edk2-ampere-tools/Makefile clean
export PATH=$OPATH
