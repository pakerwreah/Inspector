#!/bin/sh

GREEN='\033[0;32m'
CYAN='\033[0;36m'
NC='\033[0m'

if [ -f /.dockerenv ]; then
	printf "\n💡 ${GREEN}Building outside volume for better performance: ${CYAN}../cmake-build-debug${NC}\n\n"
	cmake -B../cmake-build-debug && make -C ../cmake-build-debug -j
else
	cmake -Bcmake-build-debug && make -C cmake-build-debug -j
fi
