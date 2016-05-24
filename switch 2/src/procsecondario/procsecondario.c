#include "../basic/basic.h"
#include "../impostazioni/impostazioni.h"
#include "../procinfo/procinfo.h"
#include "../datiprocfiglio/datiprocfiglio.h"
#include "../datiutiliaithread/datiutiliaithread.h"
#include "../lavorofiglio/lavorofiglio.h"
#include "../datiproc/datiproc.h"
#include "../processoprincipale/processoprincipale.h"
#include "../threadnumero/threadnumero.h"
#include "../threadfigli/threadfigli.h"



int suicidio=0;
void *attesapipepadre(void *arg)
{
//funzione che esegue il thread del processo figlio per rimanere in ascolto in lettura sulla pipe di
//comunicazione con il processo principale  
	
	struct datiprocfiglio *data=(struct datiprocfiglio *)arg;
	struct lavorofiglio * datiletti= malloc(sizeof(struct lavorofiglio));
	int v;	
	while(1)
	{
			
		v = read(data->pip[0], datiletti,sizeof(struct lavorofiglio));
		if(v == -1) 
		{
			fprintf(stderr, "Error while reading file\n");
			exit(EXIT_FAILURE);
		}
		if(v==0)
		{	
			free(datiletti);
			pthread_exit(0);
			
		}
		suicidio=datiletti->suicidio;
		
	}
}



void *gestorenumero(void *arg)
{
	struct datiutiliaithread * datidapassare=(struct datiutiliaithread *)arg;	
	pthread_t threadsupp;
	int err,max=0;	
	sleep(1);
	while(suicidio==0)
	{	//if(datidapassare->numero->threadliberi!=0)
		
		if(datidapassare->numero->threadliberi<imp->minimo_thread_liberi && datidapassare->numero->threadliberi+datidapassare->numero->threadoccupati<imp->massimo_thread_tot)
		{
		
			err=pthread_create(&(threadsupp),NULL,lavorothreadfigli,datidapassare);
			//printf ("creazione nuovo thread  %d %d\n",datidapassare->numero->threadliberi,(int)threadsupp);
			if(err!=0)
			{

				//fprintf (stderr,"Errore nella creazione del nuovo thread\n");
				//exit(1);
			
				break;
			}


		}
//			printf("liberi=%d occupati=%d  pid=%d\n",datidapassare->numero->threadliberi,datidapassare->numero->threadoccupati,getpid());
		if(!(datidapassare->numero->threadliberi+datidapassare->numero->threadoccupati<imp->massimo_thread_tot*0.90))
		{
			if(max==0)
			{	
				//printf("va a 1 %d\n",getpid());
				struct datiproc *avviso1=malloc(sizeof(struct datiproc));
				avviso1->tipo=POSTI;
				avviso1->valore=0;
				int g=write(datidapassare->pip[3],avviso1, sizeof(struct datiproc));
				if (g == -1) 
				{
					fprintf(stderr, "Errore nella comunicazione con il padre\n");
					exit(EXIT_FAILURE);
				}				
				max=1;
				free(avviso1);
			}
		}
		//printf("max=%d  lib=%d  occ=%d pid=%d\n",max,datidapassare->numero->threadliberi,datidapassare->numero->threadoccupati,getpid());
		if(max==1 && (datidapassare->numero->threadliberi+datidapassare->numero->threadoccupati)<(imp->massimo_thread_tot*0.75))
		{	
			//printf("va a 0 %d\n",getpid());
			max=0;
			struct datiproc *avviso2=malloc(sizeof(struct datiproc));
			avviso2->tipo=POSTI;
			avviso2->valore=1;
			int g=write(datidapassare->pip[3],avviso2, sizeof(struct datiproc));
			if (g == -1) 
			{
				fprintf(stderr, "Errore nella comunicazione con il padre\n");
				exit(EXIT_FAILURE);
			}
			free(avviso2);
		}
		usleep(imp->vel_creazione);
	}
	if(suicidio==0)
	{
		struct datiproc *avviso3=malloc(sizeof(struct datiproc));
		avviso3->tipo=DISP;
		avviso3->valore=0;
		int g=write(datidapassare->pip[3],avviso3, sizeof(struct datiproc));
		if (g == -1) 
		{
			fprintf(stderr, "Errore nella comunicazione con il padre\n");
			exit(EXIT_FAILURE);
		}
		free(avviso3);
	}
	//printf("muore %d\n",getpid());
	pthread_exit(0);

}


pthread_t creathreadpergestionepipepadre(struct datiprocfiglio *data)
{										//funzione per la creazione del
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


void setupthreadiniziali(struct datiutiliaithread *datithread)
{

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
void lavproc(struct datiprocfiglio *datiproc)
{							//pipe 0 lettura 3 scrittura 
	

	struct datiprocfiglio * info=datiproc;		//salvataggio dati passati alla creazione

	if(close(info->pip[1])!=0)			//chiusura delle pipe che nn servono
		perror("Close");
	if(close(info->pip[2])!=0)
		perror("Close");
	
	creathreadpergestionepipepadre(info);
	/*pthread_t readthreadp=creathreadpergestionepipepadre(info);*/	//pthread_t del thread in ascolto 										//sulla pipe con il padre
	
	struct datiutiliaithread *spaziodati=malloc(sizeof(struct datiutiliaithread));
	spaziodati->datiaccept=info->acceptdati;
	spaziodati->fd_log=info->fd_log;
	spaziodati->pip[0]=info->pip[0];
	spaziodati->pip[3]=info->pip[3];
	spaziodati->numero=malloc(sizeof(struct threadnumero));
		//struct threadnumero *spazionumeri=malloc(sizeof(struct threadnumero));
	spaziodati->numero->threadliberi=0;
	spaziodati->numero->threadoccupati=0;
		//spazionumeri->threadliberi=0;
	spaziodati->numero->suicidio=&suicidio;

	setupthreadiniziali(spaziodati);
 /*pause();		//DA TOGLIERE	
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
*/

	//struct datiproc *nuovo=malloc(sizeof(struct datiproc));
	while(1)
	{
		if(suicidio==1&&spaziodati->numero->threadoccupati==0)
		{
			//printf("muore %d\n",getpid());
			exit(0);
		}
		sleep(1);
	}
	exit(0);
}
