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
#define MAX_BUF_SIZE 3000

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

// Verifica se o header representa o servidor, se não, retorna 1
int verificar_header(char * header) {
    int size, role, operation;

    // Tamanho
    char *token = strtok(header, "=");
    token = strtok(NULL, "\n");
    size = atoi(token);

    // Role
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    role = atoi(token);
    if (role != 2) {
        return 1;
    }

    // Operação
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    operation = atoi(token);
    if (operation != -1) {
        return 1;
    }

    return 0;
}

// Remove o cabeçalho do servidor, escrevendo o corpo em body, retorna 1 se houver erro
int remove_cabecalho_servidor(char * message, char * body) {
    // Variáveis para armazenar o conteúdo do #HEADER e #BODY
    char strHeader[MAX_HEADER_SIZE] = "";
    char strBody[MAX_BODY_SIZE] = "";

    // Variável para controlar se estamos lendo o cabeçalho ou o corpo
    int readingBody = 0;

    // Separando o conteúdo
    char *token = strtok(message, "\n");
    while (token != NULL) {
        if (strcmp(token, "#BODY") == 0) {
            readingBody = 1;
        } else if (strcmp(token, "#HEADER") == 0) {
            readingBody = 0;
        } else {
            if (readingBody) {
                char strId[] = "Identifier=";
                if (strncmp(token, strId, strlen(strId)) == 0) {
                    strcat(strBody, "\n");
                }
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
    strcpy(body, strBody);
    return verificar_header(strHeader);
}

// Faz a lógica da interação entre cliente e servidor
void do_client_stuff(int sock_fd) {
    // Executando a lógica do cliente
    int role = -1, operation;
    char sendline[MAXLINE], buffer[MAX_BODY_SIZE + MAX_HEADER_SIZE];
    
    // Garante que o usuário escolha entre ser ou não admin
    while (role == -1) {
        role = run_admin();
    }
    
    while(true) {

        print_menu();
        printf("Enter operation: \n");
        fgets(buffer, sizeof(buffer), stdin);
        operation = atoi(buffer);
        memset(sendline, 0, sizeof(sendline));
        switch (operation)
            {
            case CADASTRAR_UMA_MUSICA: {
                struct music my_music;
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

                cadastrar_musica(sock_fd, my_music, role);
                break;
            }
            case REMOVER_UMA_MUSICA: {
                int identifier = read_int(1);
                remover_musica(sock_fd, identifier, role);
                break;
                }
            case LISTAR_MUSICAS_POR_ANO: {
                char buffer[MAX_BODY_SIZE + MAX_HEADER_SIZE];
                int year;

                printf("Enter year: ");
                fgets(buffer, sizeof(buffer), stdin);
                year = atoi(buffer);
                listar_musicas_por_ano(sock_fd, year, role);
                break;
                }
            case LISTAR_MUSICAS_POR_IDIOMA_E_ANO: {
                char language[LANGUAGE_LENGTH], buffer[MAX_BODY_SIZE + MAX_HEADER_SIZE];
                int year;

                printf("Enter language: ");
                fgets(language, sizeof(language), stdin);
                printf("Enter year: ");
                fgets(buffer, sizeof(buffer), stdin);
                year = atoi(buffer);
                listar_musicas_por_idioma_e_ano(sock_fd, language, year, role);
                break;
                }
            case LISTAR_MUSICAS_POR_TIPO: {
                char music_type[MUSIC_TYPE_LENGTH];

                printf("Enter music type: ");
                fgets(music_type, sizeof(music_type), stdin);
                listar_musicas_por_tipo(sock_fd, music_type, role);
                break;
                }
            case LISTAR_INFO_MUSICA_POR_ID: {
                int identifier = read_int(1);
                listar_info_musica_por_id(sock_fd, identifier, role);
                break;
                }
            case LISTAR_TODAS_INFOS_MUSICAS:
                listar_todas_infos_musicas(sock_fd, role);
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
            char body[MAX_BODY_SIZE];
            int error = remove_cabecalho_servidor(sendline, body);
            if (error == 1) {
                printf("Mensagem recebida não é do servidor, segue o corpo:\n");
            }
            printf("\n%s\n", body);
        }
    }
}

int main() {
    int sock_fd;
    struct sockaddr_in servaddr;
    configuracao serverConfig;

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

    // Esperando resposta para verificar conexão
    char buf[MAX_BUF_SIZE];
    if (recv(sock_fd, buf, MAX_BUF_SIZE, 0) < 0) {
        perror("Error receiving confirmation.");
        exit(EXIT_FAILURE);
    }
    char body[MAX_BODY_SIZE];
    int error = remove_cabecalho_servidor(buf, body);
    if (error == 1) {
        printf("Mensagem recebida não é do servidor, segue o corpo:\n");
    }
    printf("%s\n", body);

    // Executa lógica do cliente
    do_client_stuff(sock_fd);
    
    // Fechando o socket e saindo
    printf("Closing connection.\n");
    exit(0);

    return 0;
}
