#! /bin/bash

PROGNAME=mu
SRCDIR=src

DEBUG_FLAGS="-O0 -g3 -ggdb"
RELEASE_FLAGS="-O3"
COMMON_FLAGS="-Wall -std=c99 -fstrict-aliasing"
LINK_FLAGS="-lm -ljack"


FLAGS=${COMMON_FLAGS}
if [[ $1 == "debug" ]]; then
	echo Compile for debugging.
	FLAGS="${FLAGS} ${DEBUG_FLAGS}"
else
	FLAGS="${FLAGS} ${RELEASE_FLAGS}"
fi

gcc ${FLAGS} ${SRCDIR}/*.c ${LINK_FLAGS} -o ${SRCDIR}/${PROGNAME}



