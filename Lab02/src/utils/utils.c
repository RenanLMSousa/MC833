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
    char copy_buffer[MAXLINE] = ""; // Cópia da mensagem
    char body[MAX_BODY_SIZE]; // Armazena o corpo da mensagem, só vai ser passado como parâmetro e não usado
    int total_size = -2; // -2 marca que ainda não foi inicializado

    while (true) {
        int n = recv(sock_fd, message, sizeof(message), 0);
        if (n < 0) {
            perror("recv");
            return n;
        } else if (n == 0) {
            printf("Peer terminated.\n");
            return n;
        } else {
            if (total_size == -2) {
                strcpy(copy_buffer, message);
                total_size = remove_header(copy_buffer, body);
            }
            strcat(buf, message);
            int curr_size = strlen(buf) * sizeof(char);
            if (curr_size >= total_size) {
                return total_size;
            }
        }
        memset(message, 0, sizeof(message));
    }
}

// Quebra a string em tokens para obter o tamanho da mensagem e o retorna
int get_size(char * header) {
    char *token = strtok(header, "=");
    token = strtok(NULL, "\n");
    int size = atoi(token);

    return size;
}

// Remove o cabeçalho do servidor, escrevendo o corpo em body, retorna o tamanho da mensagem
int remove_header(char * message, char * body) {
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
    int declared_size = get_size(strHeader);
    strcpy(body, strBody);

    return declared_size;
}

// Função auxiliar para substituir o tamanho no cabeçalho
void set_size(char *str, int size) {
    char sizeStr[7];
    // Converte inteiro para string com 0s na frente
    sprintf(sizeStr, "%06d", size);
    // Encontra os 0s na string
    char *placeholder = strstr(str, "000000"); 

    // Faz a substituição
    if (placeholder != NULL) {
        memcpy(placeholder, sizeStr, strlen(sizeStr));
    }
}

// Monta a mensagem
void build_message(char * message , int operation) {
    char strOut[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "", strMessage[MAX_BODY_SIZE] = "";

    // Copia a mensagem para um buffer temporário
    strcat(strMessage, message);
    strcat(strMessage,"\n");

    // Calcula o tamanho da mensagem
    sprintf(strOut, "#HEADER\nSize=000000\nOperation=%d\n#BODY\n", operation);
    strcat(strOut, strMessage);
    int size = strlen(strOut) * sizeof(char);
    set_size(strOut, size);

    // Coloca a mensagem com o cabeçalho na variável original
    strcpy(message, strOut);
}