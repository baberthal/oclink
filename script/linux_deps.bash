#!/bin/bash

HIDAPI_GIT_URL=https://github.com/signal11/hidapi.git
HIDAPI_CONFIG_ARGS="--prefix=/usr/local"

Red="\e[31m"
Green="\e[32m"
Blue="\e[34m"
ResetColor="\e[0m"

if [[ $TRAVIS_OS_NAME != linux ]]; then
    printf "${Red}Exiting, because this isn't linux\n"
    exit 1
fi


echo "Creating temp directory /tmp/deps"
mkdir -p "/tmp/deps" || exit 1
cd "/tmp/deps"

printf "${Green}Cloning HIDAPI from ${ResetColor}${Blue}${HIDAPI_GIT_URL}${ResetColor}\n"

git clone "${HIDAPI_GIT_URL}" hidapi && cd hidapi

printf "${Green}Done...${ResetColor}\n"

printf "${Green}Installing libudev...${ResetColor}\n"

./bootstrap
./configure "${HIDAPI_CONFIG_ARGS}" && make && sudo make install


# vim: set ts=8 sw=4 tw=0 ft=sh noet :
