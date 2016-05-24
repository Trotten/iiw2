#ifndef DATIUTILIAITHREAD_H
#define DATIUTILIAITHREAD_H


struct datiutiliaithread
{
	struct threadnumero* numero;
	int datiaccept;
	int pip[4];
	int fd_log;
	//da inserire tutti i campi utili per l'accept che il thread dovrà eseguire
};


#endif


