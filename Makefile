CC = gcc
CFLAGS = -pthread -Wall -Wextra -O4  -g
CSFLAGS =  -pthread -Wall -Wextra -O4 -g -DDEBUG -lm
DIR = src
SUB_DIRS := ${DIR}/*
SRC = $(wildcard ${DIR}/*/*.c)
APP_WEB_SWITCH  = prog2
OBJ = $(SRC:.c=.o) ${DIR}/${APP_WEB_SWITCH}.o

all: ${OBJ}
	${CC} ${CFLAGS} -c -o ${DIR}/${APP_WEB_SWITCH}.o ${DIR}/${APP_WEB_SWITCH}.c
	${CC} ${DIR}/${APP_WEB_SWITCH}.o ${SRC} -o ${APP_WEB_SWITCH} ${CSFLAGS}

${DIR}/%/*.o : %.h

clean:
	rm -f ${APP_WEB_SWITCH};
	rm -f ${DIR}/*.o;
	rm -f ${DIR}/*/*.o;

print:
	echo ${DIR}/*/*.c
