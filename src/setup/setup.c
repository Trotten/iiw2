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

struct impostazioni *inizzializzaimp(){
struct impostazioni *impo=malloc(sizeof(struct impostazioni));
impo->procliberimin=1;
impo->procliberimax=6 ;
impo->durata_attesa=300000 ;
impo->thread_iniziali=5 ;
impo->iterazioni_massime_thread=20 ;
impo->massimo_thread_liberi=100 ;
impo->minimo_thread_liberi=30 ;
return impo;
}



struct impostazioni *caricaimpostazioni(){
/*
valori per il file di impostazioni
PROCLIBERIMIN
PROCLIBERIMAX
DURATA_ATTESA
THREAD_INIZIALI
ITERAZIONI_MASSIME_THREAD
MASSIMO_THREAD_LIBERI
MINIMO_THREAD_LIBERI

*/


int num=0;
int val;
FILE *fd;
char buff[20];
struct impostazioni * imp;
imp=inizzializzaimp();
fd=fopen("impostazioni.txt","r");
if( fd==NULL ) {
	printf("Si e' verificato un errore in apertura del file\n");
	exit(1);
}

while(1)
{

	num=fscanf(fd,"%s",buff);
	if(num==EOF)
		break;

	num=fscanf(fd,"%d",&val);
	if(num==EOF)
		break;
	
	if(strcmp("PROCLIBERIMIN",buff)==0)
	imp->procliberimin=val;
	if(strcmp("PROCLIBERIMAX",buff)==0)
	imp->procliberimax=val;
	if(strcmp("DURATA_ATTESA",buff)==0)
	imp->durata_attesa=val;
	if(strcmp("THREAD_INIZIALI",buff)==0)
	imp->thread_iniziali=val;
	if(strcmp("ITERAZIONI_MASSIME_THREAD",buff)==0)
	imp->iterazioni_massime_thread=val;
	if(strcmp("MASSIMO_THREAD_LIBERI",buff)==0)
	imp->massimo_thread_liberi=val;
	if(strcmp("MINIMO_THREAD_LIBERI",buff)==0)
	imp->minimo_thread_liberi=val;
}

fclose(fd);

return imp;
}





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
	servaddr.sin_port = htons(SERV_PORT);


	if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in))) < 0) {
		perror("errore in bind");
		exit(1);
	}


	if (listen(listensd, BACKLOG) < 0 ) { 
		perror("errore in listen");
		exit(1);
	}

return listensd;
}











void *gestorenumeroprocessifiglidisponibili(void *arg){			//thread per gestire il numero di 										//processi figli contemporaneamente 										//disponibili nel sistema
	
	struct datipernuoviproc *dati=arg;
	struct procinfo **pro=dati->procini;
	

	//struct procinfo *procini=*pro;
		
		//if(procini==NULL)
	printf("%d\n",(int)*pro);
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
		while(liberi<PROCLIBERIMIN)
			{
				aggiungiprocesso(*pro,creaprocesso(dati->listens));
				liberi++;
			}
		while(liberi>PROCLIBERIMAX)
			{
			
					//da inseire il controllo su tutti i processi in esecuzione ed eliminarne quelli liberi fino ad averne in esecuzione un numero < di PROCLIBERIMAX
			}

	usleep(DURATA_ATTESA);
	
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

	int err=pthread_create(&inf->threadcontrollo,NULL,gestorenumeroprocessifiglidisponibili,inf->pass);
	if(err!=0){
		fprintf (stderr,"Errore nel setup del server\n");
		exit(1);
	}
	pause();
	return inf;
}
