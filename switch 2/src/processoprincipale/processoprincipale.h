#ifndef PROCESSOPRINCIPALE_H
#define PROCESSOPRINCIPALE_H


void elimina_proc(struct procinfo **pro,struct procinfo *appoggio);
struct procinfo *alloc_node(void);
void *attesapipefiglio(void *arg);
void creathreadpergestioneprocesso(struct procinfo *data);
struct procinfo * creaprocesso(int dati,int fd_log);
struct procinfo * aggiungiprocesso(struct procinfo *procini,struct procinfo *newproc);
void *gestore_numero_processi_figli_disponibili(void *arg);


#endif
