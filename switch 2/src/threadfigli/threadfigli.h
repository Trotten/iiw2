#ifndef THREADFIGLI_H
#define THREADFIGLI_H


void incrementaconttorepadre(struct threadnumero *numerothreadprinc);
void decrementaconttorepadre(struct threadnumero *numerothreadprinc);
void incrementapadre(struct threadnumero *numerothreadprinc);
void decrementapadre(struct datiutiliaithread *dati);
void *lavorothreadfigli(void *arg);


#endif
