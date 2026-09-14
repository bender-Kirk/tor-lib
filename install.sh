#!/bin/sh

RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
RESET='\033[0m'
GRAS='\033[1m'


printf "${GREEN}${GRAS}instalation des dépendances :${RESET}\n\n"
apt install libcurl4-openssl-dev && apt install tor
printf "\n"
