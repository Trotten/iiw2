/* daytime_serverTCP.c - code for daytime server program that uses TCP
 Tratto da W.R. Stevens, "Network Programming Vol. 1"
 Ultima revisione: 14 gennaio 2008 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <ifaddrs.h>

#define SERV_PORT   5194
#define SWITCH_PORT 5195
#define BACKLOG       100
#define MAXLINE     2048
#define SONS    50
#define SWITCH_SERVER "10.220.205.230"


pthread_mutex_t mutex,mutex2;
pthread_cond_t cond;

char* ip_switch;

struct setup_cluster{
    int carico;
    int porta_carico;
};
struct setup_cluster* cluster;

struct send_fill_server{
    int fill;
//    char* ip;
};

struct pthread_struct{
    pthread_t tid[SONS+1];
    int           listensd, connsd;
    struct sockaddr_in    servaddr;
    char *buff,*buff2;
    int n_son;
    
};

struct send_fill_server* send_fill;


void read_file(int fd, char* stringa,long size){
    long i=0;
    while(size>i){
        long written_byte=read(fd,stringa,size-i);
        if (written_byte==-1)
        {
            perror("Errore nella read 2");
            exit(EXIT_FAILURE);
        }
        if (written_byte==0)
            break;
        stringa=stringa+written_byte;
        i=i+written_byte;
    }
}




void richiesta(char* buff, char* buff2){
    memset(buff2,0,sizeof(char)*MAXLINE);
    if (strcpy (buff2,buff)==NULL){
        perror("strcpy\n");
        exit(EXIT_FAILURE);
    }
    memset(buff,0,sizeof(char)*MAXLINE);
    int i=0;
//    while(*(buff2+i)!='\n')
//    {
////        *(buff+i)=*(buff2+i);
//        i++;
//    }
    strcpy(buff,buff2);
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
    memset(buff,0,sizeof(char)*MAXLINE);
}



void* sons_work(struct pthread_struct* struct_var){

    int listensd=struct_var->listensd;
    int connsd=struct_var->connsd;
    struct sockaddr_in servaddr=struct_var->servaddr;
    char          *buff=calloc(MAXLINE,sizeof(char));
    if (buff==NULL){
        perror("Errore calloc\n");
        exit(EXIT_FAILURE);
    }
    char            *buff2=calloc(MAXLINE,sizeof(char));
    if (buff2==NULL){
        perror("Errore calloc\n");
        exit(EXIT_FAILURE);
    }
    char* bu=calloc(MAXLINE,sizeof(char));
    if (bu==NULL){
        perror("Errore calloc\n");
        exit(EXIT_FAILURE);
    }
    for ( ; ; ) {
        memset(bu,0,sizeof(char)*MAXLINE);
        memset(buff,0,sizeof(char)*MAXLINE);
        memset(buff2,0,sizeof(char)*MAXLINE);
        if ((connsd = accept(listensd, (struct sockaddr *)NULL, NULL)) < 0) {
            perror("errore in accept");
            exit(1);
        }
        if (read(connsd,buff,sizeof(char)*MAXLINE)<0){
            perror("Errore nella read");
            exit(EXIT_FAILURE);
        }
        if (cluster->carico && pthread_mutex_lock(&mutex)!=0){
            perror("lock");
            exit(EXIT_FAILURE);
        }
        send_fill->fill=send_fill->fill+1;
        
        if (cluster->carico && pthread_cond_signal(&cond)!=0){
            perror("cond");
            exit(1);
        }
        
        if (cluster->carico && pthread_mutex_unlock(&mutex)!=0){
            perror("lock");
            exit(EXIT_FAILURE);
        }
        
        //        printf("Sono il thread numero %d: connessione accettata\n",struct_var->n_son);
        richiesta(buff,buff2);
        //buff2="/index.html";
        
        if (strcmp(buff2,"/")==0){
            strcpy(buff2,"/index.html");
        }
//        printf("apro il file\n");
        int fd;
        fd=open(buff2+1,O_RDONLY);
        if (fd<0){
            if (strcmp(buff2,"/favicon.ico")==0){
                strcpy(buff,"HTTP/1.1 404 Not Found\nConnection: close");
            }
            else{
                
            strcpy(buff,"HTTP/1.1 404 Not Found\nConnection: close\nContent-length: ");
            memset(buff2,0,MAXLINE*sizeof(char));
            strcpy(buff2,"/Error.html");
            fd=open(buff2+1,O_RDONLY);
            if (fd<0){
                perror("errore open\n");
                exit(EXIT_FAILURE);
            }
            }
            
        }
        else {
            
            strcpy (buff,"HTTP/1.1 200 OK\nContent-length: ");
            
        }
        
        
        if (!(fd<0)){
        read_file(fd,bu,MAXLINE);
            if (close(fd)<0){
                perror("fclose\n");
                exit(EXIT_FAILURE);
            }
        
        
        
        
        snprintf(buff+strlen(buff), MAXLINE-strlen(buff), "%d", strlen(bu));
        
        
        
        //strcat (buff,"\nContent-Type: image/x-icon\n\n");
        
        strcat (buff,"\nContent-Type: text/html\n\n");
        
        
        
        strcat (buff,bu);
        }
        
        /* scrive sul socket di connessione il contenuto di buff */
        if (write(connsd, buff, strlen(buff)) != strlen(buff)) {
            perror("errore in write");
            exit(1);
        }
        

        
        if (cluster->carico && pthread_mutex_lock(&mutex)!=0){
            perror("lock");
            exit(EXIT_FAILURE);
        }
        send_fill->fill=send_fill->fill-1;
        
        if (cluster->carico && pthread_cond_signal(&cond)!=0){
            perror("cond");
            exit(1);
        }
        
        if (cluster->carico && pthread_mutex_unlock(&mutex)!=0){
            perror("lock");
            exit(EXIT_FAILURE);
        }
        if (close(connsd) == -1) {  /* chiude la connessione */
            perror("errore in close");
            exit(1);
        }
        
    }
    
}

void* controller(void){
    int              sockfd, n;
    int             recvline;
    struct sockaddr_in   servaddr;
    int perc=0;
    int perc1=0;
    int fill_old=0;
    struct send_fill_server* send2;
    send2=malloc(sizeof(struct send_fill_server));
    if (send2==NULL){
        perror("malloc");
        exit(1);
    }
    if (memcpy(send2,send_fill,sizeof(struct send_fill_server))==NULL){
        perror("memcpy");
        exit(1);
    }
    for(;;){
        if (pthread_mutex_lock(&mutex2)!=0){
            perror("lock");
            exit(EXIT_FAILURE);
        }
        int i=0;
        while (fill_old==send_fill->fill){
//            printf("fill=%d\n",fill);
//            printf("indice=%d\n",i);
            i=i+1;
            if (pthread_cond_wait(&cond,&mutex2)!=0){
                perror("cond");
                exit(EXIT_FAILURE);
            }
        }
        printf("%d\n",send_fill->fill);
        if (((float)(send_fill->fill)/SONS)<0.25){
            perc1=0;
            
        }
        else if (((float)(send_fill->fill)/SONS)<0.50){
            perc1=25;
        }
        else if (((float)(send_fill->fill)/SONS)<0.75){
            perc1=50;
        }
        else if (((float)(send_fill->fill)/SONS)<1){
            perc1=75;
        }
        else {
            perc1=100;
            
        }
        fill_old=send_fill->fill;
        if (pthread_mutex_unlock(&mutex2)!=0){
            perror("unlock");
            exit(EXIT_FAILURE);
        }
        if (perc1!=perc){
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* crea il socket */
                perror("errore in socket");
                exit(1);
            }
            
            memset((void *)&servaddr, 0, sizeof(servaddr));   /* azzera servaddr */
            servaddr.sin_family = AF_INET;       /* assegna il tipo di indirizzo */
            servaddr.sin_port   = htons(SWITCH_PORT);  /* assegna la porta del server */
            /* assegna l'indirizzo del server prendendolo dalla riga di comando.
             L'indirizzo � una stringa e deve essere convertito in intero in
             network byte order. */
            if (inet_pton(AF_INET, SWITCH_SERVER, &servaddr.sin_addr) <= 0) {
                fprintf(stderr, "errore in inet_pton per \n");
                exit(1);
            }
            /* stabilisce la connessione con il server */
            if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
                perror("errore in connect");
                exit(1);
            }
//            perc=perc1;
            send2->fill=perc1;
//            printf("perc=%d\n",perc);
            if (write(sockfd, send2, sizeof(int)) != sizeof(int)) {
                perror("errore in write");
                exit(1);
            }
            if (close(sockfd)==-1){
                perror("close");
                exit(1);
            }
        }
    }
}



void create_children(struct pthread_struct* struct_var){
    send_fill=malloc(sizeof(struct send_fill_server));
    if (send_fill==NULL){
        perror("malloc");
        exit(1);
    }
    send_fill->fill=0;
    
    int i;
    struct pthread_struct* p_struct;
    for (i=0;i<SONS+1;i++){
        p_struct=malloc(sizeof(struct pthread_struct));
        if (p_struct==NULL){
            perror("malloc\n");
            exit(EXIT_FAILURE);
        }
        memcpy(p_struct,struct_var,sizeof(struct pthread_struct));
        if(p_struct==NULL){
            perror("memcpy\n");
            exit(EXIT_FAILURE);
        }
        p_struct->n_son=i;
        if (i!=0){
            if(pthread_create(&(p_struct->tid[i]),NULL,(void*)sons_work,p_struct)!=0){
                perror("Errore nella creazione del pthread\n");
                exit(EXIT_FAILURE);
            }
            struct_var->tid[i-1]=p_struct->tid[i];
        }
        else if (cluster->carico==1){
            if(pthread_create(&(p_struct->tid[i]),NULL,(void*)controller,NULL)!=0){
                perror("Errore nella creazione del pthread\n");
                exit(EXIT_FAILURE);
            }
            struct_var->tid[i]=p_struct->tid[i];
        }
        //struct_var->tid[i]=p_struct->tid[i];
        
    }
}



int main(int argc, char **argv)
{
    ip_switch=malloc(sizeof(char)*15);
    if (ip_switch==NULL){
        perror("errore malloc");
        exit(1);
    }
    int           listensd, connsd;
    struct sockaddr_in    servaddr;
    char          *buff=calloc(MAXLINE,sizeof(char));
    if (buff==NULL){
        perror("Errore calloc\n");
        exit(EXIT_FAILURE);
    }
    char            *buff2=calloc(MAXLINE,sizeof(char));
    if (buff2==NULL){
        perror("Errore calloc\n");
        exit(EXIT_FAILURE);
    }
    
    
    if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* crea il socket */
        perror("errore in socket");
        exit(1);
    }
    int yes=1;
    //char yes='1'; // use this under Solaris
    
    if (setsockopt(listensd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
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
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_size=sizeof(clientaddr);
    if ((connsd = accept(listensd, (struct sockaddr *)&clientaddr, &clientaddr_size)) < 0) {
        perror("errore in accept");
        exit(1);
    }
    
    
    ip_switch=inet_ntoa(clientaddr.sin_addr);
    printf("ip_server=%s\n",ip_switch);
    cluster=malloc(sizeof(struct setup_cluster));
    if (cluster==NULL){
        perror("setup");
        exit(1);
    }
    if (read(connsd,cluster,sizeof(struct setup_cluster))<0){
        perror("Errore nella read");
        exit(EXIT_FAILURE);
    }
    
    printf("carico=%d\n",cluster->carico);
    struct pthread_struct* pthread_struct;
    pthread_struct=malloc(sizeof(struct pthread_struct));
    if (pthread_struct==NULL){
        perror("Errore nella malloc\n");
        exit(EXIT_FAILURE);
    }
    pthread_struct->listensd=listensd;
    pthread_struct->connsd=connsd;
    pthread_struct->servaddr=servaddr;
    pthread_struct->buff=buff;
    pthread_struct->buff2=buff2;
    
    if (pthread_mutex_init(&mutex,NULL)!=0){
        perror("errore mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&mutex2,NULL)!=0){
        perror("errore mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&cond,NULL)!=0){
        perror("errore cond");
        exit(EXIT_FAILURE);
    }
    create_children(pthread_struct);
    int i;
    for (i=0;i<SONS;i++){
        if (pthread_join(pthread_struct->tid[i], NULL)!=0){
            perror("Errore nella join");
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);
    pthread_cond_destroy(&cond);
    exit(0);
}