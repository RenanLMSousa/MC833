#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "client_operations.h"
#include "../utils/utils.h"
#include "../external_files/config_handler.h"

#define true 1
#define SA struct sockaddr
#define LISTENQ 20
#define MAX_BUF_SIZE 3000

// Imprime menu de funções para guiar o usuário
void print_menu() {
    printf("Choose your operation by entering one of the following numbers: \n");
    printf("1 - List all songs of a specific type.\n");
    printf("2 - List all informations of all songs.\n");
    printf("3 - Download a song\n");
    printf("0 - Close the application.\n");
}

// Pega entrada do usuário e verifica se é inteira
int read_int(int isId) {
    int valid_input = 0, i, integer;
    char buffer[3000];
    // Até que um input valido seja enviado, continua perguntando
    while (!valid_input) {
        // Como ints são usados para IDs e anos, fazemos a distinção
        if (isId) {
            printf("Enter identifier: \n");
        }
        else {
            printf("Enter year: \n");
        }
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        for (i = 0; buffer[i] != '\0'; i++) {
            // Verifica char por char se sao todos digitos
            if (!isdigit(buffer[i])) {
                printf("Invalid input. Please enter an integer.\n");
                break; 
            }
        }
        // Garante que a iteração anterior chegou até o fim da string
        if (buffer[i] == '\0') {
            integer = atoi(buffer);
            valid_input = 1;
        }
    }
    return integer;
}

// Faz a lógica da interação entre cliente e servidor
void do_client_stuff(int sock_fd, configuration serverConfig) {
    // Executando a lógica do cliente
    int operation;
    char sendline[MAXLINE], buffer[MAX_BODY_SIZE + MAX_HEADER_SIZE];
    
    while(true) {

        print_menu();
        printf("Enter operation: \n");
        fgets(buffer, sizeof(buffer), stdin);
        operation = atoi(buffer);
        memset(sendline, 0, sizeof(sendline));
        switch (operation)
            // Executa a operação de acordo com o código
            {
            case LIST_SONGS_BY_TYPE: {
                char music_type[MUSIC_TYPE_LENGTH];

                printf("Enter music type: ");
                fgets(music_type, sizeof(music_type), stdin);
                list_songs_by_type(sock_fd, music_type);
                break;
                }
            case LIST_ALL_SONGS_INFO:
                list_all_songs_info(sock_fd);
                break;
            case DOWNLOAD_SONG: {
                int identifier = read_int(1);
                download_song(sock_fd, identifier, serverConfig);
                break;
                }
            default:
                // Operação para encerrar a execução
                return;
        }

        // Recebe a resposta do servidor após a operação
        int n;
        if ((n = recv_all(sock_fd, sendline)) == 0) {
            // Se não recebeu nada, o servidor está com problema
            printf("str_cli: server terminated prematurely\n");
        }
        else {
            // Remove o cabeçalho e imprime pro cliente o resultado da operação
            char body[MAX_BODY_SIZE] = "";
            printf("oi2\n");
            int error = remove_header(sendline, body);
            printf("oi3\n");
            if (error == -1) {
                printf("Broken message.\n");
            }
            printf("\n%s\n", body);
        }
        printf("oi4\n");
    }
}

int main() {
    int sock_fd;
    struct sockaddr_in servaddr;
    configuration serverConfig;

    printf("Reading configs\n");
    serverConfig =  read_configuration("../../server.config");
    printf("Requesting to IP:%s | PORT: %s\n",serverConfig.ip,serverConfig.port);
    
    // Criando o socket
    sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    // Configurando o endereço do servidor
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(serverConfig.port));
    servaddr.sin_addr.s_addr = htons(atoi(serverConfig.ip));
    inet_pton(AF_INET, serverConfig.ip, &servaddr.sin_addr);

    // Conectando ao servidor
    connect(sock_fd, (SA *) &servaddr, sizeof(servaddr));

    // Esperando resposta para verificar conexão
    char buf[MAX_BUF_SIZE];
    int n;
    if ((n = recv_all(sock_fd, buf)) < 0) {
        perror("Error receiving confirmation.");
        exit(EXIT_FAILURE);
    }

    char body[MAX_BODY_SIZE];
    int error = remove_header(buf, body);
    if (error == -1) {
        printf("Broken confirmation message.\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", body);

    // Executa lógica do cliente
    do_client_stuff(sock_fd, serverConfig);
    
    // Fechando o socket e saindo
    printf("Closing connection.\n");
    exit(0);

    return 0;
}
