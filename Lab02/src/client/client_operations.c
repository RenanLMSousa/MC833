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
int receive_chunks_from_server(unsigned char *buf, char * port) { 
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char s[INET_ADDRSTRLEN];
    char strPort[100] = "";

    strcpy(strPort, port);
    strPort[strcspn(strPort, "\n")] = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

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

    // printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof(their_addr);
    if ((numbytes = recvfrom(sockfd, buf, MAX_BODY_SIZE - 1, 0, 
        (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

    // printf("listener: got packet from %s\n", 
    //    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
    // printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    // printf("listener: packet contains \"%s\"\n\n", buf);

    close(sockfd);

    return 0;
}

// Baixa uma música dado um identificador
void download_song(int sockf_fd, int identifier, char * port) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    append_int(message, "Identifier=", identifier);
    // Forma a string da mensagem e envia para o servidor
    build_message(message, DOWNLOAD_SONG);
    if (send_all(sockf_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }

    // Aloca memória para o array de ponteiros de chunks
    unsigned char **chunk_list = (unsigned char **)malloc(MAX_CHUNKS * sizeof(unsigned char *));

    // Aloca memória para cada chunk
    for (int i = 0; i < MAX_CHUNKS; i++) {
        chunk_list[i] = (unsigned char *)malloc((CHUNK_SIZE + 4) * sizeof(unsigned char)); // 4 extra bytes for metadata
    }

    int num_chunks = 0;
    while (true) {
        receive_chunks_from_server(chunk_list[num_chunks], port);
        printf("recebido %d\n", num_chunks);
        num_chunks++;
    }
}