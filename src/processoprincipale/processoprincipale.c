#include "../basic/basic.h"
#include "../procinfo/procinfo.h"
#include "../datiprocfiglio/datiprocfiglio.h"
#include "../lavorofiglio/lavorofiglio.h"
#include "../datiproc/datiproc.h"
#include "../procsecondario/procsecondario.h"






struct procinfo *alloc_node()				//funzione per allocare un nuovo nodo della struttura 								//procinfo e creare le pipe necessarie
{
    struct procinfo *p = malloc(sizeof(struct procinfo));
    if (p == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
	if (pipe(p->pip)<0) 
	{
		perror("Pipe");
		exit(1);
	}
	if (pipe((p->pip)+2)<0) 
	{
		perror("Pipe");
		exit(1);
	}
   // p->next=NULL;
   // p->prec=NULL;
    return p;
}



void *attesapipefiglio(void *arg){
	//funzione che esegue il thread del processo principale per rimanere in ascolto in lettura sulla pipe 		//di comunicazione con un processo figlio
	
	struct procinfo *data=(struct procinfo *)arg;
	struct datiproc * datiletti= malloc(sizeof(struct datiproc));		
	int v;
	
	while(1){		
		v = read(data->pip[2], datiletti,sizeof(struct datiproc));
		if (v == -1) {
			fprintf(stderr, "Error while reading file\n");
			exit(EXIT_FAILURE);
		}
		switch(datiletti->tipo){			//interpretazione della struttura letta e 
								//aggiornamento dei parametri ricevuti
		case DISP:
			data->disponibile=datiletti->valore;
		break;
		case POSTI:
			data->postiliberi=datiletti->valore;
		break;
		}
		
		printf("Dati letti  %d   %d  \n",datiletti->tipo,datiletti->valore);
		printf("Dati letti  %d   %d  \n",data->disponibile,data->postiliberi);
	}
}





void creathreadpergestionepipefiglio(struct procinfo *data){			//funzione per la creazione del
										//thread in ascolto sulla pipe 											//con il processo figlio

	int err=pthread_create(&(data->threadread),NULL,attesapipefiglio,data);
	if(err!=0)
	{
		fprintf (stderr,"Errore nella creazione del nuovo thread\n");
		exit(1);

	}
}



struct procinfo * creaprocesso(int daacc){				
								//funzione per la creazione di un nuovo 								//processo figlio, chiusura delle pipe non 									//necessarie nel processo principale e 
								//creazione del thread in ascolto
	struct procinfo *procfiglio=alloc_node();
	struct datiprocfiglio *datifiglio=malloc(sizeof(struct datiprocfiglio));
	int i;
	for(i=0;i<4;i++)
	{
		datifiglio->pip[i]=procfiglio->pip[i];
	}
	datifiglio->acceptdati=daacc;
	if((procfiglio->procpid=fork())==0){ 			//creazione processo figlio
		
		lavproc(datifiglio);
	}
	
	procfiglio->disponibile=1;
								//chiusura pipe non necessarie
	if(close(procfiglio->pip[0])!=0)  			//pipe 2 lettura 1 scrittura
		perror("Close");
	if(close(procfiglio->pip[3])!=0)
		perror("Close");
	printf("Padre\n");
	creathreadpergestionepipefiglio(procfiglio);		//creazione thread in ascolto
	return procfiglio;

}


struct procinfo * aggiungiprocesso(struct procinfo *procini,struct procinfo *newproc){
							//funzione per l'aggiunta della struttura dati del 
							//nuovo processo figlio nella lista dei processi figli 								//attivi presente nel processo principale restituendo 								//il primo elemento
	if(procini==NULL)
	{
		return newproc;
/*
		procini=newproc;
		return;	
*/
	}		
		
	struct procinfo *app=procini;
	while(app->next!=NULL)
		app=app->next;		
	app->next=newproc;
	newproc->prec=app;
//      return;	
	return procini;
}

