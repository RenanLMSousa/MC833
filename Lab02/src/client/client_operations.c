#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_operations.h"
#include "../music/music.h"
#include "../external_files/storage_handler.h"
#include "../utils/utils.h"

#define MAX_INT_STR 12
#define true 1

// Lista todas as músicas de um certo tipo
void list_songs_by_type(int sock_fd,  char * type) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "MusicType=";
    strcat(message, type);

    // Forma a string da mensagem e envia para o servidor
    build_message(message, LIST_SONGS_BY_TYPE);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as informações de todas as músicas
void list_all_songs_info(int sock_fd) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    
    // Forma a string da mensagem e envia para o servidor
    build_message(message, LIST_ALL_SONGS_INFO);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Função auxiliar adaptada de Beej's Guide to Network Programming
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Função que recebe mensagens por meio de um socket udp, adaptada de Beej's Guide to Network Programming
int receive_from_server(configuration serverConfig, unsigned char * out) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char * buf = out;
    socklen_t addr_len;
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

    // Define timeout para 2 segundos
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int ready = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
    if (ready == -1) {
        perror("select");
        return 3;
    } else if (ready == 0) {
        close(sockfd);
        return 4; // Timeout
    }

    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(sockfd, buf, MAX_BODY_SIZE - 1, 0,
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            close(sockfd);
            return 5;
    }

    buf[numbytes] = '\0';
    close(sockfd);

    return 0;
}

// Função auxiliar para comparar posições em duas listas
int compare_positions(const void *a, const void *b) {
    const unsigned char *chunk_a = *(const unsigned char **)a;
    const unsigned char *chunk_b = *(const unsigned char **)b;
    uint16_t pos_a = (chunk_a[0] << 8) | chunk_a[1];
    uint16_t pos_b = (chunk_b[0] << 8) | chunk_b[1];
    return pos_a - pos_b;
}

// Reconstrói e salva o arquivo mp3, colocando os chunks na posição correta
void rebuild_mp3(unsigned char **chunk_list, int num_chunks, int identifier) {
    // Ordena lista de chunks baseado nas posições
    qsort(chunk_list, num_chunks, sizeof(unsigned char *), compare_positions);
    
    // Reconstrói o arquivo MP3
    char filepath[MAX_BODY_SIZE] = "";
    sprintf(filepath, "%d", identifier);
    strcat(filepath, ".mp3");
    
    FILE *mp3_file = fopen(filepath, "wb");
    if (mp3_file == NULL) {
        perror("Error creating MP3 file");
        return;
    }

    int i;
    for (i = 0; i < num_chunks; i++) {
        fwrite(chunk_list[i] + 4, sizeof(unsigned char), CHUNK_SIZE, mp3_file);
    }
    printf("Download finished. %d chunks writen to %s\n", i - 1, filepath);

    fclose(mp3_file);
}

// Baixa uma música dado um identificador
void download_song(int sockf_fd, int identifier, configuration serverConfig) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    append_int(message, "Identifier=", identifier);
    // Forma a string da mensagem e envia para o servidor
    build_message(message, DOWNLOAD_SONG);
    if (send_all(sockf_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }

    printf("\nDownload started, please wait.\n");
    int num_chunks = 1;
    unsigned char **chunk_list;
    // Aloca memória para o array de ponteiros de chunks
    chunk_list = (unsigned char **)malloc(MAX_CHUNKS * sizeof(unsigned char *));

    // Aloca memória para cada chunk
    for (int i = 0; i < MAX_CHUNKS; i++) {
        chunk_list[i] = (unsigned char *)malloc((CHUNK_SIZE + 4) * sizeof(unsigned char)); // 4 extra bytes for metadata
    }

    while(receive_from_server(serverConfig,chunk_list[num_chunks]) != 4){
        num_chunks++;
    }
    printf("%d chunks received\n", num_chunks - 1);
    rebuild_mp3(chunk_list, num_chunks, identifier);

    unsigned char *buf;
    // Libera memória alocada
    // for (int i = 0; i < MAX_CHUNKS; i++) {
    //     free(chunk_list[i]);
    // }
    // free(chunk_list);
}