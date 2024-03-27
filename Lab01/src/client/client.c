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
    ssize_t n;
    while (fgets(sendline, MAXLINE, fp) != NULL) {
        send_all(sock_fd,sendline,0);
        if ((n = recv(sock_fd, sendline, MAXLINE,0)) == 0){
            printf("str_cli: server terminated prematurely");
        }
        else{
            printf("%s\n",sendline);
        }
    }
}

void do_client_stuff(int sock_fd){

    // Executando a lógica do cliente
    struct music my_music;
    int operation;
    scanf("%d",operation);

    switch (operation)
    {
    case CADASTRAR_UMA_MUSICA:
      
        break;
    case REMOVER_UMA_MUSICA:
        
        break;
    case LISTAR_MUSICAS_POR_ANO:
    
        break;
    case LISTAR_MUSICAS_POR_IDIOMA_E_ANO:
    
        break;
    case LISTAR_MUSICAS_POR_TIPO:
    
        break;
    case LISTAR_INFO_MUSICA_POR_ID:
    
        break;
    
    default:
        break;
    }
    
    str_cli(stdin, sock_fd); /* faz tudo */

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
    servaddr.sin_addr.s_addr = htons(atoi(serverConfig.ip));
    printf("%d\n",servaddr.sin_addr.s_addr);
    inet_pton(AF_INET, serverConfig.porta, &servaddr.sin_addr);

    // Conectando ao servidor
    connect(sock_fd, (SA *) &servaddr, sizeof(servaddr));
    

    printf("Connection stablished\n");
    do_client_stuff(sock_fd);
    
    // Fechando o socket e saindo
    exit(0);

   return 0;
}
