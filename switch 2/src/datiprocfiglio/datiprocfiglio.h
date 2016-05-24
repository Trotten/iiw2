#ifndef DATIPROCFIGLIO_H
#define DATIPROCFIGLIO_H

//struttura per il passaggio di informazioni alla creazione del nuovo processo figlio sempre da padre a figlio

struct datiprocfiglio
{

	int pip[4];		//pipe 0     	lettura processo figlio
				//pipe 1	scrittura processo principale
				//pipe 2	lettura processo principale
				//pipe 3	scrittura processo figlio
	int acceptdati;
	int fd_log;
};


#endif
