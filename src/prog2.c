
#include "basic/basic.h"
#include "procinfo/procinfo.h"
#include "datiprocfiglio/datiprocfiglio.h"
#include "lavorofiglio/lavorofiglio.h"
#include "datiproc/datiproc.h"
#include "processoprincipale/processoprincipale.h"
#include "procsecondario/procsecondario.h"
#include "setup/setup.h"
#include "infoserver/infoserver.h"
#include "datipernuoviproc/datipernuoviproc.h"



int main(){	

	struct impostazioni *imp=caricaimpostazioni();
	struct infoserver *info;
	int listensd=inizializzaporta();	
	info=setup(listensd);








pause();	//DA TOGLIERE
	struct procinfo *procini2=*(info->pass->procini);
		//printf("%d\n",(int)info);
		//printf("%d     %d        %d\n",(int)info->procini,(int)*info->procini,(int)procini2);
		//printf("%d\n",(int)procini2->procpid);
	struct lavorofiglio *c=malloc(sizeof(struct lavorofiglio));
	c->dacambiare=99;
	int g=write(procini2->pip[1],c, sizeof(struct lavorofiglio));
		if (g == -1) {
			fprintf(stderr, "Error while writing file\n");
			exit(EXIT_FAILURE);
		}

	
int pinnolone=0;
	while(1){
		//lavoro padre
			pinnolone++;
			sleep(1);
		
	}


	exit(0);
}
