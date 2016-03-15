#include "../basic/basic.h"
#include "../procinfo/procinfo.h"
#include "../datiprocfiglio/datiprocfiglio.h"
#include "../lavorofiglio/lavorofiglio.h"
#include "../datiproc/datiproc.h"
#include "../processoprincipale/processoprincipale.h"
#include "../procsecondario/procsecondario.h"
#include "../threadnumero/threadnumero.h"
#include "../datiutiliaithread/datiutiliaithread.h"


pthread_mutex_t mutex_numero_disponibili = PTHREAD_MUTEX_INITIALIZER;


void incrementaconttorepadre(struct threadnumero *numerothreadprinc)
{

	pthread_mutex_lock( &mutex_numero_disponibili );
	if(numerothreadprinc->threadliberi>=MASSIMO_THREAD_LIBERI)
	{	
		printf("ADDIO %d\n",(int)pthread_self());
		pthread_exit(0);
	}
	else
		numerothreadprinc->threadliberi++;
	//printf("thread incrementa valore a %d   %d\n",numerothreadprinc->threadliberi,(int)pthread_self());
	pthread_mutex_unlock( &mutex_numero_disponibili );

}


void decrementaconttorepadre(struct threadnumero *numerothreadprinc)
{

	pthread_mutex_lock( &mutex_numero_disponibili );

	numerothreadprinc->threadliberi--;
	//printf("thread decrementa valore a %d   %d\n",numerothreadprinc->threadliberi,(int)pthread_self());
	pthread_mutex_unlock( &mutex_numero_disponibili );

}



void *lavorothreadfigli(void *arg){

	struct datiutiliaithread *datiutili=(struct datiutiliaithread *)arg;
	int cont;
	for(cont=0;cont<ITERAZIONI_MASSIME_THREAD;cont++){
		incrementaconttorepadre(datiutili->numero);
			
		int connsd;
		if ((connsd = accept(datiutili->datiaccept, (struct sockaddr *)NULL, NULL)) < 0) {
			perror("errore in accept");
			exit(1);
		}
		printf("accept   %d\n",(int)pthread_self());
			//accept sulla porta bloccante

		sleep(1);
		decrementaconttorepadre(datiutili->numero);

		char* buff="HTTP/1.1 200 OK\nContent-length: 46\nContent-Type: text/html\n\n<html><body><H1>Hello world</H1></body></html>";



		//snprintf(buff, sizeof(buff), "%.24s\r\n","ciaoooooooooooo\n");


	if (write(connsd,buff,strlen(buff))<0) {
	      perror("errore in write"); 
	      exit(1);
    	}


	printf("write\n");

	 if (close(connsd) == -1) {  		// chiude la connessione 
	      perror("errore in close");
	      exit(1);
	    }	
	printf("close\n");

	
			//lavoro thread per gestire una richiesta	

	}

	pthread_exit(0);
}

