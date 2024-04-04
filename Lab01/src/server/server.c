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
#include "server_operations.h"

#define MAX_SONGS 100
#define MAXLINE 1000
#define SA  struct sockaddr
#define LISTENQ 20

struct _header {
    int operation;
    int size;
};

struct _header extract_header(char * strHeader){

    struct _header header;

    //Size
    char *token = strtok(strHeader, "=");
    token = strtok(NULL, "\n");
    header.size = atoi(token);

    //Operation
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    header.operation = atoi(token);

    return header;
}

struct _header read_message(char * _message, char * body) {

    // Variáveis para armazenar o conteúdo do #HEADER e #BODY
    char strHeader[1000] = "";
    char strBody[1000] = "";

    // Variável para controlar se estamos lendo o cabeçalho ou o corpo
    int readingBody = 0;

    // Separando o conteúdo
    char *token = strtok(_message, "\n");
    while (token != NULL) {
        if (strcmp(token, "#BODY") == 0) {
            readingBody = 1;
        } else if (strcmp(token, "#HEADER") == 0) {
            readingBody = 0;
        } else {
            if (readingBody) {
                strcat(strBody, token);
                strcat(strBody, "\n");
            } else {
                strcat(strHeader, token);
                strcat(strHeader, "\n");
            }
        }
        token = strtok(NULL, "\n");
    }

    // Exibindo o conteúdo separado

    strcpy(body,strBody);
    return extract_header(strHeader);

}

void do_server_stuff(int new_fd){
    ssize_t n;
    char buf[MAXLINE];
    int error;

again:
    memset(buf, 0, sizeof(buf));
    while ((n = recv(new_fd, buf, MAXLINE,0)) > 0) {
        // Escrever de volta para o cliente
        char body[3000];
        char strMusic[10000];
        struct _header header = read_message(buf,body);
        int operation = header.operation, counter;
        memset(strMusic, 0, sizeof(strMusic));
        switch (operation)
            {
            case CADASTRAR_UMA_MUSICA:            
                error = cadastrar_musica(body);
                if (error == 1) {
                    if (write(new_fd, "This id is already in use.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                else if (error == 2) {
                    if (write(new_fd, "You have reached the limit number of songs.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                else {
                    if (write(new_fd, "Song registered.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                break;
            case REMOVER_UMA_MUSICA:
                error = remover_musica(body);
                if (error == 1) {
                    if (write(new_fd, "Song not found.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                else {
                    if (write(new_fd, "Song deleted.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                break;
            case LISTAR_MUSICAS_POR_ANO:
                counter = listar_musicas_por_ano(body, strMusic);
                if (counter == 0) {
                    if (write(new_fd, "0 songs found.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                else { 
                    send_all(new_fd, strMusic, 0);
                }
                break;
            case LISTAR_MUSICAS_POR_IDIOMA_E_ANO:
                counter = listar_musicas_por_idioma_e_ano(body, strMusic);
                if (counter == 0) {
                    if (write(new_fd, "0 songs found.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                else {
                    send_all(new_fd, strMusic, 0);
                }
                break;
            case LISTAR_MUSICAS_POR_TIPO:
                counter = listar_musicas_por_tipo(body, strMusic);
                if (counter == 0) {
                    if (write(new_fd, "0 songs found.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                else {
                    send_all(new_fd, strMusic, 0);
                }
                break;
            case LISTAR_INFO_MUSICA_POR_ID:
                error = listar_info_musica_por_id(body, strMusic);
                if (error == 1) {
                    if (write(new_fd, "Song not found.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                else {
                    send_all(new_fd, strMusic, 0);
                }
                break;
            case LISTAR_TODAS_INFOS_MUSICAS:
                counter = listar_todas_infos_musicas(strMusic);
                if (counter == 0) {
                    if (write(new_fd, "No songs to list.\n", n) < 0) {
                        perror("str_echo: write error");
                        return;
                    }
                }
                else {
                    send_all(new_fd, strMusic, 0);
                }
                break;
            default:
                return;
        }
        memset(buf, 0, sizeof(buf));
    }

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        perror("str_echo: read error");
    


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
    printf("%d\n",servaddr.sin_addr.s_addr);
    listen(sock_fd, LISTENQ);

    for (;;) {
        clilen = sizeof(cliaddr);
        // Aceitar conexões entrantes
        
        new_fd = accept(sock_fd, (SA *) &cliaddr, &clilen);
        printf("Connection established\n");
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
