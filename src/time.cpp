#include <stdio.h>
#include <ctime>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib> 


using namespace std;

__attribute__((always_inline)) inline string get_I()
{
        int pipe1[2];

        // Run wget -q -O- http://worldtimeapi.org/api/timezone/America/Chicago
        if(pipe(pipe1) < 0)
                 return 0;
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
int main(int argc, char** argv){
        srand((unsigned)time(0));
        int i = (rand()%300000 + 1730042331);
        if (argc != 2){
                cout << "Err: For Real!" << endl;
                return -1;
        }
        if (argv[1][0] == '1'){
                printf("%02d:%02d:%02d\n", (i%24),(i%60), (i/60%60));
		if (static_cast<int>(time(NULL)) < 946641599)
			return i%32;
		else
			return (i%32) + 32;
        }
        else if(argv[1][0] == '0'){
                string time_json = get_I();
                cout << time_json.substr(time_json.find("datetime")+22, 8) << endl;
                return i;
        }
	return 0;
}
