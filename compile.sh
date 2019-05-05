#! /bin/bash

PROGNAME=mu

DEBUG_FLAGS="-O0 -g3 -ggdb"
RELEASE_FLAGS="-O3"
COMMON_FLAGS="-Wall"

LINK_FLAGS="-lm -ljack"

FLAGS=${COMMON_FLAGS}
if [[ $1 == "debug" ]]; then
	echo Compile for debugging.
	FLAGS="${FLAGS} ${DEBUG_FLAGS}"
else

	FLAGS="${FLAGS} ${RELEASE_FLAGS}"
fi

mkdir -p bin
gcc ${FLAGS} src/*.c ${LINK_FLAGS} -o bin/${PROGNAME}



