#ifndef SETUP_H
#define SETUP_H

struct impostazioni *caricaimpostazioni();
int inizializzaporta();
void *gestorenumeroprocessifiglidisponibili(void *arg);
void sighandler(int segnale);
struct infoserver * setup(int listens);

#endif
