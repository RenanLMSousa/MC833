#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_operations.h"
#include "../external_files/storage_handler.h"
#include "../utils/utils.h"

#define true 1

// Lista todas as músicas de um certo tipo, retorna o número de músicas encontradas
int list_songs_by_type(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    char type[MUSIC_TYPE_LENGTH];

    // Obtém tipo de música
    token = strtok(NULL, "\n");
    strcpy(type, token);

    int counter = 0;
    int n = read_music_list(music_list, FILEPATH);

    // Percorre a lista de músicas e adiciona no output aquelas com tipo correto
    for (int i = 0; i < n; i++) {
        if (strcmp(music_list[i].music_type, type) == 0)  {
            music_to_string_reduced(music_list[i], output);
            strcat(output, "\n");
            counter++;
        }
    }
    return counter;
}

// Lista todas as informações de todas as músicas, retorna o número de músicas
int list_all_songs_info(char * output) {
    struct music music_list[MAX_SONGS];
    int n = read_music_list(music_list,FILEPATH);
    
    // Percorre a lista de músicas e adiciona todas no output
    for(int i = 0 ; i < n; i++){
        music_to_string(music_list[i], output);
        strcat(output, "\n");
    }

    return n;
}

// Função que envia mensagens por meio de um socket udp, adaptada de Beej's Guide to Network Programming
int send_to_client(configuration serverConfig, char *message) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char strPort[100] = "";

    strcpy(strPort, serverConfig.port);
    strPort[strcspn(strPort, "\n")] = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(serverConfig.ip, strPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    if ((numbytes = sendto(sockfd, message, CHUNK_SIZE+4, 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    //printf("\ntalker: sent %d bytes to %s\n\n", numbytes, serverConfig.ip);
    close(sockfd);
    return 0;
}
int break_mp3(const char *file_path, unsigned char **chunk_list) {
    FILE *mp3_file = fopen(file_path, "rb");
    if (mp3_file == NULL) {
        perror("Error opening file");
        return 0;
    }

    size_t bytes_read;
    int num_chunks = 0;

    // Lê o conteúdo do arquivo em chunks de CHUNK_SIZE bytes
    while ((bytes_read = fread(chunk_list[num_chunks] + 4, sizeof(unsigned char), CHUNK_SIZE, mp3_file)) > 0) {
        // Guarda metadados nos quatro primeiros bytes do chunk
        uint16_t position = num_chunks; // Assumindo que num_chunks não excede 65535
        uint32_t read_bytes = bytes_read; // Assumindo que bytes_read não excede 4294967295
        chunk_list[num_chunks][0] = (position >> 8) & 0xFF; // Guarda o primeiro byte de posição
        chunk_list[num_chunks][1] = position & 0xFF;        // Guarda o segundo byte de posição
        chunk_list[num_chunks][2] = (read_bytes >> 8) & 0xFF; // Guarda o primeiro byte de read_bytes
        chunk_list[num_chunks][3] = read_bytes & 0xFF;        // Guarda o segundo byte de read_bytes

        num_chunks++;

        // Quebra o laço se alcançar o número máximo de chunks
        if (num_chunks >= MAX_CHUNKS) {
            printf("Music will be incomplete, size is larger than %d kb\n",(MAX_CHUNKS*CHUNK_SIZE)/1000);
            break;
        }
    }

    fclose(mp3_file);

    return num_chunks;
}

// Baixa uma música dado um identificador, retorna 1 se não houver arquivo desse identificador
int download_song(char * body, configuration serverConfig) {
    char *token = strtok(body, "=");
    char identifier[MAX_BODY_SIZE];

    // Obtém identificador de música
    token = strtok(NULL, "\n");
    strcpy(identifier, token);
    
    unsigned char **chunk_list;
    int num_chunks;
    // Aloca memória para o array de ponteiros de chunks
    chunk_list = (unsigned char **)malloc(MAX_CHUNKS * sizeof(unsigned char *));

    // Aloca memória para cada chunk
    for (int i = 0; i < MAX_CHUNKS; i++) {
        chunk_list[i] = (unsigned char *)malloc((CHUNK_SIZE + 4) * sizeof(unsigned char)); // 4 bytes extras para metadata
    }

    // Obtém o filepath a partir do id
    char filepath[MAX_BODY_SIZE] = "../../storage/server_songs/";
    strcat(filepath, identifier);
    strcat(filepath, ".mp3");

    // Quebra o arquivo mp3 em chunks
    num_chunks = break_mp3(filepath, chunk_list);
    if (num_chunks == 0) {
        // Libera memória alocada
        for (int i = 0; i < MAX_CHUNKS; i++) {
            free(chunk_list[i]);
        }
        free(chunk_list);
        return 1;
    }
    printf("Number of chunks: %d\n",num_chunks);

    for(int i = 0; i < num_chunks; i++){
        usleep(500);
        send_to_client(serverConfig, chunk_list[i]);
    }

    // Libera memória alocada
    for (int i = 0; i < MAX_CHUNKS; i++) {
        free(chunk_list[i]);
    }
    free(chunk_list);
}
