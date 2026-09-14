#!/bin/sh
GREEN='\033[32m'
GRAS='\033[1m'
RESET='\033[0m'


printf "${GREEN}${GRAS}Installing dependencies:${RESET}\n\n"
apt install libcurl4-openssl-dev && apt install tor
printf "\n"
