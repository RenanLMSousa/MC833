#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

#include "../music/music.h"
#include "../external_files/storage_handler.h"
#include "../external_files/config_handler.h"
#include "server_operations.h"

#define MAXLINE 1000
#define SA struct sockaddr
#define LISTENQ 20
#define MAX_HEADER_SIZE 1000
#define MAX_BODY_SIZE 3000

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

// Anexa cabeçalho da operação ao corpo da mensagem
void anexar_header_operacao(char * message){
    char strOut[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "", strMessage[MAX_BODY_SIZE] = "";
    int role = 2; // Role do servidor é 2
    int operacao = -1; // Operacao das respostas do servidor é -1 

    // Copia a mensagem para um buffer temporário
    strcat(strMessage, message);
    strcat(strMessage,"\n");
    int msg_size = strlen(strMessage) * sizeof(char);

    // Cria o cabeçalho
    sprintf(strOut, "#HEADER\nSize=%d\nRole=%d\nOperation=%d\n#BODY\n", msg_size, role, operacao);
    strcat(strOut, strMessage);

    // Coloca a mensagem com o cabeçalho na variável original
    strcpy(message, strOut);
}

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
        char body[MAX_BODY_SIZE];
        char strMusic[MAX_HEADER_SIZE];
        struct _header header = read_message(buf,body);
        int operation = header.operation, role = header.role, counter;
        // Garantir que o buffer está limpo
        memset(strMusic, 0, sizeof(strMusic));
        switch (operation)
            {
            case CADASTRAR_UMA_MUSICA:       
                // Verifica se o usuário é admin     
                if (role == 1) {
                    error = cadastrar_musica(body);
                    if (error == 1) {
                        char err_msg[] = "This id is already in use.\n";
                        anexar_header_operacao(err_msg);
                        if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                    else if (error == 2) {
                        char err_msg[] = "You have reached the limit number of songs.\n";
                        anexar_header_operacao(err_msg);
                        if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                    else {
                        char msg[] = "Song registered.\n";
                        anexar_header_operacao(msg);
                        if (send_all(new_fd, msg, strlen(msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                }
                else {
                    char err_msg[] = "This operation is not available to regular users.\n";
                    anexar_header_operacao(err_msg);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case REMOVER_UMA_MUSICA:
                // Verifica se o usuário é admin  
                if (role == 1) {
                    error = remover_musica(body);
                    if (error == 1) {
                        char err_msg[] = "Song not found.\n";
                        anexar_header_operacao(err_msg);
                        if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                    else {
                        char msg[] = "Song deleted.\n";
                        anexar_header_operacao(msg);
                        if (send_all(new_fd, msg, strlen(msg)) < 0) {
                            perror("str_echo: send error");
                            return;
                        }
                    }
                }
                else {
                    char err_msg[] = "This operation is not available to regular users.\n";
                    anexar_header_operacao(err_msg);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LISTAR_MUSICAS_POR_ANO:
                counter = listar_musicas_por_ano(body, strMusic);
                if (counter == 0) {
                    char err_msg[] = "No songs found.\n";
                    anexar_header_operacao(err_msg);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else { 
                    anexar_header_operacao(strMusic);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LISTAR_MUSICAS_POR_IDIOMA_E_ANO:
                counter = listar_musicas_por_idioma_e_ano(body, strMusic);
                if (counter == 0) {
                    char err_msg[] = "No songs found.\n";
                    anexar_header_operacao(err_msg);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    anexar_header_operacao(strMusic);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LISTAR_MUSICAS_POR_TIPO:
                counter = listar_musicas_por_tipo(body, strMusic);
                if (counter == 0) {
                    char err_msg[] = "No songs found.\n";
                    anexar_header_operacao(err_msg);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    anexar_header_operacao(strMusic);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LISTAR_INFO_MUSICA_POR_ID:
                error = listar_info_musica_por_id(body, strMusic);
                if (error == 1) {
                    char err_msg[] = "Song not found.\n";
                    anexar_header_operacao(err_msg);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    anexar_header_operacao(strMusic);
                    if (send_all(new_fd, strMusic, strlen(strMusic)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                break;
            case LISTAR_TODAS_INFOS_MUSICAS:
                counter = listar_todas_infos_musicas(strMusic);
                if (counter == 0) {
                    char err_msg[] = "No songs to list.\n";
                    anexar_header_operacao(err_msg);
                    if (send_all(new_fd, err_msg, strlen(err_msg)) < 0) {
                        perror("str_echo: send error");
                        return;
                    }
                }
                else {
                    anexar_header_operacao(strMusic);
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
    configuracao serverConfig;

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

        // Envio de mensagem para confirmar conexão com cliente
        char conf_message[] = "Connection established.";
        anexar_header_operacao(conf_message);
        if (send_all(new_fd, conf_message, strlen(conf_message)) < 0) {
            perror("str_echo: send error");
            exit(0);
        }
        printf("Connection established.\n");

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
