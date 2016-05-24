#include "../basic/basic.h"
#include "../server_cluster/server_cluster.h"
#include "../impostazioni/impostazioni.h"
#include "../procinfo/procinfo.h"
#include "../datiprocfiglio/datiprocfiglio.h"
#include "../lavorofiglio/lavorofiglio.h"
#include "../datiproc/datiproc.h"
#include "../processoprincipale/processoprincipale.h"
#include "../procsecondario/procsecondario.h"
#include "../threadnumero/threadnumero.h"
#include "../datiutiliaithread/datiutiliaithread.h"
#include "../io/io.h"



pthread_mutex_t mutex_numero_disponibili = PTHREAD_MUTEX_INITIALIZER;


void incrementacontatorepadre(struct threadnumero *numerothreadprinc)
{

	pthread_mutex_lock( &mutex_numero_disponibili );
	numerothreadprinc->threadoccupati--;
	if(numerothreadprinc->threadliberi>=imp->massimo_thread_liberi||(*(numerothreadprinc->suicidio))==1)
	{	
		//printf("ADDIO %d\n",(int)pthread_self());
		pthread_mutex_unlock( &mutex_numero_disponibili );
		pthread_exit(0);
	}
	else
		numerothreadprinc->threadliberi++;
	//printf("thread incrementa valore a %d   %d\n",numerothreadprinc->threadliberi,(int)pthread_self());
	pthread_mutex_unlock( &mutex_numero_disponibili );

}


void decrementacontatorepadre(struct threadnumero *numerothreadprinc)
{

	pthread_mutex_lock( &mutex_numero_disponibili );
	numerothreadprinc->threadliberi--;
	numerothreadprinc->threadoccupati++;	
	//printf("thread decrementa valore a %d   %d\n",numerothreadprinc->threadliberi,(int)pthread_self());
	pthread_mutex_unlock( &mutex_numero_disponibili );

}

void incrementapadre(struct threadnumero *numerothreadprinc)
{
	pthread_mutex_lock( &mutex_numero_disponibili );
	numerothreadprinc->threadoccupati++;
	pthread_mutex_unlock( &mutex_numero_disponibili );
}

void decrementapadre(struct datiutiliaithread *dati)
{

	pthread_mutex_lock( &mutex_numero_disponibili );
	dati->numero->threadoccupati--;	
	if(dati->numero->threadoccupati==0 && dati->numero->threadliberi==0)
	{
		struct datiproc *avviso=malloc(sizeof(struct datiproc));
		avviso->tipo=DISP;
		avviso->valore=0;
		int g=write(dati->pip[3],avviso, sizeof(struct datiproc));
		if (g == -1) 
		{
			fprintf(stderr, "Errore nella comunicazione con il padre\n");
			exit(EXIT_FAILURE);
		}
		free(avviso);//forse ok

	}
	pthread_mutex_unlock( &mutex_numero_disponibili );

}


void *lavorothreadfigli(void *arg)
{

	struct datiutiliaithread *datiutili=(struct datiutiliaithread *)arg;
	int cont,connsd,sockfd,num_serv ;
	struct sockaddr_in servaddr;
	char* buff=malloc(sizeof(char)*MAXLINE);
	char* buff2=malloc(sizeof(char)*MAXLINE);

	incrementapadre(datiutili->numero);
	for(cont=0;cont<imp->iterazioni_massime_thread;cont++)
	{
		incrementacontatorepadre(datiutili->numero);
		//printf("figlio=%d\n",imp->prova);
		
//accept sulla porta bloccante
		if ((connsd = accept(datiutili->datiaccept, (struct sockaddr *)NULL, NULL)) < 0) 
		{
			perror("errore in accept con il client");
			exit(1);
		}
//printf("accept   %d\n",(int)pthread_self());
			
		decrementacontatorepadre(datiutili->numero);


//aggiungere tutta la parte di comunicazione con il cluster di server
		int s=read(connsd,buff,sizeof(char)*MAXLINE);
		if(s<0)
		{
			
			perror("errore in read con il client"); 
		      	exit(1);
		}

		//printf("%s\n\n\n\n\n",buff);

		if(imp->log==1)
		{
			//fare il lock sul file e scrivere il log
	        	if ((myflock(datiutili->fd_log,LOCK_EX))==-1)
			{
				perror("Errore nella flock\n");
				exit(EXIT_FAILURE);
		        }
			if(writen(datiutili->fd_log,buff,strlen(buff))==-1)
				fprintf(stderr,"Errore nella scrittura sul file di log\n");
	
			//write_file(datiutili->fd_log,buff);
	      		if ((myflock(datiutili->fd_log,LOCK_UN))==-1)
			{
	        		perror("Errore nella flock\n");
	        		exit(EXIT_FAILURE);
	        	}
		}	

//printf("\n%s\n",buff);


//scelta server del cluster




		
		
		if(imp->tipo_scelta_cluster==0)				
		{
//prendere il lock globale
			if(pthread_mutex_lock(imp->mutex_round)!=0)
			{
				perror("errore nel lock");
				exit(1);
			}
			num_serv=*(imp->posizione);
			//fprintf(stderr,"pos:%d\n",*(imp->posizione));
			(*(imp->posizione))=(num_serv+1)%(NUMSER);

//rilasciare il lock globale
			if(pthread_mutex_unlock(imp->mutex_round)!=0)
			{
				perror("errore nel lock");
				exit(1);
			}
		

		}
		else{
			num_serv=*(imp->posizione);
		}
//invio richiesta al cluster

       		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		{
        		perror("errore in socket");
			exit(1);
		}    
	        memset((void *)&servaddr, 0, sizeof(servaddr));
	        servaddr.sin_family = AF_INET;
	        servaddr.sin_port = htons(imp->clust_port);
	        if (inet_pton(AF_INET, imp->clust[num_serv]->ip, &servaddr.sin_addr) <= 0) 
		{
			fprintf(stderr, "errore in inet_pton per %s\n", imp->clust[num_serv]->ip);
        		exit(1);
        	}   
		/* si connette al server */
        	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) 
		{
			perror("errore in connect");
			if (close(sockfd) == -1)
			{  		// chiude la connessione 
	      			perror("Errore in close");
	      			exit(1);
	    		}
			continue;
			//exit(1);
		}
		if (writen(sockfd,buff,strlen(buff))<0)
		{
		      	perror("Errore in write con il cluster"); 
		      	exit(1);
    		}
		memset(buff,0,sizeof(char)*MAXLINE);
		int u=readn(sockfd,buff,sizeof(char)*MAXLINE);
		if(u<0)
		{	
			perror("errore in read con il cluster"); 
			if (close(sockfd) == -1)
			{  		// chiude la connessione 
	      			perror("Errore in close");
	      			exit(1);
	    		}
			continue;
		      	//exit(1);
		}
		if (close(sockfd) == -1)
		{  		// chiude la connessione 
	      		perror("Errore in close");
	      		exit(1);
	    	}
//printf("\n%s\n",buff);



/*
		memset(buff2,0,sizeof(char)*MAXLINE);
		strcpy (buff2,buff);
		memset(buff,0,sizeof(char)*MAXLINE);
		int i=0;
		while(*(buff2+i)!='\n')
		{
			*(buff+i)=*(buff2+i);
			i++;
		}
		memset(buff2,0,sizeof(char)*MAXLINE);
		i=0;
		if(*buff=='G' && *(buff+1)=='E' && *(buff+2)=='T')
		{
			while(*(buff+i+4)!=' ')
			{
				*(buff2+i)=*(buff+i+4);
				i++;
			}
		}
		*(buff2+i)='\0';
		//printf("%s\n",buff2);
		//if(strcmp(buff2,"/favicon.ico")==0)
		//	ico=1;
		memset(buff,0,sizeof(char)*MAXLINE);
*/



		












		//scegliere il server del cluster 

		//aprire una connessione con il server scelto


		//inviare la richiesta ricevuta al server 


		//leggere la risposta del server

		//inviare la risposta al client





		
		//char* buff2="HTTP/1.1 200 OK\nContent-length: 258\nContent-Type: text/html\n\n<!doctype html><meta content=\"text/html;charset=utf-8\" http-equiv=\"Content-Type\"><meta content=\"utf-8\" http-equiv=\"encoding\"><html xmlns=\"http://www.w3.org/1999/xhtml\"><body><H1>Hello world<p><a href=\"prova.html\">CLICCA PER PROVA</a></p></H1></body></html>";
//strcpy (str,"strings ");
//strcat (str,"have been ");
//char* buff2="HTTP/1.1 200 OK\nContent-length: "+(char*)strlen(bu)+"\nContent-Type: text/html\n\n"+bu;
		//unsigned int h;

		//for(h=0;h<strlen(buff2);h++)
		//	buff[h]=buff2[h];

//		char* bu="<!doctype html>\n<meta content=\"text/html;charset=utf-8\" http-equiv=\"Content-Type\">\n<meta content=\"utf-8\" http-equiv=\"encoding\">\n<html>\n<body>\n<H1>\n\tHello world\n\t<p>\n\t<a href=\"prova.html\">CLICCA PER PROVA</a>\n\t</p>\n</H1>\n</body>\n</html>";

/*
		if(ico==1)
		{	
		
			bu=malloc(4287*sizeof(char));
			//FILE *fdi=fopen("icon.ico","r");
			int fdi=open("icon.ico",O_RDONLY);
			if(fdi==-1)
			{
				perror("open\n");
				exit(1);
			}
			read_file(fdi,bu,4287);
			//int y=0;
			//while((*(bu+y)=fgetc(fdi))!=EOF)
			//	{
			//	printf("%c\n",*(bu+y));
			//	y++;}
			
		}
*/
/*
		strcpy (buff,"HTTP/1.1 200 OK\nContent-length: ");
		snprintf(buff+strlen(buff), MAXLINE-strlen(buff), "%d", strlen(bu));

		
		
		//strcat (buff,"\nContent-Type: image/x-icon\n\n");
		
		strcat (buff,"\nContent-Type: text/html\n\n");

		
		
		strcat (buff,bu);

		//printf("%s\n\n\n\n\n\n\n",buff);
*/
//strcpy (buff,"HTTP/1.1 404 Not Found\nConnection: close\n\n");
			
		
		if (writen(connsd,buff,strlen(buff))<0)
		{
		      	perror("Errore in write"); 
		      	exit(1);
    		}

		//printf("write\n");
//	sleep(10);
	 	if (close(connsd) == -1)
		{  		// chiude la connessione 
	      		perror("Errore in close");
	      		exit(1);
	    	}	
		//printf("close\n");

	
			//lavoro thread per gestire una richiesta	
	usleep(round(imp->ritardo_lavoro));
	}
	decrementapadre(datiutili);
	//printf("SONO VECCHIOOOOOOOOOOOOOOOOOOOOO\n");
	free(buff);
	free(buff2);
	pthread_exit(0);
}

