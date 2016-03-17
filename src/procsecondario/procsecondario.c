#include "../basic/basic.h"
#include "../impostazioni/impostazioni.h"
#include "../procinfo/procinfo.h"
#include "../datiprocfiglio/datiprocfiglio.h"
#include "../lavorofiglio/lavorofiglio.h"
#include "../datiproc/datiproc.h"
#include "../processoprincipale/processoprincipale.h"
#include "../threadnumero/threadnumero.h"
#include "../threadfigli/threadfigli.h"
#include "../datiutiliaithread/datiutiliaithread.h"





void *attesapipepadre(void *arg){
//funzione che esegue il thread del processo figlio per rimanere in ascolto in lettura sulla pipe di
//comunicazione con il processo principale  
	
	struct datiprocfiglio *data=(struct datiprocfiglio *)arg;
	struct lavorofiglio * datiletti= malloc(sizeof(struct lavorofiglio));
	int v;	
	while(1){
			
		v = read(data->pip[0], datiletti,sizeof(struct lavorofiglio));
			if (v == -1) {
				fprintf(stderr, "Error while reading file\n");
				exit(EXIT_FAILURE);
			}
		printf("Thf  %d\n",datiletti->dacambiare);
	}
}



void *gestorenumero(void *arg){
	struct datiutiliaithread * datidapassare=(struct datiutiliaithread *)arg;

	pthread_t threadsupp;
	int err;	
	while(1)
	{
		usleep(imp->vel_creazione);
		if(datidapassare->numero->threadliberi<imp->minimo_thread_liberi)
		{
		
			err=pthread_create(&(threadsupp),NULL,lavorothreadfigli,datidapassare);
			printf ("creazione nuovo thread  %d %d\n",datidapassare->numero->threadliberi,(int)threadsupp);
			if(err!=0)
			{
				fprintf (stderr,"Errore nella creazione del nuovo thread\n");
				exit(1);

			}


		}
	}

}


pthread_t creathreadpergestionepipepadre(struct datiprocfiglio *data){		//funzione per la creazione del
										//thread in ascolto sulla pipe 											//con il processo principale
	pthread_t readthread;
	int err=pthread_create(&(readthread),NULL,attesapipepadre,data);
	if(err!=0)
	{
		fprintf (stderr,"Errore nella creazione del nuovo thread\n");
		exit(1);

	}
	return readthread;
}


void setupthreadiniziali(struct datiutiliaithread *datithread){

	int nt;
	pthread_t threadsupp;
	int err;
	for(nt=0;nt<imp->thread_iniziali;nt++)
	{	
		err=pthread_create(&(threadsupp),NULL,lavorothreadfigli,datithread);
		if(err!=0)
		{
			fprintf (stderr,"Errore nella creazione del nuovo thread\n");
			exit(1);
		}
		
	}
	pthread_t gestorenum;
	err=pthread_create(&(gestorenum),NULL,gestorenumero,datithread);
	if(err!=0)
	{
		fprintf (stderr,"Errore nella creazione del nuovo thread\n");
		exit(1);

	}

}




//corpo del processo figlio
void lavproc(struct datiprocfiglio *datiproc){		//pipe 0 lettura 3 scrittura 
	

	struct datiprocfiglio * info=datiproc;		//salvataggio dati passati alla creazione

	if(close(info->pip[1])!=0)			//chiusura delle pipe che nn servono
		perror("Close");
	if(close(info->pip[2])!=0)
		perror("Close");

	pthread_t readthreadp=creathreadpergestionepipepadre(info);	//pthread_t del thread in ascolto 										//sulla pipe con il padre

	struct datiutiliaithread *spaziodati=malloc(sizeof(struct datiutiliaithread));
	spaziodati->datiaccept=info->acceptdati;
	spaziodati->numero=malloc(sizeof(struct threadnumero));
		//struct threadnumero *spazionumeri=malloc(sizeof(struct threadnumero));
	spaziodati->numero->threadliberi=0;
		//spazionumeri->threadliberi=0;

	setupthreadiniziali(spaziodati);
 pause();		//DA TOGLIERE	
	struct datiproc *nuovo=malloc(sizeof(struct datiproc));
	nuovo->tipo=POSTI;
	nuovo->valore=4;

	printf("Figlio   %d    %d  \n",(int)getpid(),(int)readthreadp);
	int g=write(info->pip[3],nuovo, sizeof(struct datiproc));
	if (g == -1) {
		fprintf(stderr, "Error while writing file\n");
		exit(EXIT_FAILURE);
	}
	
	
	sleep(20);

	exit(0);
}
