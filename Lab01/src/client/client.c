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
#include <ctype.h>

#include "client_operations.h"
#include "../external_files/config_handler.h"

#define true 1
#define SA  struct sockaddr
#define LISTENQ 20
#define MAXLINE 1000

/*
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
}*/

void print_menu() {
    printf("Choose your operation by entering one of the following numbers: \n");
    printf("1 - Register a new song.\n");
    printf("2 - Remove a song.\n");
    printf("3 - List all songs released in a specific year.\n");
    printf("4 - List all songs of a specific language released in a specific year.\n");
    printf("5 - List all songs of a specific type.\n");
    printf("6 - List all informations of a specific song.\n");
    printf("7 - List all informations of all songs.\n");
    printf("0 - Close the application.\n");
}

int read_int(int isId) {
    int valid_input = 0, i, integer;
    char buffer[3000];
    while (!valid_input) {
        if (isId) {
            printf("Enter identifier: \n");
        }
        else {
            printf("Enter year: \n");
        }
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        for (i = 0; buffer[i] != '\0'; i++) {
            if (!isdigit(buffer[i])) {
                printf("Invalid input. Please enter an integer.\n");
                break; 
            }
        }
        if (buffer[i] == '\0') {
            integer = atoi(buffer);
            valid_input = 1;
        }
    }
    return integer;
}

void do_client_stuff(int sock_fd) {
    // Executando a lógica do cliente
    struct music my_music;
    int operation, identifier, year;
    char language[LANGUAGE_LENGTH], music_type[MUSIC_TYPE_LENGTH], buffer[3000];
    while(true){
        print_menu();
        printf("Enter operation: \n");
        fgets(buffer, sizeof(buffer), stdin);
        operation = atoi(buffer);
        switch (operation)
            {
            case CADASTRAR_UMA_MUSICA:
                my_music.identifier = read_int(1);
                
                printf("Enter title: \n");
                fgets(my_music.title, sizeof(my_music.title), stdin);
                my_music.title[strcspn(my_music.title, "\n")] = '\0'; // Remover a quebra de linha

                printf("Enter performer: \n");
                fgets(my_music.performer, sizeof(my_music.performer), stdin);
                my_music.performer[strcspn(my_music.performer, "\n")] = '\0';

                printf("Enter language: \n");
                fgets(my_music.language, sizeof(my_music.language), stdin);
                my_music.language[strcspn(my_music.language, "\n")] = '\0';

                printf("Enter music type: \n");
                fgets(my_music.music_type, sizeof(my_music.music_type), stdin);
                my_music.music_type[strcspn(my_music.music_type, "\n")] = '\0';

                printf("Enter chorus: \n");
                fgets(my_music.chorus, sizeof(my_music.chorus), stdin);
                my_music.chorus[strcspn(my_music.chorus, "\n")] = '\0';

                my_music.release_year = read_int(0);

                cadastrar_musica(sock_fd, my_music);
                break;
            case REMOVER_UMA_MUSICA:
                identifier = read_int(1);
                remover_musica(sock_fd, identifier);
                break;
            case LISTAR_MUSICAS_POR_ANO:
                printf("Enter year: ");
                fgets(buffer, sizeof(buffer), stdin);
                year = atoi(buffer);
                listar_musicas_por_ano(sock_fd, year);
                break;
            case LISTAR_MUSICAS_POR_IDIOMA_E_ANO:
                printf("Enter language: ");
                fgets(language, sizeof(language), stdin);
                printf("Enter year: ");
                fgets(buffer, sizeof(buffer), stdin);
                year = atoi(buffer);
                listar_musicas_por_idioma_e_ano(sock_fd, language, year);
                break;
            case LISTAR_MUSICAS_POR_TIPO:
                printf("Enter music type: ");
                fgets(music_type, sizeof(music_type), stdin);
                listar_musicas_por_tipo(sock_fd, music_type);
                break;
            case LISTAR_INFO_MUSICA_POR_ID:
                identifier = read_int(1);
                listar_info_musica_por_id(sock_fd, identifier);
                break;
            case LISTAR_TODAS_INFOS_MUSICAS:
                listar_todas_infos_musicas(sock_fd);
                break;
            default:
                return;
        }
        char sendline[MAXLINE];
        int n;
        if ((n = recv(sock_fd, sendline, MAXLINE,0)) == 0){
            printf("str_cli: server terminated prematurely");
        }
        //else{
            // printf("%s\n",sendline);
        //}

    }
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
    servaddr.sin_addr.s_addr = htons(atoi(serverConfig.ip));
    printf("%d\n",servaddr.sin_addr.s_addr);
    inet_pton(AF_INET, serverConfig.porta, &servaddr.sin_addr);

    // Conectando ao servidor
    connect(sock_fd, (SA *) &servaddr, sizeof(servaddr));
    printf("Connection established\n");

    do_client_stuff(sock_fd);
    
    // Fechando o socket e saindo
    printf("Closing connection.\n");
    exit(0);

    
    return 0;
}
