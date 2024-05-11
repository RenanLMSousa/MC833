#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../music/music.h"
#include "../external_files/storage_handler.h"
#include "../external_files/config_handler.h"
#include "../utils/utils.h"
#include "server_operations.h"

#define SA struct sockaddr
#define LISTENQ 20

// Estrutura privada para guardar informações do cabeçalho
struct _header {
    int operation;
    int size;
};

struct _header extract_header(char * strHeader){
    struct _header header;

    // Tamanho
    char *token = strtok(strHeader, "=");
    token = strtok(NULL, "\n");
    header.size = atoi(token);

    // Operação
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    header.operation = atoi(token);

    return header;
}

// Separa header de body, preenchendo o vetor body e retornando a estrutura header
struct _header read_message(char * _message, char * body) {
    // Variáveis para armazenar o conteúdo do #HEADER e #BODY
    char strHeader[MAX_HEADER_SIZE] = "";
    char strBody[MAX_BODY_SIZE] = "";

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

    strcpy(body,strBody);
    return extract_header(strHeader);
}

void do_server_stuff(int new_fd, configuration serverConfig){
    ssize_t n;
    char buf[MAXLINE];
    int error;

again:
    memset(buf, 0, sizeof(buf));
    while ((n = recv_all(new_fd, buf)) > 0) {
        char body[MAX_BODY_SIZE];
        char strMusic[MAX_HEADER_SIZE + MAX_BODY_SIZE];
        struct _header header = read_message(buf,body);
        int operation = header.operation, counter;
        
        // Garantir que o buffer está limpo
        memset(strMusic, 0, sizeof(strMusic));
        switch (operation)
            {
            case LIST_SONGS_BY_TYPE:
                counter = list_songs_by_type(body, strMusic);
                if (counter == 0) {
                    // Lida com erro de música não encontrada
                    char err_msg[] = "No songs found.\n";
                    build_message(err_msg, -1);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    // Operação concluída
                    build_message(strMusic, -1);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LIST_ALL_SONGS_INFO:
                counter = list_all_songs_info(strMusic);
                if (counter == 0) {
                    // Lida com erro de lista de músicas vazia
                    char err_msg[] = "No songs to list.\n";
                    build_message(err_msg, -1);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    // Operação concluída
                    build_message(strMusic, -1);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case DOWNLOAD_SONG:
                error = download_song(body, serverConfig);
                if (error == 1) {
                    char err_msg[] = "File not found.\n";
                    build_message(err_msg, -1);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                // Operação concluída
                else {
                    char confirm_message[] = "Operation concluded successfuly.\n";
                    build_message(confirm_message, -1);
                    if (send_all(new_fd, confirm_message, strlen(confirm_message)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
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
    configuration serverConfig;
    printf("Reading configs\n");
    serverConfig =  read_configuration("../../server.config");
    printf("Initializing server, hearing on PORT: %s\n",serverConfig.port);

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
    servaddr.sin_port = htons(atoi(serverConfig.port));
    
    // Associação do socket ao endereço do servidor
    bind(sock_fd, (SA *) &servaddr, sizeof(servaddr));
    // Definição do socket para escutar conexões
    listen(sock_fd, LISTENQ);

    for (;;) {
        clilen = sizeof(cliaddr);

        // Aceitar conexões entrantes
        new_fd = accept(sock_fd, (SA *) &cliaddr, &clilen);

        // Envio de mensagem para confirmar conexão com cliente
        char conf_message[] = "Connection established.\n";
        build_message(conf_message, -1);
        if (send_all(new_fd, conf_message, strlen(conf_message)) < 0) {
            perror("str_echo: send error");
            exit(0);
        }
        printf("Connection established.\n");

        // Criação de um processo filho para tratar a conexão
        if ((childpid = fork()) == 0) { /* processo filho */
            close(sock_fd); /* fechar o socket de escuta */
            // Processar a solicitação
            do_server_stuff(new_fd, serverConfig);

            exit(0);
        }
        // Pai fecha o socket conectado
        close(new_fd);
    }
    return 0;
}
