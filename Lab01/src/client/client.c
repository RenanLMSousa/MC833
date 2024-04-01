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

void print_menu() {
    printf("Choose your operation by entering one of the following numbers: \n");
    printf("1 - Register a new song.\n");
    printf("2 - Remove a song.\n");
    printf("3 - List all songs released in a specific year.\n");
    printf("4 - List all songs of a specific language released in a specific year.\n");
    printf("5 - List all songs of a specific type.\n");
    printf("6 - List all informations of a specific song.\n");
    printf("7 - List all informations of all songs.\n");
}

void do_client_stuff(int sock_fd){

    // Executando a lógica do cliente
    struct music my_music;
    int operation;
    int identifier, year;
    char language[LANGUAGE_LENGTH], music_type[MUSIC_TYPE_LENGTH];
    print_menu();
    scanf("%d", &operation);

    switch (operation)
    {
    case CADASTRAR_UMA_MUSICA:
        printf("Enter identifier: \n");
        scanf("%d", &my_music.identifier);
        printf("Enter title: \n");
        scanf(" %s", my_music.title);
        printf("Enter performer: \n");
        scanf(" %s", my_music.performer);
        printf("Enter language: \n");
        scanf(" %s", my_music.language);
        printf("Enter music type: \n");
        scanf(" %s", my_music.music_type);
        printf("Enter chorus: \n");
        scanf(" %s", my_music.chorus);
        printf("Enter release year: \n");
        scanf("%d", &my_music.release_year);
        cadastrar_musica(sock_fd, my_music);
        break;
    case REMOVER_UMA_MUSICA:
        printf("Enter identifier: ");
        scanf("%d", &identifier);
        remover_musica(sock_fd, identifier);
        break;
    case LISTAR_MUSICAS_POR_ANO:
        printf("Enter year: ");
        scanf("%d", &year);
        listar_musicas_por_ano(sock_fd, year);
        break;
    case LISTAR_MUSICAS_POR_IDIOMA_E_ANO:
        printf("Enter year: ");
        scanf("%d", &year);
        printf("Enter language: ");
        scanf(" %s", language);
        listar_musicas_por_idioma_e_ano(sock_fd, language, year);
        break;
    case LISTAR_MUSICAS_POR_TIPO:
        printf("Enter music type: ");
        scanf(" %s", music_type);
        listar_musicas_por_tipo(sock_fd, music_type);
        break;
    case LISTAR_INFO_MUSICA_POR_ID:
        printf("Enter identifier: ");
        scanf("%d", &identifier);
        listar_info_musica_por_id(sock_fd, identifier);
        break;
    default:
        listar_todas_infos_musicas(sock_fd);
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
