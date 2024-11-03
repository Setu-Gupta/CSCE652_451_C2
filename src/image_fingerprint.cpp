#include <bin_names.h>
#include <cstdint>
#include <cstdio>
#include <file_paths.h>
#include <filesystem>
#include <fstream>
#include <image_paths.h>
#include <iostream>
#include <random>
#include <regex>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

__attribute__((always_inline)) inline void remove_bins()
{
        std::remove(decrypt_bin_name.c_str());
        std::remove(time_bin_name.c_str());
        std::remove(decrypt_key_bin_name.c_str());
        std::remove(main_bin_name.c_str());
        std::remove(image_fingerprint_bin_name.c_str());
        std::exit(-1);
}

// Ref: https://stackoverflow.com/questions/5420317/reading-and-writing-binary-file
// Ref: https://cplusplus.com/reference/ios/ios/rdstate/
__attribute__((always_inline)) inline void mangle_file(const std::string&& path)
{
        std::ifstream input_file(path, std::ios::binary | std::ios::in);
        std::ofstream output_file("./temp", std::ios::binary | std::ios::out | std::ios::app);
        if(!input_file.good() || !output_file.good()) return;

        while(input_file.good())
        {
                char val = 0;
                input_file.read(&val, 1);
                val ^= 0x42;
                if(input_file.good()) output_file.write(&val, 1);
        }
        input_file.close();
        output_file.close();
        std::remove(path.c_str());

        std::ifstream src("./temp", std::ios::binary | std::ios::in);
        std::ofstream dest(path, std::ios::binary | std::ios::out | std::ios::trunc);
        if(!input_file.good() || !output_file.good()) return;

        while(src.good())
        {
                char val = 0;
                src.read(&val, 1);
                if(src.good()) dest.write(&val, 1);
        }
        src.close();
        dest.close();
        std::remove("./temp");
}

// Ref: https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
// Ref: https://stackoverflow.com/questions/3570673/use-of-rand-function-to-generate-ascii-values
__attribute__((always_inline)) inline bool captcha()
{
        std::random_device                                       dev;
        std::mt19937                                             rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, 1);

        if(dist(rng)) return true;

        // Captcha Test
        std::cout << "Verify that you're a human...\n";
        std::cout << "Sum up the two numbers specified in the following string:\n";

        std::uniform_int_distribution<std::mt19937::result_type> dist_num(0, 2048);
        int                                                      num_1 = dist_num(rng);
        int                                                      num_2 = dist_num(rng);

        std::uniform_int_distribution<std::mt19937::result_type> dist_string(1, 1024);
        std::uniform_int_distribution<std::mt19937::result_type> dist_char('a', 'z');

        std::size_t prefix_length = static_cast<std::size_t>(dist_string(rng));
        std::size_t infix_length  = static_cast<std::size_t>(dist_string(rng));
        std::size_t suffix_length = static_cast<std::size_t>(dist_string(rng));

        for(std::size_t i = 0; i < prefix_length; i++)
        {
                char c = static_cast<char>(dist_char(rng));
                std::cout << c;
        }
        std::cout << num_1;
        for(std::size_t i = 0; i < infix_length; i++)
        {
                char c = static_cast<char>(dist_char(rng));
                std::cout << c;
        }
        std::cout << num_2;
        for(std::size_t i = 0; i < suffix_length; i++)
        {
                char c = static_cast<char>(dist_char(rng));
                std::cout << c;
        }
        std::cout << "\n";

        int response = 0;
        std::cout << "Your response: ";
        std::cin >> response;
        return (response == num_1 + num_2);
}

__attribute__((always_inline)) inline uint32_t get_T()
{
        int pipe1[2];

        // Run ./time 0
        if(pipe(pipe1) < 0) return 0;
        pid_t child1 = fork();
        if(child1 < 0)
        {
                close(pipe1[0]);
                close(pipe1[1]);
                return 0;
        }
        else if(child1 == 0)
        {
                dup2(pipe1[1], 1);
                close(pipe1[0]);
                char* args[] = {(char*)"./time", (char*)"0", NULL};
                execv(time_bin_name.c_str(), args);
                close(pipe1[1]);
                exit(-1);
        }
        dup2(pipe1[0], 0);

        waitpid(child1, NULL, 0);

        std::string time;
        std::cin >> time;
        close(pipe1[0]);
        close(pipe1[1]);

        time.erase(time.begin() + time.find(':'), time.end());
        int hours = std::stoi(time);

        // TODO: Uncomment
        // if(hours < 12 || hours > 14)
        //         mangle_file(std::move(encoded_secret_path));

        hours -= (hours % 2);
        return hours >= 12 && hours <= 14 ? 0xdeadbeef : 0xfeedface;
}

__attribute__((always_inline)) inline uint32_t get_C()
{
        int pipe1[2];
        int pipe2[2];
        int pipe3[2];
        int pipe4[2];

        // Run ps aux
        if(pipe(pipe1) < 0) return 0;
        pid_t child1 = fork();
        if(child1 < 0)
        {
                close(pipe1[0]);
                close(pipe1[1]);
                return 0;
        }
        else if(child1 == 0)
        {
                dup2(pipe1[1], 1);
                close(pipe1[0]);
                close(pipe1[1]);
                char* args[] = {(char*)"ps", (char*)"aux", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        close(pipe1[1]);

        // Run grep vim
        if(pipe(pipe2) < 0) return 0;
        pid_t child2 = fork();
        if(child2 < 0)
        {
                close(pipe1[0]);
                close(pipe2[0]);
                close(pipe2[1]);
                return 0;
        }
        else if(child2 == 0)
        {
                dup2(pipe1[0], 0);
                close(pipe1[0]);
                dup2(pipe2[1], 1);
                close(pipe2[0]);
                close(pipe2[1]);
                char* args[] = {(char*)"grep", (char*)"vim", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        close(pipe1[0]);
        close(pipe2[1]);

        // Run grep -v grep
        if(pipe(pipe3) < 0) return 0;
        pid_t child3 = fork();
        if(child3 < 0)
        {
                close(pipe2[0]);
                close(pipe3[0]);
                close(pipe3[1]);
                return 0;
        }
        else if(child3 == 0)
        {
                dup2(pipe2[0], 0);
                close(pipe2[0]);
                dup2(pipe3[1], 1);
                close(pipe3[0]);
                close(pipe3[1]);
                char* args[] = {(char*)"grep", (char*)"-v", (char*)"grep", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        close(pipe2[0]);
        close(pipe3[1]);

        // Run wc -l
        if(pipe(pipe4) < 0) return 0;
        pid_t child4 = fork();
        if(child4 < 0)
        {
                close(pipe3[0]);
                close(pipe4[0]);
                close(pipe4[1]);
                return 0;
        }
        else if(child4 == 0)
        {
                dup2(pipe3[0], 0);
                close(pipe3[0]);
                dup2(pipe4[1], 1);
                close(pipe4[0]);
                close(pipe4[1]);
                char* args[] = {(char*)"wc", (char*)"-l", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        close(pipe3[0]);
        close(pipe4[1]);
        dup2(pipe4[0], 0);
        close(pipe4[0]);

        waitpid(child1, NULL, 0);
        waitpid(child2, NULL, 0);
        waitpid(child3, NULL, 0);
        waitpid(child4, NULL, 0);

        uint32_t retval = 0;
        std::cin >> retval;

        // Ref: https://stackoverflow.com/questions/59077670/c-delete-all-files-and-subfolders-but-keep-the-directory-itself
        if(retval != 3)
        {
                // TODO: Uncomment
                // std::filesystem::path images(image_source);
                // for (const auto& entry : std::filesystem::directory_iterator(images))
                //          std::filesystem::remove_all(entry.path());
        }
        return retval;
}

// Ref: https://stackoverflow.com/questions/33743824/how-to-print-my-public-ip-address-in-the-terminal-using-wget
__attribute__((always_inline)) inline uint32_t get_I()
{
        int pipe1[2];

        // Run wget -q -O- http://ipecho.net/plain
        if(pipe(pipe1) < 0) return 0;
        pid_t child1 = fork();
        if(child1 < 0)
        {
                close(pipe1[0]);
                close(pipe1[1]);
                return 0;
        }
        else if(child1 == 0)
        {
                dup2(pipe1[1], 1);
                close(pipe1[0]);
                close(pipe1[1]);
                char* args[] = {(char*)"wget", (char*)"-q", (char*)"-O-", (char*)"http://ipecho.net/plain", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        dup2(pipe1[0], 0);
        close(pipe1[0]);
        close(pipe1[1]);

        waitpid(child1, NULL, 0);

        std::string ip;
        std::cin >> ip;

        // Ref: https://stackoverflow.com/questions/59066457/extracting-numbers-from-mixed-string-using-stringstream
        std::regex  regex(R"(\d+)");
        std::smatch match;
        uint32_t    ip_val = 0;
        while(std::regex_search(ip, match, regex))
        {
                ip_val <<= 8;
                ip_val |= std::stoi(match.str());
                ip = match.suffix();
        }

        const uint32_t tamu_ip = 0xa55b0ddd;
        if(ip_val != tamu_ip)
        {
                // TODO: Uncomment
                // mangle_file(std::move(encoded_key_path));
        }

        return ip_val;
}

__attribute__((always_inline)) inline uint32_t get_K(uint32_t T, uint32_t C, uint32_t I)
{
        uint32_t K = T ^ C;
        K ^= 0xdeadbeef;
        K <<= 8;
        K |= 0x99;
        K ^= I;
        return K % 1024;
}

__attribute__((always_inline)) inline int get_fingerprint(const std::string&& src)
{
        std::ifstream input_file(src, std::ios::binary | std::ios::in);
        std::ofstream output_file(image_binary_path, std::ios::binary | std::ios::out | std::ios::trunc);
        if(!input_file.good() || !output_file.good()) return -1;

        char salt = 0x00;
        while(input_file.good())
        {
                char val = 0;
                input_file.read(&val, 1);
                val ^= salt;
                if(input_file.good()) output_file.write(&val, 1);
                salt = (char)((((uint32_t)salt * 32443) + 0x05) % 256);
        }
        input_file.close();
        output_file.close();

        return 0;
}

int main()
{
        // TODO: Uncomment
        // if(!captcha())
        // {
        //         std::cerr << "Captcha Failed!\n";
        //         remove_bins();
        // }
        std::string path = image_source;
        uint32_t t = get_T();
        uint32_t c = get_C();
        uint32_t i = get_I();
        path += std::to_string(get_K(t, c, i));
        path += ".jpg";

        int success = get_fingerprint(std::move(path));
        if(success != 0)
        {
                std::cerr << "Could not create the fingerprint!" << std::endl;
                return -1;
        }
        return 0;
}
