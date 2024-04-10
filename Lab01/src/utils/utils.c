#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "utils.h"

// Envia todos os bytes do buffer
int send_all(int s, char *buf, int len) {
    int total = 0;
    int bytesleft = len;
    int n;
    while(total < len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { 
            return -1; 
        }
        total += n;
        bytesleft -= n;
        }
    
    return total;
}

// Recebe todos os bytes enviados por send_all
int recv_all(int sock_fd, char * buf) {
    char message[MAXLINE];
    char copy_buffer[MAXLINE]; // copia da mensagem
    char body[MAX_BODY_SIZE]; // armazena o corpo da mensagem, só vai ser passado como parâmetro
    int total_size = -2; // -2 marca que ainda não foi inicializado

    while (true) {
        int n = recv(sock_fd, message, sizeof(message), 0);
        if (n < 0) {
            perror("recv");
            return n;
        } else if (n == 0) {
            printf("str_cli: server terminated prematurely\n");
            return n;
        } else {
            if (total_size == -2) {
                total_size = remove_cabecalho(copy_buffer, body);
            }
            strcat(buf, message);
            if (strlen(message) * sizeof(char) == total_size) {
                return total_size;
            }
        }
        memset(message, 0, sizeof(message));
    }
}

// Verifica se o header representa o servidor, se sim retorna o tamanho, se não retorna -1
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
    // Como o servidor tem role = 2, verifica isso
    if (role != 2) {
        return -1;
    }

    // Operação
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    operation = atoi(token);
    // Como o servidor tem operation = -1, verifica isso
    if (operation != -1) {
        return -1;
    }

    return size;
}

// Remove o cabeçalho do servidor, escrevendo o corpo em body, retorna -1 se houver erro e o tamanho do body caso contrário
int remove_cabecalho(char * message, char * body) {
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

    // Token removeu o \n final do corpo, adicionamos de volta
    strcat(strBody, "\n");

    // Exibindo o conteúdo separado
    int declared_size = verificar_header(strHeader);
    strcpy(body, strBody);

    return declared_size;
}

// Função auxiliar para substituir o tamanho no cabeçalho
void troca_size(char *str, int size) {
    char sizeStr[7];
    sprintf(sizeStr, "%06d", size); // Converte inteiro para string com 0s na frente
    char *placeholder = strstr(str, "000000"); // Encontra os 0s na string
    if (placeholder != NULL) {
        memcpy(placeholder, sizeStr, strlen(sizeStr)); // Faz a substituição
    }
}

// Anexa cabeçalho da operação ao corpo da mensagem
void anexar_header_operacao(char * message , int operacao, int role){
    char strOut[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "", strMessage[MAX_BODY_SIZE] = "";

    // Copia a mensagem para um buffer temporário
    strcat(strMessage, message);
    strcat(strMessage,"\n");

    // Calcula o tamanho da mensagem
    sprintf(strOut, "#HEADER\nSize=000000\nRole=%d\nOperation=%d\n#BODY\n", role, operacao);
    strcat(strOut, strMessage);
    int size = strlen(strOut) * sizeof(char);
    troca_size(strOut, size);

    // Coloca a mensagem com o cabeçalho na variável original
    strcpy(message, strOut);
}