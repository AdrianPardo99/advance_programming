# Archivo Makefile
# @author Adrian Gonzalez Pardo
PROJ=app
DSTPATH=/usr/local/bin
CFLAGS=-Wall
COBJ=-c
LINKF=-lm -pthread
CC=gcc
SRCC=$(wildcard *.c)
OBJS=$(SRCC:.c=.o)

all: ${PROJ}

%.o:	%.c
	${CC} ${COBJ} $< ${CFLAGS}

${PROJ}: ${OBJS}
	${CC} $^ -o $@ ${LINKF}
	echo ${PROJ} > .gitignore

.PHONY: clean install clean

clean:
	rm -f *.o ${PROJ}

install: all
	if [ ! -d ${DSTPATH} ]; then\
		sudo mkdir ${DSTPATH}; \
	fi
	sudo cp ${PROJ} ${DSTPATH}

uninstall:
	sudo rm ${DSTPATH}/${PROJ}
