#!/bin/bash
export WORKSPACE=$PWD
#
# DEVEL_MODE 
# 1. disable PXE/HTTP boot
# 2. no BIOS capsule generated
# 3. generate .img only.
#
DEVEL_MODE=0
#
# Firmware Version
#
  VER=2.10
  BUILD=100.03_BMC_disabled_VIAVI_AARONN
# DEBUG=0

. adlink-platforms/Platform/Ampere/ComHpcAltPkg/edk2_adlink-ampere-altra/tools/make_adlink.sh ComHpcAlt A2
