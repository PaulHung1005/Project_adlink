#!/bin/bash
# 
echo "==========================================================================="
echo "test connection to SSH host"
echo "==========================================================================="
if [ "eval $(ssh -T git@github.com-adlink | grep -q "authenticated")" != "" ] ; then
  echo "establish SSH connection to GitHub already"
else
  echo "==========================================================================="
  echo "establishing SSH connection to GitHub"
  echo "==========================================================================="
  ssh-keygen -t rsa -b 4096 -C "GitHub-ADLINK" -f ~/.ssh/id_rsa_github_adlink -N ""
  if [ $? == 0 ] ; then
    cat ~/.ssh/id_rsa_github_adlink.pub
    echo ""
    echo "1. copy above public key to clipboard"
    echo "2. press ENTER to continue, "
    echo "   will lead you to gitlab to add this public key to your SSH Key setting, "
    echo "   close the brower to continue following procedure"
    echo "wait for the browser..."
    firefox http://git@github.com
    echo ""
    while ! ssh -T git@github.com-adlink | grep -q "authenticated"
    do 
      echo "wait for the ssh..."
      sleep 1
    done  
  fi
fi

echo "==========================================================================="
echo "clone Ampere Altra ADLINK development platforms"
echo "==========================================================================="
# cd $HOME 
SILLICON_FAMILY=$1
if [ -z "$SILLICON_FAMILY" ]; then
  SILLICON_FAMILY="edk2-adlink-ampere"
fi
if [ -d "$SILLICON_FAMILY" ]; then
  read -p "Do you wish to override current $SILLICON_FAMILY folder?" yn
  case $yn in
    [Yy]* )   rm -rf $SILLICON_FAMILY;;
    * ) return 1;;
  esac
fi
git clone --recurse-submodules -j8 https://github.com/ADLINK/edk2-adlink-ampere.git $SILLICON_FAMILY
echo "==========================================================================="
echo "fetch submodules recursively"
echo "==========================================================================="
cd $SILLICON_FAMILY
if [ "eval $(ssh -T git@github.com-adlink | grep -q "authenticated")" != "" ] ; then
  echo "==========================================================================="
  echo "replace HTTPS access with SSH access if authenticated"
  echo "==========================================================================="
    git clone --recurse-submodules -j8 https://github.com/ADLINK/AmpereAltra-ATF-SCP.git 
    git clone --recurse-submodules -j8 --branch v2.10.100-ampere https://github.com/AmpereComputing/edk2-platforms.git 
    git clone --recurse-submodules -j8 https://github.com/Linaro/OpenPlatformPkg.git
    git clone --recurse-submodules -j8 --branch adlink https://github.com/ADLINK/edk2-ampere-tools.git 
    git clone --recurse-submodules -j8 --branch edk2-stable202402 https://github.com/tianocore/edk2.git 
    cd edk2/UnitTestFrameworkPkg/Library/SubhookLib/
    git clone https://github.com/tianocore/edk2-subhook subhook
    git submodule update --init --recursive
    cd ../../../
    git submodule update --init --recursive
    cd ..
  git remote set-url origin git@github.com-adlink:ADLINK/edk2-adlink-ampere.git
fi
echo "==========================================================================="
echo "set building environment"
echo "==========================================================================="
export WORKSPACE=$PWD
source edk2/edksetup.sh --reconfig
source adlink-platforms/Platform/Ampere/ComHpcAltPkg/edk2_adlink-ampere-altra/tools/edk2.sh
echo "==========================================================================="
echo "Ready to build !!!"
echo "==========================================================================="
