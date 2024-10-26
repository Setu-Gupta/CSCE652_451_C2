#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

void encrypt_file(const uint8_t *key, const char *input_file, const char *output_file){
    uint8_t buffer[512];
    size_t bytes_read;

    FILE *in = fopen(input_file, "rb");
    FILE *out = fopen(output_file, "wb");

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            buffer[i] ^= key[i];
        }
        fwrite(buffer, 1, bytes_read, out);
    }

    fclose(in);
    fclose(out);
}

//example usage (me testing that it worked)
/*
int main(){
    srand(time(NULL));
    uint8_t key[512];
    for (size_t i = 0; i < 512; i++) {
        key[i] = rand() % 256;  // Random byte value (0-255)
    }

    encrypt_file(key, "secret", "EncSecret");
    encrypt_file(key, "EncSecret", "temp_test");

}*/