#include "../basic/basic.h"
#include "../io/io.h"
#include "../server_cluster/server_cluster.h"
#include "../setup_cluster/setup_cluster.h"
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





int inizializzaporta(int port)
{
	struct sockaddr_in    servaddr;
	int listensd;
	if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("errore in socket");
		exit(1);
	}

	int yes=1;
	//char yes='1'; // use this under Solaris

	if (setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
	{
		perror("setsockopt");
		exit(1);
	}
	memset((void *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(port);
	
	if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in))) < 0) 
	{
		perror("errore in bind");
		exit(1);
	}


	if (listen(listensd,imp->backlog) < 0 ) 
	{ 
		perror("errore in listen");
		exit(1);
	}

	return listensd;
}




void sighandler(int segnale)
{
	
	if(segnale==SIGUSR1)
		printf("setup terminato\n");
	else 
		printf("%d\n",segnale);
	//signal(segnale, sighandler);
}




struct infoserver * setup(int listen)
{
	signal(SIGUSR1, sighandler);

	int i=0,sockfd;
	struct sockaddr_in servaddr;
	struct setup_cluster* buff=calloc(sizeof(struct setup_cluster),1);
	//for(i=0;i<NUMSER;i++){
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		{
        		perror("errore in socket");
			exit(1);
		}    
	        memset((void *)&servaddr, 0, sizeof(servaddr));
	        servaddr.sin_family = AF_INET;
	        servaddr.sin_port = htons(imp->clust_port);
	        if (inet_pton(AF_INET, imp->clust[i]->ip, &servaddr.sin_addr) <= 0) 
		{
			fprintf(stderr, "errore in inet_pton per %s\n", imp->clust[i]->ip);
        		exit(1);
        	}   
		
        	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) 
		{
			perror("errore in connect");
			exit(1);
		}
		buff->carico=imp->tipo_scelta_cluster;
		buff->porta_carico=imp->fill_port;
		if (writen(sockfd,buff,sizeof(buff))<0)
		{
		      	perror("Errore in write con il cluster per il setup"); 
		      	exit(1);
    		}	
		//sleep(1);
		if (close(sockfd) == -1)
		{  		// chiude la connessione 
	      		perror("Errore in close");
	      		exit(1);
	    	}

	//}
	free(buff);





	
    	int fd=0;
	if(imp->log==1)
	{
		fd=open("testo_output.txt",O_CREAT|O_RDWR|O_APPEND,0666);
    		if (fd==-1)
		{
        		perror("Errore nella open");
        		exit(EXIT_FAILURE);
    		}
	}
	struct infoserver *inf=malloc(sizeof(struct infoserver));
	inf->pass=malloc(sizeof(struct datipernuoviproc));
	inf->pass->listens=listen;
	inf->pass->procini=calloc(1,sizeof(struct procinfo*));
	inf->pass->fd_log=fd;
	//*(inf->procini)=malloc(sizeof(struct procinfo));	
	//(*(inf->procini))->listensd=listens;
	//struct procinfo *procini2=NULL;
	//inf->procini=&procini2;
	int err=pthread_create(&inf->threadcontrollo,NULL,gestore_numero_processi_figli_disponibili,inf->pass);
	if(err!=0)
	{
		fprintf (stderr,"Errore nel setup del server\n");
		exit(1);
	}

	pause();

			
	return inf;
}
