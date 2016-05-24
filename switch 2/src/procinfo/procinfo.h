#ifndef PROCINFO_H
#define PROCINFO_H

//struttura per il salvataggio dei dati del processo principale relativi ad ogni nuovo processo

struct procinfo
{
	pid_t procpid;                   //pid del processo
	pthread_t threadread;		 //pthread del thread che gestisce la pipe in lettura
	int  	disponibile;		 //disponibilità del processo 1 disponibile 0 no
	int   	postiliberi;		 //posti liberi per nuove connessioni
	int	pip[4];			 //pipe per la comunicazione fra processi
	struct procinfo *next;		 //puntatore alla struttura successiva
	struct procinfo *prec;		 //puntatore alla struttura precedente
	
};


#endif
