#ifndef SETUP_H
#define SETUP_H


int inizializzaporta(int port);
void sighandler(int segnale);
struct infoserver * setup(int listens);


#endif
