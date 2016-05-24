#include "../basic/basic.h"
#include "../procinfo/procinfo.h"
#include "../datiprocfiglio/datiprocfiglio.h"
#include "../lavorofiglio/lavorofiglio.h"
#include "../datiproc/datiproc.h"
#include "../procsecondario/procsecondario.h"
#include "../infoserver/infoserver.h"
#include "../datipernuoviproc/datipernuoviproc.h"
#include "../impostazioni/impostazioni.h"
#include "../caricaimpostazioni/caricaimpostazioni.h"





void elimina_proc(struct procinfo **pro,struct procinfo *appoggio)
{
	while((*pro)->prec!=NULL)
		*pro=(*pro)->prec;		
	if((*pro)->procpid==appoggio->procpid)
	{
		*pro=appoggio->next;
		if(*pro!=NULL)
			(*pro)->prec=NULL;
		
	}
	else
	{
		if(appoggio->next!=NULL)
		{
			appoggio->prec->next=appoggio->next;
			appoggio->next->prec=appoggio->prec;
		}
		else
		{
			appoggio->prec->next=NULL;
		}
	}	
	//if(close(appoggio->pip[1])!=0)  			
	//	perror("Close qui");
	//if(close(appoggio->pip[2])!=0)
	//	perror("Close qui");
	//free(appoggio);
}





struct procinfo *alloc_node()				//funzione per allocare un nuovo nodo della struttura 								//procinfo e creare le pipe necessarie
{
	struct procinfo *p = malloc(sizeof(struct procinfo));
	if(p == NULL) 
	{
	        fprintf(stderr, "Memory allocation error\n");
	        exit(EXIT_FAILURE);
	}
	if(pipe(p->pip)<0) 
	{
		perror("Pipe");
		exit(1);
	}
	if(pipe((p->pip)+2)<0) 
	{
		perror("Pipe");
		exit(1);
	}
	return p;
}



void *attesapipefiglio(void *arg)
{
	//funzione che esegue il thread del processo principale per rimanere in ascolto in lettura sulla pipe 		//di comunicazione con un processo figlio
	
	struct procinfo *data=(struct procinfo *)arg;
	struct datiproc * datiletti= malloc(sizeof(struct datiproc));		
	int v;
	while(1){		
		v = read(data->pip[2], datiletti,sizeof(struct datiproc));
		if(v == -1) 
		{
			fprintf(stderr, "Errore nella lettura della pipe\n");
			exit(EXIT_FAILURE);
		}
		if(v==0)
		{
			waitpid(data->procpid,NULL,0);
			pthread_exit(0);

		}
		switch(datiletti->tipo)
		{						//interpretazione della struttura letta e 
								//aggiornamento dei parametri ricevuti
			case DISP:
				data->disponibile=datiletti->valore;
				if(data->disponibile==0)
					if(kill(data->procpid,SIGKILL)!=0)
						perror("kill");
			break;
			case POSTI:
				data->postiliberi=datiletti->valore;
			break;
		}
		
		//printf("Dati letti  TIPO=%d   VALORE=%d  PID=%d\n",datiletti->tipo,datiletti->valore,data->procpid);
	}
}





void crea_thread_per_gestione_pipe_figlio(struct procinfo *data)
{										//funzione per la creazione del
										//thread in ascolto sulla pipe 											//con il processo figlio

	int err=pthread_create(&(data->threadread),NULL,attesapipefiglio,data);
	if(err!=0)
	{
		fprintf (stderr,"Errore nella creazione del nuovo thread\n");
		exit(1);

	}
}



struct procinfo * creaprocesso(int dati,int fd_log)
{				
								//funzione per la creazione di un nuovo 								//processo figlio, chiusura delle pipe non 									//necessarie nel processo principale e 
								//creazione del thread in ascolto
	struct procinfo *procfiglio=alloc_node();
	struct datiprocfiglio *datifiglio=malloc(sizeof(struct datiprocfiglio));
	int i;
	for(i=0;i<4;i++)
	{
		datifiglio->pip[i]=procfiglio->pip[i];
	}
	datifiglio->acceptdati=dati;
	datifiglio->fd_log=fd_log;
	if((procfiglio->procpid=fork())==0) 			//creazione processo figlio
		lavproc(datifiglio);
	
	procfiglio->disponibile=1;
	procfiglio->postiliberi=1;
								//chiusura pipe non necessarie
	if(close(procfiglio->pip[0])!=0)  			//pipe 2 lettura 1 scrittura
		perror("Close");
	if(close(procfiglio->pip[3])!=0)
		perror("Close");
	crea_thread_per_gestione_pipe_figlio(procfiglio);		//creazione thread in ascolto
	return procfiglio;

}


struct procinfo * aggiungiprocesso(struct procinfo *procini,struct procinfo *newproc)
{
							//funzione per l'aggiunta della struttura dati del 
							//nuovo processo figlio nella lista dei processi figli 								//attivi presente nel processo principale restituendo 								//il primo elemento
	if(procini==NULL)
		return newproc;
		
	struct procinfo *app=procini;
	while(app->next!=NULL)
		app=app->next;		
	app->next=newproc;
	newproc->prec=app;	
	return procini;
}



void *gestore_numero_processi_figli_disponibili(void *arg)
{									//thread per gestire il numero di 										//processi figli contemporaneamente 										//disponibili nel sistema
	
	struct datipernuoviproc *dati=arg;
	struct procinfo **pro=dati->procini;
	

	//struct procinfo *procini=*pro;
		
		//if(procini==NULL)

	if(*pro==NULL)
		//procini=aggiungiprocesso(procini,creaprocesso());	

		*pro=aggiungiprocesso(*pro,creaprocesso(dati->listens,dati->fd_log));

	struct procinfo *appoggio;
	int liberi,tot;
	if(kill(getpid(),SIGUSR1)!=0)
		perror("kill");
	for(liberi=0;liberi<(imp->processi_iniziali-1);liberi++)
		     aggiungiprocesso(*pro,creaprocesso(dati->listens,dati->fd_log));
	while(1)
	{
		appoggio=*pro;
		if(appoggio->disponibile==1 && appoggio->postiliberi==1)
			liberi=1;
		else
			liberi=0;
		tot=1;
		//printf("disp=%d posti=%d\n",appoggio->disponibile,appoggio->postiliberi);
		appoggio=appoggio->next;
		while(appoggio!=NULL && appoggio->next!=NULL)
		{
			tot++;
			//printf("disp=%d posti=%d\n",appoggio->disponibile,appoggio->postiliberi);
			if(appoggio->disponibile==1 && appoggio->postiliberi==1)
			{
				liberi++;
				appoggio=appoggio->next;				
			}
			else
				if(appoggio->disponibile==0)
				{
					struct procinfo *a=appoggio->next;
					elimina_proc(pro,appoggio);
					appoggio=a;
					tot--;
				}
				else
					appoggio=appoggio->next;
			}
		if(appoggio!=NULL)
		{	
			//printf("disp=%d posti=%d\n",appoggio->disponibile,appoggio->postiliberi);
			tot++;
			if(appoggio->disponibile==1 && appoggio->postiliberi==1)
				liberi++;
		}
	//printf("liberi=%d\n",liberi);	
		while(liberi<imp->procliberimin && tot<imp->procmax)
		{	
			aggiungiprocesso(*pro,creaprocesso(dati->listens,dati->fd_log));
			liberi++;
			tot++;
					
		}
		if(liberi>imp->procliberimax)
		{	
			appoggio->disponibile=0;
			struct lavorofiglio *c=malloc(sizeof(struct lavorofiglio));
			c->suicidio=1;
			int g=write(appoggio->pip[1],c, sizeof(struct lavorofiglio));
			if (g == -1) 
			{
				fprintf(stderr, "Errore nello scrivere sulla pipe del figlio \n");
				exit(EXIT_FAILURE);
			}
			elimina_proc(pro,appoggio);	
			
		}
		usleep(imp->durata_attesa);

	}
}


