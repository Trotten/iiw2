#ifndef SETUP_H
#define SETUP_H

int inizializzaporta();
void *gestore_numero_processi_figli_disponibili(void *arg);
void sighandler(int segnale);
struct infoserver * setup(int listens);

#endif
