#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <file_paths.h>

/*

CODE FIVE

To compile: gcc -o decrypt_key decrypt_key.c -lssl -lcrypto
To run: ./decrypt_key  ../data/key2.bin ../data/EncKeyFile

Under the correct conditions, given inputs of the correct K2 and an encrypted file, will return an output of the decrypted K1
Both the key and encrypted file must be txt files.

Note:
-The kernel validation test is currently commented out.
-The path/checksum of the B binary file must be set (PATH_TO_B and B_CHECKSUM)
-In extend_key, we get system time, which still needs to be switched to the function in code three
*/

#define BLOCK_SIZE    16
#define INPUT_SIZE    512
#define KEY_SIZE      8
#define ENC_FILE_SIZE (1024 * 1024)

#define B_CHECK_SUM "26caef843a0005434f12794088e1f8e0e22245eb00f396ded990a7dd882d4b54"

inline __attribute((always_inline)) void extend_key(const unsigned char* key64, unsigned char* key128)
{
        // Simple extension: Copy the 64-bit key and pad with zeros
        memcpy(key128, key64, KEY_SIZE);
        memset(key128 + KEY_SIZE, 0, 16 - KEY_SIZE); // Pad with zeros

        // switch to call time function from code 3
        time_t        t            = time(NULL);
        struct tm*    tm_info      = localtime(&t);
        int           current_hour = tm_info->tm_hour;
        unsigned char hour_group   = (unsigned char)(current_hour / 2); // Dependent on two hour intervals
#ifdef DEBUG_FORCE_TIME
        (void)hour_group;
        key128[15] = 2;
#else
        key128[15] = hour_group;
#endif
}

inline __attribute((always_inline)) void decrypt_k1(const unsigned char* input, const unsigned char* key, unsigned char* output)
{
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key, NULL);
        int out_len = INPUT_SIZE;
        EVP_DecryptUpdate(ctx, output, &out_len, input, INPUT_SIZE);
        EVP_DecryptFinal(ctx, output + out_len, &out_len);
        EVP_CIPHER_CTX_free(ctx);
}

inline __attribute((always_inline)) int validate_access_time(unsigned char* key128)
{
        // If the hour group is not currently 2 (hours 4 and 5), return false
        // (The EncKeyFile must be encrypted/made in hour group 2 as well, is that the desired functionality?)
        if(key128[15] != 2)
        {
                return 0;
        }
        return 1;
}

inline __attribute((always_inline)) int validate_kernel_version()
{
        // If the current kernel version is not the latest arch linux version, return false
        struct utsname buffer;

        // Get system information
        if(uname(&buffer) == 0)
        {
                char version[128];
                memcpy(version, buffer.release, sizeof(buffer.release) % 128);
#ifdef DEBUG_VALIDATE_FUNCS
                printf("Kernel Version: %.10s\n", version);
#endif
                if(strncmp(version, "6.11.3", 6) >= 0)
                {
                        return 1;
                }
                else
                {
                        return 0;
                }
        }
        else
        {
                perror("uname");
                return 0;
        }

        return 0;
}

inline __attribute((always_inline)) int validate_ip_address()
{
        // If not on the TAMU network, return false
        FILE* fp = NULL;
        char  buffer[128];

        fp = popen("curl -s http://ifconfig.me", "r");
        if(fp == NULL)
        {
                fprintf(stderr, "Failed to run curl command\n");
                return 1;
        }

        if(fgets(buffer, sizeof(buffer), fp) != NULL)
        {
                if(strncmp(buffer, "165.91.", 7) == 0)
                {
                        pclose(fp);
                        return 1;
                }
                if(strncmp(buffer, "165.95.", 7) == 0)
                {
                        pclose(fp);
                        return 1;
                }
                if(strncmp(buffer, "128.194.", 8) == 0)
                {
                        pclose(fp);
                        return 1;
                }
        }
        pclose(fp);
        return 0;
}

inline __attribute((always_inline)) int validate_checksum()
{
        // If the checksum of the hardcoded file path does not match its hardcoded checksum, return false
        unsigned char hash[SHA256_DIGEST_LENGTH];
        unsigned char buffer[1024];
        FILE*         file = fopen(image_binary_path.c_str(), "rb");
        if(!file) return 0;

        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
        size_t bytes;
        while((bytes = fread(buffer, 1, 1024, file)) > 0)
        {
                EVP_DigestUpdate(ctx, buffer, bytes);
        }
        EVP_DigestFinal_ex(ctx, hash, NULL);
        fclose(file);

        char hash_string[65];
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
                sprintf(hash_string + (i * 2), "%02x", hash[i]);
        }
        return strcmp(hash_string, B_CHECK_SUM) == 0;
}

inline __attribute((always_inline)) int validate_file_creation_time()
{
        // If the provided file was not created in hours 12 or 13 (group 6), return false
        struct stat attr;
        if(stat(image_binary_path.c_str(), &attr) != 0) return 0;

        struct tm* time_info = localtime(&attr.st_ctime);
#ifdef DEBUG_VALIDATE_FUNCS
        printf("File creation time %d\n", time_info->tm_hour);
#endif
        return (time_info->tm_hour >= 12 && time_info->tm_hour < 14);
}

inline __attribute((always_inline)) void mangle(const char* filepath)
{
#ifdef DEBUG_NO_KEY_MANGLE
        printf("Mangling Key!!\n");
        (void)filepath;
#else
        // For mangling the file when the validation checks fail
        unsigned char enc_key_file[ENC_FILE_SIZE];

        FILE* enc_file = fopen(filepath, "rb");
        if(!enc_file)
        {
                perror("Error opening EncKeyFile");
                return;
        }
        (void)fread(enc_key_file, 1, ENC_FILE_SIZE, enc_file);
        fclose(enc_file);

        srand((unsigned int)time(NULL)); // Seed the random number generator with current time
        for(int i = 4; i < ENC_FILE_SIZE; i++)
        {
                if(rand() % 2)
                {
                        // Flip a random bit in the byte at position i
                        unsigned char random_bit = 1 << (rand() % 8);
                        enc_key_file[i] ^= random_bit;
                }
        }

        FILE* file = fopen(filepath, "wb");
        if(file == NULL)
        {
                perror("Error opening file");
                return;
        }
        fwrite(enc_key_file, 1, ENC_FILE_SIZE, file);
        fclose(file);

        printf("Attempting key...\n"); // a misleading string
#endif
}

inline __attribute((always_inline)) void read_enc_key(const char* filepath, unsigned char* payload)
{
        unsigned char enc_key_file[ENC_FILE_SIZE];

        FILE* enc_file = fopen(filepath, "rb");
        if(!enc_file)
        {
                perror("Error opening EncKeyFile");
                return;
        }
        size_t bytes_read = fread(enc_key_file, 1, ENC_FILE_SIZE, enc_file);
        fclose(enc_file);

        int offset;
        if(bytes_read >= 4)
        {
                offset = *((int*)enc_key_file);
                if(bytes_read >= (size_t)offset + INPUT_SIZE)
                {
                        memcpy(payload, enc_key_file + offset, INPUT_SIZE);
                }
        }
}

int main(int argc, char* argv[])
{
        if(argc != 3)
        {
                fprintf(stderr, "Usage: %s <encrypted file> <key>\n", argv[0]);
                return 1;
        }

        // unsigned char key[KEY_SIZE] = {0};  // 8-byte key
        unsigned char* key                   = (unsigned char*)argv[2];
        unsigned char  payload[INPUT_SIZE]   = {0};
        unsigned char  decrypted[INPUT_SIZE] = {0};
        unsigned char  key128[16];

        srand((unsigned int)time(NULL));

        const char* enc_filepath = argv[1];

        /*
        FILE *key_file = fopen(argv[1], "rb");
        if (!key_file) {
            perror("Error opening key file");
            return 1;
        }
        */
        // (void) fread(key, 1, KEY_SIZE, stdin);
        // fclose(key_file);

        extend_key(key, key128); // Extend the 64-bit key to 128 bits

        if(!validate_ip_address() || !validate_access_time(key128) || !validate_kernel_version() || !validate_checksum() || !validate_file_creation_time())
        {
                mangle(enc_filepath);
        }

        // Retrieve encrypted payload from file
        read_enc_key(enc_filepath, payload);
        // printf("Payload:\n");
        // for (int i = 0; i < INPUT_SIZE; i++) {
        //     printf("%02x", payload[i]);
        // }
        // printf("\n");

        // Decrypt the data
        decrypt_k1(payload, key128, decrypted);
        for(int i = 0; i < INPUT_SIZE; i++)
        {
                fprintf(stderr, "%02x", decrypted[i]);
        }

#ifdef DEBUG_SHOW_VALIDATE_FUNCS
        printf("Connected to network: %d\n", validate_ip_address());
        printf("Between hours of 4 and 5: %d\n", validate_access_time(key128));
        printf("Validate kernel version: %d\n", validate_kernel_version());
        printf("Validate checksum: %d\n", validate_checksum());
        printf("Validate creation time: %d\n", validate_file_creation_time());
#endif

        return 0;
}
