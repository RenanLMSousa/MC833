#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

#include "../music/music.h"
#include "../external_files/storage_handler.h"
#include "../external_files/config_handler.h"
#include "../utils/utils.h"
#include "server_operations.h"
#include <sys/select.h>

#define SA struct sockaddr
#define LISTENQ 20

// Estrutura privada para guardar informações do cabeçalho
struct _header {
    int operation;
    int size;
    int role;
};

struct _header extract_header(char * strHeader){
    struct _header header;

    // Tamanho
    char *token = strtok(strHeader, "=");
    token = strtok(NULL, "\n");
    header.size = atoi(token);

    // Role
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    header.role = atoi(token);

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

void do_server_stuff(int new_fd){
    ssize_t n;
    char buf[MAXLINE];
    int error;

again:
    memset(buf, 0, sizeof(buf));
    while ((n = recv_all(new_fd, buf)) > 0) {
        char body[MAX_BODY_SIZE];
        char strMusic[MAX_HEADER_SIZE + MAX_BODY_SIZE];
        struct _header header = read_message(buf,body);
        int operation = header.operation, role = header.role, counter;
        // Garantir que o buffer está limpo
        memset(strMusic, 0, sizeof(strMusic));
        switch (operation)
            {
            case REGISTER_SONG:       
                // Verifica se o usuário é admin     
                if (role == 1) {
                    error = register_song(body);
                    // Lida com erro de id já utilizado
                    if (error == 1) {
                        char err_msg[] = "This id is already in use.\n";
                        build_message(err_msg, -1, 2);
                        if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                    // Lida com erro de limite de músicas
                    else if (error == 2) {
                        char err_msg[] = "You have reached the limit number of songs.\n";
                        build_message(err_msg, -1, 2);
                        if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                    // Operação concluída
                    else {
                        char msg[] = "Song registered.\n";
                        build_message(msg, -1, 2);
                        if (send_all(new_fd, msg, strlen(msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                }
                else {
                    char err_msg[] = "This operation is not available to regular users.\n";
                    build_message(err_msg, -1, 2);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case REMOVE_SONG:
                // Verifica se o usuário é admin  
                if (role == 1) {
                    error = remove_song(body);
                    if (error == 1) {
                        // Lida com erro de música não encontrada
                        char err_msg[] = "Song not found.\n";
                        build_message(err_msg, -1, 2);
                        if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                    else {
                        // Operação concluída
                        char msg[] = "Song deleted.\n";
                        build_message(msg, -1, 2);
                        if (send_all(new_fd, msg, strlen(msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                }
                else {
                    char err_msg[] = "This operation is not available to regular users.\n";
                    build_message(err_msg, -1, 2);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LIST_SONGS_BY_YEAR:
                counter = list_songs_by_year(body, strMusic);
                if (counter == 0) {
                    // Lida com erro de música não encontrada
                    char err_msg[] = "No songs found.\n";
                    build_message(err_msg, -1, 2);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else { 
                    // Operação concluída
                    build_message(strMusic, -1, 2);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LIST_SONGS_BY_LANGUAGE_AND_YEAR:
                counter = list_songs_by_language_and_year(body, strMusic);
                if (counter == 0) {
                    // Lida com erro de música não encontrada
                    char err_msg[] = "No songs found.\n";
                    build_message(err_msg, -1, 2);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    // Operação concluída
                    build_message(strMusic, -1, 2);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LIST_SONGS_BY_TYPE:
                counter = list_songs_by_type(body, strMusic);
                if (counter == 0) {
                    // Lida com erro de música não encontrada
                    char err_msg[] = "No songs found.\n";
                    build_message(err_msg, -1, 2);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    // Operação concluída
                    build_message(strMusic, -1, 2);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LIST_SONG_INFO_BY_ID:
                error = list_song_info_by_id(body, strMusic);
                if (error == 1) {
                    // Lida com erro de música não encontrada
                    char err_msg[] = "Song not found.\n";
                    build_message(err_msg, -1, 2);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    // Operação concluída
                    build_message(strMusic, -1, 2);
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
                    build_message(err_msg, -1, 2);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    // Operação concluída
                    build_message(strMusic, -1, 2);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
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

    int sock_fd_tcp,sock_fd_udp, new_fd;
    pid_t childpid;
    socklen_t clilen;
    fd_set rset;
    struct sockaddr_in cliaddr, servaddr;
    // Criação do socket
    sock_fd_tcp = socket(PF_INET, SOCK_STREAM, 0);

    // Configuração do endereço do servidor
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(serverConfig.port));
    
    const int on =1;
    setsockopt(sock_fdp_tcp,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    // Associação do socket ao endereço do servidor
    bind(sock_fd_tcp, (SA *) &servaddr, sizeof(servaddr));
    // Definição do socket para escutar conexões
    listen(sock_fd_tcp, LISTENQ);

    //Criação socket UDP

    sock_fd_udp = socket(AF_INET,SOCK_DGRAM,0);

    bzero(&serveraddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(serverConfig.porta));
    bind(sock_fd_udp , (SA*) &servaddr, sizeof(servaddr));

    FD_ZERO(&rset);
    int maxfdp1 = max(sock_fd_tcp,sock_fd_udp) +1;

    for (;;) {
        clilen = sizeof(cliaddr);

        // Aceitar conexões entrantes
        new_fd = accept(sock_fd_tcp, (SA *) &cliaddr, &clilen);

        // Envio de mensagem para confirmar conexão com cliente
        char conf_message[] = "Connection established.\n";
        build_message(conf_message, -1, 2);
        if (send_all(new_fd, conf_message, strlen(conf_message)) < 0) {
            perror("str_echo: send error");
            exit(0);
        }
        printf("Connection established.\n");

        // Criação de um processo filho para tratar a conexão
        if ((childpid = fork()) == 0) { /* processo filho */
            close(sock_fd_tcp); /* fechar o socket de escuta */
            // Processar a solicitação
            do_server_stuff(new_fd);

            exit(0);
        }
        // Pai fecha o socket conectado
        close(new_fd);
    }
    return 0;
}
