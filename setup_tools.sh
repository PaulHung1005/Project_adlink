#!/bin/bash
# 
# detect install system
# 
APT_CMD=$(which apt)
APT_GET_CMD=$(which apt-get)
DNF_CMD=$(which dnf)
YUM_CMD=$(which yum)
if [[ ! -z $DNF_CMD ]]; then
  INSTALLER=dnf
  UUID_LIB=libuuid-devel
elif [[ ! -z $YUM_CMD ]]; then
  INSTALLER=yum
  UUID_LIB=libuuid-devel
elif [[ ! -z $APT_CMD ]]; then
  INSTALLER=apt
  UUID_LIB=uuid-dev
elif [[ ! -z $APT_GET_CMD ]]; then
  INSTALLER=apt-get
  UUID_LIB=uuid-dev
else
  echo "error can't install tools"
  exit 1;
fi
echo "==========================================================================="
echo "update system before install tools"
echo "==========================================================================="
sudo $INSTALLER upgrade -y
sudo $INSTALLER autoremove -y
#
# for platform build, do not extract into VirtualBox shared folder, 
# or it will cause sumbolic link fail such as liblto_plugin.so cannot be linked from liblto_plugin.so.0.0.0.0
#
echo "==========================================================================="
echo "install tools" 
echo "==========================================================================="
if [[ $INSTALLER == "apt" || $INSTALLER == "apt-get" ]]; then
  sudo $INSTALLER install -y libssl-dev build-essential lzma uuid-dev gnu-efi
else
  sudo $INSTALLER install -y openssl-devel libuuid-devel
  wget http://mirror.centos.org/centos/8/PowerTools/x86_64/os/Packages/xz-lzma-compat-5.2.4-3.el8.x86_64.rpm
  sudo $INSTALLER localinstall -y xz-lzma-compat-5.2.4-3.el8.x86_64.rpm
  rm xz-lzma-compat-5.2.4-3.el8.x86_64.rpm
fi
sudo $INSTALLER install -y flex bison git
echo "==========================================================================="
echo "install python 3.6 sample"
echo "==========================================================================="
version=$(python -V 2>&1 | grep -Po '(?<=Python )(.+)')
echo $version
parsedVersion=${version:0:3}
if [[ -z "$version" || "$parsedVersion" != "3.6" ]] ; then
  if [[ $INSTALLER == "apt" || $INSTALLER == "apt-get" ]]; then
      sudo add-apt-repository -y ppa:deadsnakes/ppa
      sudo apt update -y
      sudo apt install -y python3.6 # 3.6 works for tag 1.07
  else
    sudo dnf install -y python3
  fi
  plink=/usr/bin/python
  if [ -L ${plink} ]; then
    sudo unlink ${plink}
  fi 
  sudo ln -s /usr/bin/python3.6 ${plink}
fi
sudo apt install -y python3.6-distutils python3-pip
echo "==========================================================================="
