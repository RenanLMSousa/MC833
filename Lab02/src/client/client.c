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
            // Executa a operação de acordo com o código
            {
            case REGISTER_SONG: {
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

                register_song(sock_fd, my_music, role);
                break;
            }
            case REMOVE_SONG: {
                int identifier = read_int(1);
                remove_song(sock_fd, identifier, role);
                break;
                }
            case LIST_SONGS_BY_YEAR: {
                char buffer[MAX_BODY_SIZE + MAX_HEADER_SIZE];
                int year;

                printf("Enter year: ");
                fgets(buffer, sizeof(buffer), stdin);
                year = atoi(buffer);
                list_songs_by_year(sock_fd, year, role);
                break;
                }
            case LIST_SONGS_BY_LANGUAGE_AND_YEAR: {
                char language[LANGUAGE_LENGTH], buffer[MAX_BODY_SIZE + MAX_HEADER_SIZE];
                int year;

                printf("Enter language: ");
                fgets(language, sizeof(language), stdin);
                printf("Enter year: ");
                fgets(buffer, sizeof(buffer), stdin);
                year = atoi(buffer);
                list_songs_by_language_and_year(sock_fd, language, year, role);
                break;
                }
            case LIST_SONGS_BY_TYPE: {
                char music_type[MUSIC_TYPE_LENGTH];

                printf("Enter music type: ");
                fgets(music_type, sizeof(music_type), stdin);
                list_songs_by_type(sock_fd, music_type, role);
                break;
                }
            case LIST_SONG_INFO_BY_ID: {
                int identifier = read_int(1);
                list_song_info_by_id(sock_fd, identifier, role);
                break;
                }
            case LIST_ALL_SONGS_INFO:
                list_all_songs_info(sock_fd, role);
                break;
            default:
                // Operação para encerrrar a execução
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
            int error = remove_header(sendline, body);
            if (error == -1) {
                printf("Broken message.\n");
            }
            printf("\n%s\n", body);
        }
    }
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int receive_from_server(configuration serverConfig) { 
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAX_BUF_SIZE];
    socklen_t addr_len;
    char s[INET_ADDRSTRLEN];
    char strPort[100] = "";

    strcpy(strPort, serverConfig.port);
    strPort[strcspn(strPort, "\n")] = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(NULL, strPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof(their_addr);
    if ((numbytes = recvfrom(sockfd, buf, MAX_BUF_SIZE - 1, 0, 
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

    printf("listener: got packet from %s\n", 
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);

    close(sockfd);

    return 0;
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
    receive_from_server(serverConfig);

    // Executa lógica do cliente
    do_client_stuff(sock_fd);
    
    // Fechando o socket e saindo
    printf("Closing connection.\n");
    exit(0);

    return 0;
}
