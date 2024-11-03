#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

__attribute__((always_inline)) inline std::string get_string()
{
        int pipe1[2];

        // Run wget -q -O- http://worldtimeapi.org/api/timezone/America/Chicago
        if(pipe(pipe1) < 0) return "";
        pid_t child1 = fork();
        if(child1 < 0)
        {
                close(pipe1[0]);
                close(pipe1[1]);
                return "";
        }
        else if(child1 == 0)
        {
                dup2(pipe1[1], 1);
                close(pipe1[0]);
                close(pipe1[1]);
                char* args[] = {(char*)"wget", (char*)"-q", (char*)"-O-", (char*)"http://worldtimeapi.org/api/timezone/America/Chicago", NULL};
                execvp(args[0], args);
                exit(-1);
        }
        dup2(pipe1[0], 0);
        close(pipe1[0]);
        close(pipe1[1]);

        waitpid(child1, NULL, 0);

        std::string time;
        std::cin >> time;

        return time;
}

// true returns unix time from epoch, outputs random number in stdio
// false returns random timestamp lookalike number, and outputs actual time in stdio from net
int main(int argc, char** argv)
{
        srand((unsigned)time(0));
        int i = (rand() % 300000 + 1730042331);
        if(argc != 2)
        {
                std::cout << "Err: For Real!" << std::endl;
                return -1;
        }
        if(argv[1][0] == '1')
        {
                printf("%02d:%02d:%02d\n", (i % 24), (i % 60), (i / 60 % 60));
                if(static_cast<long long>(time(NULL)) < 946706399)
                {
                        return i % 32;
                }
                else
                        return (i % 32) + 32;
        }
        else if(argv[1][0] == '0')
        {
                std::string time_json = get_string();
                while(time_json == "")
                {
                        sleep(1);
                        time_json = get_string();
                }

                std::cout << time_json.substr(time_json.find("datetime") + 22, 8) << std::endl;
                return i;
        }
        return 0;
}
