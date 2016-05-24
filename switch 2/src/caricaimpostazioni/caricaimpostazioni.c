#include "../basic/basic.h"
#include "../server_cluster/server_cluster.h"
#include "../procinfo/procinfo.h"
#include "../datiprocfiglio/datiprocfiglio.h"
#include "../lavorofiglio/lavorofiglio.h"
#include "../datiproc/datiproc.h"
#include "../processoprincipale/processoprincipale.h"
#include "../procsecondario/procsecondario.h"
#include "../infoserver/infoserver.h"
#include "../datipernuoviproc/datipernuoviproc.h"
#include "../impostazioni/impostazioni.h"
#include "../setup/setup.h"



struct impostazioni *inizzializzaimp()
{
	struct impostazioni *impo=malloc(sizeof(struct impostazioni));
	int j,id;
	for(j=0;j<NUMSER;j++)
	{
		id=shmget(IPC_PRIVATE,sizeof(struct server_cluster),IPC_CREAT | 0666);
		if(id==-1)
		{
			perror("shmemory");
			exit(1);
	
		}
	

	//impo->clust=calloc(NUMSER,sizeof(struct server_cluster));
		impo->clust[j]=shmat(id,NULL,0);
		if(impo->clust[j]==NULL)
		{
			perror("errore shamt");
			exit(1);		
		}
	}
	for(j=0;j<NUMSER;j++)
	{
		id=shmget(IPC_PRIVATE,sizeof(15*sizeof(char)),IPC_CREAT | 0666);
		if(id==-1)
		{
			perror("shmemory");
			exit(1);
	
		}
		impo->clust[j]->ip=shmat(id,NULL,0);
		if(impo->clust[j]->ip==NULL)
		{
			perror("errore shamt");
			exit(1);		
		}
		strcpy(impo->clust[j]->ip,"10.220.118.234");
		impo->clust[j]->carico=0;	
	}
	

	id=shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT | 0666);
	if(id==-1)
	{
		perror("shmemory");
		exit(1);
	}
	impo->posizione=shmat(id,NULL,0);
	*(impo->posizione)=0;
	impo->procliberimin=1;
	impo->procliberimax=6 ;
	impo->durata_attesa=300000 ;
	impo->thread_iniziali=5 ;
	impo->iterazioni_massime_thread=20 ;
	impo->massimo_thread_liberi=10 ;
	impo->minimo_thread_liberi=30 ;
	impo->log=0;
	impo->serv_port=5193;
	impo->backlog=10;
	impo->numproc=10;
	impo->vel_creazione=100;
	impo->processi_iniziali=3;
	impo->clust_port=5194;
	//impo->clust[1]->ip="10.10.10.11";
	//impo->clust[2]->ip="10.10.10.12";
	//impo->clust[3]->ip="10.10.10.13";
	impo->massimo_thread_tot=200;
	impo->procmax=10;
	impo->fill_port=5195;
	impo->tipo_scelta_cluster=1;
	impo->ritardo_lavoro=0;
	return impo;
}



struct impostazioni *caricaimpostazioni()
{
/*
valori per il file di impostazioni

PROCLIBERIMIN
PROCLIBERIMAX
PROCMAX
DURATA_ATTESA
THREAD_INIZIALI
ITERAZIONI_MASSIME_THREAD
MASSIMO_THREAD_LIBERI
MINIMO_THREAD_LIBERI
LOG
SERV_PORT
BACKLOG
NUMPROC
VEL_CREAZIONE
PROCESSI_INIZIALI
MASSIMO_THREAD_TOT
CLUST_PORT
CLUST_IP
CLUST_IP
CLUST_IP
CLUST_IP
FILL_PORT
TIPO_SCELTA_CLUSTER
RITARDO_LAVORO
*/


	int i=0,val,num=0;
	FILE *fd;
	char buff[50],app[11];
	struct impostazioni * imp;
	imp=inizzializzaimp();
	fd=fopen("impostazioni.txt","r");
	if( fd==NULL ) 
	{
		printf("Si e' verificato un errore in apertura del file di impostazioni, ");
		printf("verranno caricate quelle di default\n");
		return imp;				
	}

	while(1)
	{
	
		num=fscanf(fd,"%s",buff);
		if(num==EOF)
			break;
		if(strcmp("CLUST_IP",buff)==0)
		{
			num=fscanf(fd,"%s",app);
			if(num==EOF)
				break;
			/*imp->clust[i]->ip=calloc(strlen(app),sizeof(char));
			if(imp->clust[i]->ip==NULL)
			{	
				perror("malloc");
				exit(1);
			}*/
			memset((void *)imp->clust[i]->ip, 0, 15);
			strcpy(imp->clust[i]->ip,app);		
			i++;
			continue;
		}
		num=fscanf(fd,"%d",&val);
		if(num==EOF)
			break;
		
		else if(strcmp("PROCLIBERIMIN",buff)==0)
		imp->procliberimin=val;
		else if(strcmp("PROCLIBERIMAX",buff)==0)
		imp->procliberimax=val;
		else if(strcmp("DURATA_ATTESA",buff)==0)
		imp->durata_attesa=val;
		else if(strcmp("THREAD_INIZIALI",buff)==0)
		imp->thread_iniziali=val;
		else if(strcmp("ITERAZIONI_MASSIME_THREAD",buff)==0)
		imp->iterazioni_massime_thread=val;
		else if(strcmp("MASSIMO_THREAD_LIBERI",buff)==0)
		imp->massimo_thread_liberi=val;
		else if(strcmp("MINIMO_THREAD_LIBERI",buff)==0)
		imp->minimo_thread_liberi=val;
		else if(strcmp("LOG",buff)==0)
		imp->log=val;
		else if(strcmp("SERV_PORT",buff)==0)
		imp->serv_port=val;
		else if(strcmp("BACKLOG",buff)==0)
		imp->backlog=val;
		else if(strcmp("NUMPROC",buff)==0)
		imp->numproc=val;
		else if(strcmp("VEL_CREAZIONE",buff)==0)
		imp->vel_creazione=val;
		else if(strcmp("PROCESSI_INIZIALI",buff)==0)
		imp->processi_iniziali=val;
		else if(strcmp("CLUST_PORT",buff)==0)
		imp->clust_port=val;
		else if(strcmp("MASSIMO_THREAD_TOT",buff)==0)
		imp->massimo_thread_tot=val;	
		else if(strcmp("PROCMAX",buff)==0)
		imp->procmax=val;	
		else if(strcmp("FILL_PORT",buff)==0)
		imp->fill_port=val;
		else if(strcmp("TIPO_SCELTA_CLUSTER",buff)==0)
		imp->tipo_scelta_cluster=val;
		else if(strcmp("RITARDO_LAVORO",buff)==0)
		imp->ritardo_lavoro=val;
		if(imp->tipo_scelta_cluster==0)
		{
			int id2=shmget(IPC_PRIVATE,sizeof(pthread_mutex_t),IPC_CREAT | 0666);
			if(id2==-1)
			{
				perror("shmemory");
				exit(1);
	
			}
	

	//impo->clust=calloc(NUMSER,sizeof(struct server_cluster));
			imp->mutex_round=shmat(id2,NULL,0);
			if(imp->mutex_round==NULL)
			{
				perror("errore shamt");
				exit(1);		
			}

		}
		
	
	}
	
	fclose(fd);
	return imp;
}


