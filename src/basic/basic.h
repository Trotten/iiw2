
#ifndef BASIC_H
#define BASIC_H


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>


#define MAXLINE     1024
#define NUMPROC       10

#define DISP	     100
#define POSTI        101

struct impostazioni *imp;

#endif
