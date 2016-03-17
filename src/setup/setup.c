#include "../basic/basic.h"
#include "../procinfo/procinfo.h"
#include "../datiprocfiglio/datiprocfiglio.h"
#include "../lavorofiglio/lavorofiglio.h"
#include "../datiproc/datiproc.h"
#include "../processoprincipale/processoprincipale.h"
#include "../procsecondario/procsecondario.h"
#include "../infoserver/infoserver.h"
#include "../datipernuoviproc/datipernuoviproc.h"
#include "../impostazioni/impostazioni.h"
#include "../caricaimpostazioni/caricaimpostazioni.h"



int inizializzaporta(){
	struct sockaddr_in    servaddr;
	int listensd;


	if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("errore in socket");
		exit(1);
	}


	memset((void *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(imp->serv_port);
	


	if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in))) < 0) {
		perror("errore in bind");
		exit(1);
	}


	if (listen(listensd,imp->backlog) < 0 ) { 
		perror("errore in listen");
		exit(1);
	}

return listensd;
}


void *gestore_numero_processi_figli_disponibili(void *arg){		//thread per gestire il numero di 										//processi figli contemporaneamente 										//disponibili nel sistema
	
	struct datipernuoviproc *dati=arg;
	struct procinfo **pro=dati->procini;
	

	//struct procinfo *procini=*pro;
		
		//if(procini==NULL)

	if(*pro==NULL)
		//procini=aggiungiprocesso(procini,creaprocesso());	
		*pro=aggiungiprocesso(*pro,creaprocesso(dati->listens));

	struct procinfo *appoggio;
	int liberi;
	if(kill(getpid(),SIGUSR1)!=0)
		perror("kill");
	while(1){
	
		appoggio=*pro;
		liberi=1;
		while(appoggio->next!=NULL)
			{
				liberi++;
				appoggio=appoggio->next;

			}
		while(liberi<imp->procliberimin)
			{	
				aggiungiprocesso(*pro,creaprocesso(dati->listens));
				liberi++;
			}
		while(liberi>imp->procliberimax)
			{
			
					//da inseire il controllo su tutti i processi in esecuzione ed eliminarne quelli liberi fino ad averne in esecuzione un numero < di PROCLIBERIMAX
			}

	usleep(imp->durata_attesa);
	
	}
}



void sighandler(int segnale){
	
	if(segnale==SIGUSR1)
		printf("setup terminato\n");
	else 
		printf("%d\n",segnale);
	//signal(segnale, sighandler);
}




struct infoserver * setup(int listen){
	signal(SIGUSR1, sighandler);
	struct infoserver *inf=malloc(sizeof(struct infoserver));
	inf->pass=malloc(sizeof(struct datipernuoviproc));
	inf->pass->listens=listen;
	inf->pass->procini=malloc(sizeof(struct procinfo*));

	//*(inf->procini)=malloc(sizeof(struct procinfo));	
	//(*(inf->procini))->listensd=listens;
	//struct procinfo *procini2=NULL;
	//inf->procini=&procini2;
	int err=pthread_create(&inf->threadcontrollo,NULL,gestore_numero_processi_figli_disponibili,inf->pass);
	if(err!=0){
		fprintf (stderr,"Errore nel setup del server\n");
		exit(1);
	}
	pause();
	return inf;
}
