#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "client_operations.h"
#include "../external_files/config_handler.h"

#define true 1
#define SA struct sockaddr
#define LISTENQ 20
#define MAXLINE 1000

// Pergunta se o usuário quer rodar como administrador e retorna o resultado
int run_admin() {
    char choice[20];
    printf("Do you want to execute as admin? (yes/no): ");
    fgets(choice, sizeof(choice), stdin);

    // Remove caracter de nova linha se houver um
    choice[strcspn(choice, "\n")] = '\0';

    // Garante que o input funciona em caixa alta ou não
    for (int i = 0; choice[i]; i++) {
        choice[i] = tolower(choice[i]);
    }

    if (strcmp(choice, "yes") == 0 || strcmp(choice, "y") == 0) {
        printf("Executing as admin\n\n");
        return 1;
    } 
    else if (strcmp(choice, "no") == 0 || strcmp(choice, "n") == 0) {
        printf("Executing as regular user\n\n");
        return 0;
    } 
    else {
        // Retorna -1 para continuar perguntando
        printf("Invalid input. Please enter 'yes' or 'no'.\n");
        return -1;
    }
}

// Imprime menu de funções para guiar o usuário
void print_menu() {
    printf("Choose your operation by entering one of the following numbers: \n");
    printf("1 - Register a new song (only admin).\n");
    printf("2 - Remove a song (only admin).\n");
    printf("3 - List all songs released in a specific year.\n");
    printf("4 - List all songs of a specific language released in a specific year.\n");
    printf("5 - List all songs of a specific type.\n");
    printf("6 - List all informations of a specific song.\n");
    printf("7 - List all informations of all songs.\n");
    printf("0 - Close the application.\n");
}

// Pega entrada do usuário e verifica se é inteira
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

// Faz a lógica da interação entre cliente e servidor
void do_client_stuff(int sock_fd) {
    // Executando a lógica do cliente
    struct music my_music;
    int admin = -1, operation, identifier, year;
    char language[LANGUAGE_LENGTH], music_type[MUSIC_TYPE_LENGTH], sendline[MAXLINE], buffer[3000];
    
    // Garante que o usuário escolha entre ser ou não admin
    while (admin == -1) {
        admin = run_admin();
    }
    
    while(true) {
        print_menu();
        printf("Enter operation: \n");
        fgets(buffer, sizeof(buffer), stdin);
        operation = atoi(buffer);
        memset(sendline, 0, sizeof(sendline));
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

                cadastrar_musica(sock_fd, my_music, admin);
                break;
            case REMOVER_UMA_MUSICA:
                identifier = read_int(1);
                remover_musica(sock_fd, identifier, admin);
                break;
            case LISTAR_MUSICAS_POR_ANO:
                printf("Enter year: ");
                fgets(buffer, sizeof(buffer), stdin);
                year = atoi(buffer);
                listar_musicas_por_ano(sock_fd, year, admin);
                break;
            case LISTAR_MUSICAS_POR_IDIOMA_E_ANO:
                printf("Enter language: ");
                fgets(language, sizeof(language), stdin);
                printf("Enter year: ");
                fgets(buffer, sizeof(buffer), stdin);
                year = atoi(buffer);
                listar_musicas_por_idioma_e_ano(sock_fd, language, year, admin);
                break;
            case LISTAR_MUSICAS_POR_TIPO:
                printf("Enter music type: ");
                fgets(music_type, sizeof(music_type), stdin);
                listar_musicas_por_tipo(sock_fd, music_type, admin);
                break;
            case LISTAR_INFO_MUSICA_POR_ID:
                identifier = read_int(1);
                listar_info_musica_por_id(sock_fd, identifier, admin);
                break;
            case LISTAR_TODAS_INFOS_MUSICAS:
                listar_todas_infos_musicas(sock_fd, admin);
                break;
            default:
                return;
        }

        // Recebe a resposta do servidor após a operação
        int n;
        if ((n = recv(sock_fd, sendline, MAXLINE,0)) == 0){
            printf("str_cli: server terminated prematurely");
        }
        else {
            printf("\n%s\n",sendline);
        }
    }
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
    inet_pton(AF_INET, serverConfig.ip, &servaddr.sin_addr);

    // Conectando ao servidor
    connect(sock_fd, (SA *) &servaddr, sizeof(servaddr));
    printf("Connection established\n");

    do_client_stuff(sock_fd);
    
    // Fechando o socket e saindo
    printf("Closing connection.\n");
    exit(0);

    return 0;
}
