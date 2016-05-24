
#ifndef BASIC_H
#define BASIC_H


#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
#include <math.h>


#define MAXLINE     2048
#define NUMPROC       10
#define NUMSER	       4

#define DISP	     100
#define POSTI        101

struct impostazioni *imp;


#endif
