#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

/*
CODE FOUR

To compile: gcc -o aes aes.c -lssl -lcrypto
To run: ./aes input_to_encrypt_file_name key_file_name
        ex: ./aes K1.txt K2.txt

Update extend_key so that time pulls from code three
*/
#define BLOCK_SIZE 16
#define INPUT_SIZE 512
#define KEY_SIZE 8
#define ENC_FILE_SIZE (1024 * 1024)

void extend_key(const unsigned char* key64, unsigned char* key128) {
    // Simple extension: Copy the 64-bit key and pad with zeros
    memcpy(key128, key64, KEY_SIZE);
    memset(key128 + KEY_SIZE, 0, 16 - KEY_SIZE); // Pad with zeros

    // switch to call time function from code 3
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    int current_hour = tm_info->tm_hour;
    unsigned char hour_group = (unsigned char) (current_hour / 2); // Dependent on two hour intervals
    key128[15] = hour_group;
}

void do_AES_encrypt(const unsigned char* input, const unsigned char* key64, unsigned char* output) {
    unsigned char key128[16];
    extend_key(key64, key128); // Extend the 64-bit key to 128 bits

    AES_KEY enc_key;
    AES_set_encrypt_key(key128, 128, &enc_key); // Use 128 bits
    AES_encrypt(input, output, &enc_key);
}



void encrypt_k1(const unsigned char* input, const unsigned char* key, unsigned char* output) {
    for (int i = 0; i < INPUT_SIZE; i += BLOCK_SIZE) {
        do_AES_encrypt(input + i, key, output + i);
    }
}



void create_enc_key_file(const unsigned char* input, unsigned char* enc_key_file) {
    // Creates a 1 MB sized file filled with random clutter, at a random offset lies the encrypted 4096 bit key

    for (int i = 0; i < ENC_FILE_SIZE; i++) {
        enc_key_file[i] = rand() % 256;
    }
    int offset = rand() % (ENC_FILE_SIZE - INPUT_SIZE -4);
    memcpy(enc_key_file, &offset, sizeof(offset));
    memcpy(enc_key_file + offset, input, INPUT_SIZE);
}

void write_enc_key_file(const char* filename, const unsigned char* enc_key_file) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    fwrite(enc_key_file, 1, ENC_FILE_SIZE, file);
    fclose(file);
    printf("EncKeyFile written to %s\n", filename);
}



int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <K1 file> <K2 file>\n", argv[0]);
        return 1;
    }

    unsigned char input[INPUT_SIZE] = {0};  // 512-byte input
    unsigned char key[KEY_SIZE] = {0};  // 8-byte key
    unsigned char encrypted[INPUT_SIZE] = {0};
    unsigned char enc_key_file[ENC_FILE_SIZE];

    srand((unsigned int)time(NULL));

    // Open the file and read the number
    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        perror("Error opening input file");
        return 1;
    }   
    size_t bytes_read = fread(input, 1, INPUT_SIZE, input_file);
    fclose(input_file);

    FILE *key_file = fopen(argv[2], "rb");
    if (!key_file) {
        perror("Error opening key file");
        return 1;
    }
    bytes_read = fread(key, 1, KEY_SIZE, key_file);
    fclose(key_file);

    // Encrypt the input
    encrypt_k1(input, key, encrypted);
    // printf("Encrypted data:\n");
    // for (int i = 0; i < INPUT_SIZE; i++) {
    //     printf("%02x", encrypted[i]);
    // }
    // printf("\n");

    // Write to file
    create_enc_key_file(encrypted, enc_key_file);
    write_enc_key_file("EncKeyFile", enc_key_file);

    return 0;
}
