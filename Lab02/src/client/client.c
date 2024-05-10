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
void do_client_stuff(int sock_fd) {
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
                download_song(sock_fd, identifier);
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

int receive_from_server(configuration serverConfig, unsigned char * out) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char * buf = out;
    socklen_t addr_len;
    char s[INET_ADDRSTRLEN];
    char strPort[100] = "";

    strcpy(strPort, serverConfig.port);
    strPort[strcspn(strPort, "\n")] = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, strPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
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

    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    // Set timeout to 2 seconds
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
    if (ready == -1) {
        perror("select");
        return 3;
    } else if (ready == 0) {
        printf("Timeout occurred. Exiting...\n");
        close(sockfd);
        return 4; // Timeout
    }

    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAX_BUF_SIZE - 1, 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            close(sockfd);
            return 5;
    }

    printf("listener: got packet from %s\n",
           inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    //printf("listener: packet contains \"%s\"\n\n", buf);

    close(sockfd);

    return 0;
}
int compare_positions(const void *a, const void *b) {
    const unsigned char *chunk_a = *(const unsigned char **)a;
    const unsigned char *chunk_b = *(const unsigned char **)b;
    uint16_t pos_a = (chunk_a[0] << 8) | chunk_a[1];
    uint16_t pos_b = (chunk_b[0] << 8) | chunk_b[1];
    return pos_a - pos_b;
}
void rebuild_mp3(unsigned char **chunk_list, int num_chunks) {
    // Sort the chunk list based on positions
    qsort(chunk_list, num_chunks, sizeof(unsigned char *), compare_positions);
    // Rebuild the MP3 file
    FILE *mp3_file = fopen("reconstructed.mp3", "wb");
    if (mp3_file == NULL) {
        perror("Error creating MP3 file");
        return;
    }

    for (int i = 0; i < num_chunks; i++) {
        fwrite(chunk_list[i] + 4, sizeof(unsigned char), CHUNK_SIZE, mp3_file);
        printf("Escrevendo chunk:  %d\n",i);
    }

    fclose(mp3_file);
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

    int num_chunks = 1;
    unsigned char **chunk_list;
    // Allocate memory for the array of pointers to chunks
    chunk_list = (unsigned char **)malloc(MAX_CHUNKS * sizeof(unsigned char *));

    // Allocate memory for each chunk
    for (int i = 0; i < MAX_CHUNKS; i++) {
        chunk_list[i] = (unsigned char *)malloc((CHUNK_SIZE + 4) * sizeof(unsigned char)); // 4 extra bytes for metadata
    }

    while(receive_from_server(serverConfig,chunk_list[num_chunks]) != 4){
        printf("%d\n",num_chunks);
        num_chunks++;
    }
    rebuild_mp3(chunk_list,num_chunks);
    // Executa lógica do cliente
    do_client_stuff(sock_fd);
    
    // Fechando o socket e saindo
    printf("Closing connection.\n");
    exit(0);

    return 0;
}
