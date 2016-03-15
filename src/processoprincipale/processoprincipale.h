
#ifndef PROCESSOPRINCIPALE_H
#define PROCESSOPRINCIPALE_H

struct procinfo *alloc_node(void);
void *attesapipefiglio(void *arg);
void creathreadpergestioneprocesso(struct procinfo *data);
struct procinfo * creaprocesso(int daacc);
struct procinfo * aggiungiprocesso(struct procinfo *procini,struct procinfo *newproc);

#endif
