# Linux Assignment for OSES


This repository contains a single Yocto layer that includes two recipes: one for building and deploying the Linux user application (APP) and one for the Linux character-based driver (cDD)

# How to clone the repository
Simply clone the repository with the git command:
```sh
git clone https://github.com/nicosilverx/linux_assignment.git
```
# Yocto integration
From the ```/poky  ``` folder issue the commands
```
source oe-init-build-env <your_build>
bitbake-layers create-layer ../meta-ppg
``` 

Copy the content of the repository in your 
```/poky/meta-ppg  ``` folder. 

Switch back to the ```/poky``` folder and enter
```
bitbake-layers add-layer ../meta-ppg/
```
This tells to Yocto to integrate our new layer to the distribution. 

<!---
Then navigate to your build folder (for example ```/build_qemuarm```) and open the ```conf/bblayers.conf``` and in the ``` BBLAYERS ?= ``` add the line 
```   
/home/<your_username>/poky/meta-ppg \
```
-->

Open the ```conf/local.conf``` file and add to the end 
```
IMAGE_INSTALL_append = " mymod"
KERNEL_MODULE_AUTOLOAD += "mymod"

IMAGE_INSTALL_append = " mymod-app"
KERNEL_MODULE_AUTOLOAD += "mymod-app"
```
Switch back to the ```/poky``` folder and enter
```
source oe-init-build-env <your_build>
```
to build the new image use the command
```
bitbake core-image-minimal
```

# Testing on the Raspberry

Since I don't own a Raspberry Pi3 board, I tested the distribution on an older Raspberry Pi2 board, and everything worked fine. 
To compile the distro we need more recipes than before: from the ```/poky``` folder issue the commands:

```
git clone -b dunfell git://git.openembedded.org/meta-openembedded
git clone -b dunfellhttps://github.com/meta-qt5/meta-qt5
git clone -b dunfell git://git.yoctoproject.org/meta-security
git clone -b dunfell git://git.yoctoproject.org/meta-raspberrypi
```
Then, create a new build with 
```
source oe-init-build-env build_rpi2
```
Modify the ```/conf/bblayers.conf``` by adding the following lines in the ``` BBLAYERS ?= ```:
```
/home/yoctotrainee/poky/meta-openembedded/meta-oe \
/home/yoctotrainee/poky/meta-openembedded/meta-multimedia \
/home/yoctotrainee/poky/meta-openembedded/meta-networking \
/home/yoctotrainee/poky/meta-openembedded/meta-python \
/home/yoctotrainee/poky/meta-raspberrypi \
```

Then modify the ```/conf/local.con``` by adding the following lines:
``` 
MACHINE ?= "raspberrypi2"
ENABLE_UART = "1"

EXTRA_IMAGE_FEATURES += "debug-tweaks tools-debug eclipse-debug ssh-server-openssh"
IMAGE_INSTALL_append = " linux-firmware-rpidistro-bcm43455"
IMAGE_FSTYPES = "tar.xz ext3 rpi-sdimg"
``` 
As before, we need to add our recipe to the layer by:
``` 
bitbake-layers add-layer ../meta-ppg/
``` 
but this time we also need to modify the file in ```/poky/meta-ppg/recipes-ppg/mymod/mymod.bb``` and change the COMPATIBLE_MACHINE with ```raspberrypi2``` 

Then we need to build the system with
``` 
bitbake core-image-full-cmdline
``` 

To flash the SD card with the new image use the command:
``` 
sudo dd if=tmp/deploy/images/raspberrypi2/core-image-full-cmdline-raspberrypi2.rpi-sdimg of=/dev/<your_sdcard> bs=1M
``` 
