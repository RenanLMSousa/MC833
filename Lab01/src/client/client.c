#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "client_operations.h"
#include "../external_files/config_handler.h"


#define SA  struct sockaddr
#define LISTENQ 20
#define MAXLINE 1000


void str_cli(FILE *fp, int sock_fd) {
    char sendline[MAXLINE];
    
    while (fgets(sendline, MAXLINE, fp) != NULL) {
        printf("%s",sendline);
        send_all(sock_fd,sendline,0);
    }
}
void do_client_stuff(int sock_fd){

    // Executando a lógica do cliente
    struct music my_music;
    my_music.identifier = 1;
    strcpy(my_music.title, "The Four Seasons");
    strcpy(my_music.performer, "Antonio Vivaldi");
    strcpy(my_music.language, "Italian");
    strcpy(my_music.music_type, "Classical");
    my_music.release_year = 1723;
    strcpy(my_music.filename, "the_four_seasons.mp3");
    cadastrar_musica(sock_fd,my_music);
    //str_cli(stdin, sock_fd); /* faz tudo */

}
int main() {
    int sock_fd;
    struct sockaddr_in servaddr;
    Configuracao serverConfig;

    printf("Reading configs\n");
    serverConfig =  ler_configuracao("../../server.config");
    printf("Requesting to IP:%s | PORT: %s\n",serverConfig.ip,serverConfig.porta);


    
    // Criando o socket
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    // Configurando o endereço do servidor
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(serverConfig.porta));
    inet_pton(AF_INET, serverConfig.porta, &servaddr.sin_addr);

    // Conectando ao servidor
    connect(sock_fd, (SA *) &servaddr, sizeof(servaddr));

    do_client_stuff(sock_fd);
    
    // Fechando o socket e saindo
    exit(0);

   return 0;
}
