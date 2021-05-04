#!/bin/sh

ubuntu_version=$(lsb_release -r)
ubuntu_version=$(cut -f2 <<< "$ubuntu_version")

mkdir ~/.milo/ -p
cd ~/.milo/
mkdir thirdparty -p
cd thirdparty

# Install Apt Libraries

read -r -p "Do you want to install joystick dependency? [y/N] " response
if [[ "$response" =~ ^([yY][eE][sS]|[yY])+$ ]]
then
	sudo apt install joystick jstest-gtk
fi

sudo apt-get install libboost-all-dev

sudo apt install libopencv-dev python3-opencv

sudo apt install libswscale-dev libavcodec-dev libavutil-dev

# Install latest CMake release

read -r -p "Do you want to install latest version of CMake [y/N] " response
if [[ "$response" =~ ^([yY][eE][sS]|[yY])+$ ]]
then
	git clone -b "release" "https://github.com/Kitware/CMake"
	cd CMake
	./bootstrap ; make ; sudo make install
	cd ..
fi
