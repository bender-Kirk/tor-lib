#!/bin/sh
GREEN='\033[32m'
GRAS='\033[1m'


printf "${GREEN}${GRAS}instalation des dépendances :${RESET}\n\n"
apt install libcurl4-openssl-dev && apt install tor
printf "\n"
