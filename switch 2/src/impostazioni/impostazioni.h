#ifndef	IMPOSTAZIONI_H
#define	IMPOSTAZIONI_H


struct impostazioni
{

	int procliberimin;
	int procliberimax;
	int durata_attesa;
	int thread_iniziali;
	int iterazioni_massime_thread;
	int massimo_thread_liberi;
	int minimo_thread_liberi;
	int log;
	int serv_port;
	int backlog;
	int numproc;
	int processi_iniziali;
	int vel_creazione;
	int clust_port;
	int massimo_thread_tot;
	int procmax;
	int fill_port;
	struct server_cluster * clust[NUMSER];
	int *posizione;
	int tipo_scelta_cluster;
	int ritardo_lavoro;
	pthread_mutex_t * mutex_round;
};


#endif
