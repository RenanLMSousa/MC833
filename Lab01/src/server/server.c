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

#include "../music/music.h"
#include "../external_files/storage_handler.h"
#include "../external_files/config_handler.h"

#define MAX_SONGS 100
#define MAXLINE 1000
#define SA  struct sockaddr
#define LISTENQ 20

void str_echo(int new_fd) {
    ssize_t n;
    char buf[MAXLINE];

again:
    bzero(&buf, sizeof(buf));
    while ((n = recv(new_fd, buf, MAXLINE,0)) > 0) {
        // Escrever de volta para o cliente
        printf("BUFFER SIZE: %d\n%s\n",strlen(buf),buf);
        if (write(new_fd, buf, n) < 0) {
            perror("str_echo: write error");
            return;
        }

    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        perror("str_echo: read error");
}

void do_server_stuff(int new_fd){

    struct music music_list[MAX_SONGS];
    int num_songs = read_music_list(music_list, "../../storage/music_list.csv");
    str_echo(new_fd);

}


int main() {
    Configuracao serverConfig;

    printf("Reading configs\n");
    serverConfig =  ler_configuracao("../../server.config");
    printf("Initializing server, hearing on PORT: %s\n",serverConfig.porta);

    int sock_fd, new_fd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    // Criação do socket
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    // Configuração do endereço do servidor
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(serverConfig.porta));
    
    // Associação do socket ao endereço do servidor
    bind(sock_fd, (SA *) &servaddr, sizeof(servaddr));
    // Definição do socket para escutar conexões

    listen(sock_fd, LISTENQ);

    for (;;) {
        clilen = sizeof(cliaddr);
        // Aceitar conexões entrantes
        
        new_fd = accept(sock_fd, (SA *) &cliaddr, &clilen);
        printf("Connection stablished\n");

        // Criação de um processo filho para tratar a conexão
        if ((childpid = fork()) == 0) { /* processo filho */
            close(sock_fd); /* fechar o socket de escuta */
            // Processar a solicitação
            do_server_stuff(new_fd);
            exit(0);
        }
        // Pai fecha o socket conectado
        close(new_fd);
    }
    return 0;
}
