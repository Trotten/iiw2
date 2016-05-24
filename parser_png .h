/* daytime_serverTCP.c - code for daytime server program that uses TCP
 Tratto da W.R. Stevens, "Network Programming Vol. 1"
 Ultima revisione: 14 gennaio 2008 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SERV_PORT   5195
#define BACKLOG       10
#define MAXLINE     1024

int main(int argc, char **argv)
{
    int           listensd, connsd;
    struct sockaddr_in    servaddr;
    int          buff;
    time_t        ticks;
    
    if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* crea il socket */
        perror("errore in socket");
        exit(1);
    }
    
    memset((void *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta
                                                   connessioni su una qualunque delle sue intefacce di rete */
    servaddr.sin_port = htons(SERV_PORT); /* numero di porta del server */
    
    /* assegna l'indirizzo al socket */
    if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
        perror("errore in bind");
        exit(1);
    }
    
    if (listen(listensd, BACKLOG) < 0 ) {
        perror("errore in listen");
        exit(1);
    }
    if ((connsd = accept(listensd, (struct sockaddr *)NULL, NULL)) < 0) {
        perror("errore in accept");
        exit(1);
    }
    for ( ; ; ) {
        
        if (read(connsd,&buff,sizeof(int))<0){
            perror("Errore nella read");
            exit(EXIT_FAILURE);
        }
        
//        if ((float)(buff/SONS)<0.25){
//            printf("Meno del 25\n");
//        }
//        else if ((float)(buff/SONS)<0.50){
//            printf("Meno del 50\n");
//        }
//        else if ((float)(buff/SONS)<0.75){
//            printf("Meno del 75\n");
//        }
//        else if ((float)(buff/SONS)<1){
//            printf("Meno del 100\n");
//        }
//        else {
//            printf("Carico\n");
//        }
        
        printf("%d\n",buff);
        
        
    }
    exit(0);
}