#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline __attribute__((always_inline)) void encrypt_file(const uint8_t* key, const char* input_file, const char* output_file)
{
        uint8_t buffer[512];
        size_t  bytes_read;

        FILE* in  = fopen(input_file, "rb");
        FILE* out = fopen(output_file, "wb");

        if(!in || !out)
        {
                perror("File error");
                exit(1);
        }

        while((bytes_read = fread(buffer, 1, sizeof(buffer), in)) > 0)
        {
                for(size_t i = 0; i < bytes_read; i++)
                {
                        buffer[i] ^= key[i];
                }
                fwrite(buffer, 1, bytes_read, out);
        }

        fclose(in);
        fclose(out);
}

inline __attribute__((always_inline)) uint8_t* read_key(const char* key_file, size_t* key_length)
{
        FILE* key_fp = fopen(key_file, "rb");
        if(!key_fp)
        {
                perror("Could not open key file");
                exit(1);
        }

        fseek(key_fp, 0, SEEK_END);
        *key_length = ftell(key_fp);
        fseek(key_fp, 0, SEEK_SET);

        uint8_t* key = (uint8_t*)malloc(*key_length);
        fread(key, 1, *key_length, key_fp);
        fclose(key_fp);
        return key;
}

int main(int argc, char* argv[])
{
        if(argc != 4)
        {
                fprintf(stderr, "Usage: %s <key_file> <input_file> <output_file>\n", argv[0]);
                return 1;
        }

        char key[512];
        strncpy(key, argv[1], 512);
        const char* input_file  = argv[2];
        const char* output_file = argv[3];

        encrypt_file((uint8_t*)key, input_file, output_file);

        printf("Encryption complete. Output saved to %s\n", output_file);
        return 0;
}
