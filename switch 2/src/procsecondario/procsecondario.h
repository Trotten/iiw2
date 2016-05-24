#ifndef PROCSECONDARIO_H
#define PROCSECONDARIO_H


void *attesapipepadre(void *arg);
void *gestorenumero(void *arg);
pthread_t creathreadpergestionepipepadre(struct datiprocfiglio *data);
void lavproc(struct datiprocfiglio *datiproc);


#endif
