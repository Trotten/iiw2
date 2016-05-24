#include "basic/basic.h"
#include "io/io.h"
#include "server_cluster/server_cluster.h"
#include "impostazioni/impostazioni.h"
#include "procinfo/procinfo.h"
#include "datiprocfiglio/datiprocfiglio.h"
#include "lavorofiglio/lavorofiglio.h"
#include "datiproc/datiproc.h"
#include "infoserver/infoserver.h"
#include "datipernuoviproc/datipernuoviproc.h"
#include "processoprincipale/processoprincipale.h"
#include "procsecondario/procsecondario.h"
#include "caricaimpostazioni/caricaimpostazioni.h"
#include "setup/setup.h"



int main(){
    printf("ciao\n");
	imp=caricaimpostazioni();		
	struct infoserver *info;
	int listensd=inizializzaporta(imp->serv_port);	
	
	info=setup(listensd);

printf("ciao\n");
	struct procinfo *procini2=*(info->pass->procini);
//imp->prova=2;
//printf("padre=%d\n",imp->prova);	
	if(procini2==NULL)
	{
		printf("Errore nel setup del server\n");
		exit(1);
	}
	if(imp->tipo_scelta_cluster==1)
	{
		int connsd,num,carico;
		int servsd=inizializzaporta(imp->fill_port);	
//struct server_cluster* buff=malloc(sizeof(struct server_cluster));
		char* appoggio=malloc(sizeof(char)*15);
		struct sockaddr_in clientaddr;
		socklen_t clientaddr_size = sizeof(clientaddr);
		while(1){		
			//lavoro padre	
			memset(appoggio, 0, sizeof(char)*15);
				
			if ((connsd = accept(servsd, (struct sockaddr *)&clientaddr, &clientaddr_size)) < 0) 
			{
				perror("errore in accept");
				exit(1);
			}		
			appoggio=inet_ntoa(clientaddr.sin_addr);
			printf("IP=%s\n",inet_ntoa(clientaddr.sin_addr));
			num=readn(connsd,&carico,sizeof(int));
			if(num<0)
			{		
				perror("errore in read carico"); 
			      	exit(1);
			}
			printf("ecco il carico=%d\n\n\n\n\n",carico);
			
			for(num=0;num<NUMSER;num++)
				if(strcmp(imp->clust[num]->ip,appoggio)==0)
				{
					imp->clust[num]->carico=carico;
					break;
				}
			for(num=0;num<NUMSER;num++)
				if(imp->clust[num]->carico<imp->clust[(*(imp->posizione))]->carico)
					(*(imp->posizione))=num;		
			
		}
	}
	//while(1);
	pause();


	exit(0);
}

