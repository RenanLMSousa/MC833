#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#define CHUNK_SIZE 500
#define MAX_CHUNKS 10000

int compare_positions(const void *a, const void *b) {
    const unsigned char *chunk_a = *(const unsigned char **)a;
    const unsigned char *chunk_b = *(const unsigned char **)b;
    uint16_t pos_a = (chunk_a[0] << 8) | chunk_a[1];
    uint16_t pos_b = (chunk_b[0] << 8) | chunk_b[1];
    return pos_a - pos_b;
}
// Function to break the MP3 file into chunks
int break_mp3(const char *file_path, unsigned char **chunk_list) {
    FILE *mp3_file = fopen(file_path, "rb");
    if (mp3_file == NULL) {
        perror("Error opening file");
        return 0;
    }

    size_t bytes_read;
    int num_chunks = 0;

    // Read the file contents in chunks of CHUNK_SIZE bytes
    while ((bytes_read = fread(chunk_list[num_chunks] + 4, sizeof(unsigned char), CHUNK_SIZE, mp3_file)) > 0) {
        // Store metadata in the first four bytes of the chunk
        uint16_t position = num_chunks; // Assuming num_chunks won't exceed 65535
        uint32_t read_bytes = bytes_read; // Assuming bytes_read won't exceed 4294967295
        chunk_list[num_chunks][0] = (position >> 8) & 0xFF; // Store the first byte of position
        chunk_list[num_chunks][1] = position & 0xFF;        // Store the second byte of position
        chunk_list[num_chunks][2] = (read_bytes >> 8) & 0xFF; // Store the first byte of read_bytes
        chunk_list[num_chunks][3] = read_bytes & 0xFF;        // Store the second byte of read_bytes

        num_chunks++;

        // Break the loop if we reach the maximum number of chunks
        if (num_chunks >= MAX_CHUNKS) {
            printf("Music will be incomplete, size is larger than %d kb\n",(MAX_CHUNKS*CHUNK_SIZE)/1000);
            break;
        }
    }

    fclose(mp3_file);

    return num_chunks;
}

// Function to rebuild the MP3 file from the chunk list
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
    }

    fclose(mp3_file);
}
void shuffle_chunks(unsigned char **chunk_list, int num_chunks) {
    srand(time(NULL)); // Seed the random number generator

    for (int i = num_chunks - 1; i > 0; i--) {
        int j = rand() % (i + 1); // Generate a random index in [0, i]

        // Swap chunk_list[i] and chunk_list[j]
        unsigned char *temp = chunk_list[i];
        chunk_list[i] = chunk_list[j];
        chunk_list[j] = temp;
    }
}

int main() {


    //PARTE QUE FICARIA NO SERVIDOR

    unsigned char **chunk_list;
    int num_chunks;
    // Allocate memory for the array of pointers to chunks
    chunk_list = (unsigned char **)malloc(MAX_CHUNKS * sizeof(unsigned char *));

    // Allocate memory for each chunk
    for (int i = 0; i < MAX_CHUNKS; i++) {
        chunk_list[i] = (unsigned char *)malloc((CHUNK_SIZE + 4) * sizeof(unsigned char)); // 4 extra bytes for metadata
    }

    // Break the MP3 file into chunks
    num_chunks = break_mp3("storage/server_songs/1.mp3", chunk_list);
    //O SERVIDOR ENVIA TODAS AS CHUNKS


    //PARTE QUE FICARIA NO CLIENTE


    //SÓ SERVE PARA EMBARALHAR E GARANTIR QUE AS CHUNKS ESTÃO SENDO ORDENADAS ADEQUADAMENTE, SOMENTE PARA TESTES
    shuffle_chunks(chunk_list, num_chunks);
    // Rebuild the MP3 file from the chunks

    //O CLIENTE RECEBERIA AS CHUNKS E COLOCARIA ELAS EM CHUNK_LIST (OBTERIA VÁRIAS CHUNKS FORA DE ORDEM)
    //AO RECEBER AS CHUNKS O CLIENTE DEVE CONTAR QUANTAS ELE OBTEVE E SALVAR EM NUM_CHUNKS

    //RECONSTROI A MÚSICA E GERA O .MP3 NOVO
    rebuild_mp3(chunk_list, num_chunks);

    // Free the allocated memory
    for (int i = 0; i < MAX_CHUNKS; i++) {
        free(chunk_list[i]);
    }
    free(chunk_list);

    return 0;
}
