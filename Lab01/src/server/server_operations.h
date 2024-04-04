#ifndef SERVER_OPERATIONS_H
#define SERVER_OPERATIONS_H

#include "../music/music.h"

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

#define MAX_SONGS 100

// Definições para operações
#define CADASTRAR_UMA_MUSICA 1
#define REMOVER_UMA_MUSICA 2
#define LISTAR_MUSICAS_POR_ANO 3
#define LISTAR_MUSICAS_POR_IDIOMA_E_ANO 4
#define LISTAR_MUSICAS_POR_TIPO 5
#define LISTAR_INFO_MUSICA_POR_ID 6
#define LISTAR_TODAS_INFOS_MUSICAS 7

// Função para cadastrar uma nova música, retorna 1 se não foi possivel cadastrar
int cadastrar_musica(char * body);

// Função para remover uma música a partir de seu identificador, retorna 1 se não existe música com esse identificador
int remover_musica(char * body);

// Função para listar todas as músicas lançadas em um determinado ano, retorna o número de músicas encontradas
int listar_musicas_por_ano(char * body, char * output);

// Função para listar todas as músicas em um dado idioma lançadas em um certo ano, retorna o número de músicas encontradas
int listar_musicas_por_idioma_e_ano(char * body, char * output);

// Função para listar todas as músicas de um certo tipo, retorna o número de músicas encontradas
int listar_musicas_por_tipo(char * body, char * output);

// Função para listar todas as informações de uma música dado o seu identificador, retorna 1 se não existe música com esse identificador
int listar_info_musica_por_id(char * body, char * output);

// Função para listar todas as informações de todas as músicas, retorna o número de músicas
int listar_todas_infos_musicas(char * body, char * output);

#endif /* SERVER_OPERATIONS_H */
