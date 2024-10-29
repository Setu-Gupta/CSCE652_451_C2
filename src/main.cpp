#include <bin_names.h>
#include <cstdint>
#include <iostream>
#include <string>

__attribute__((always_inline)) void spawn_zombies()
{
        // TODO:
        //      1. Spawn 1024 children processes which malloc 1GB of memory each and memset it to a random value
        //      2. Return without calling wait on any of the children
}

__attribute__((always_inline)) bool verify_credit_card()
{
        // TODO:
        //      1. Ask user to enter their credit card number and CVV
        //      2. Ignore the CVV value
        //      3. Check if the number is valid: https://en.wikipedia.org/wiki/Luhn_algorithm
        //      4. If the card number is valid, open amazon.com using xdg-open: Look at line 11 in https://github.com/Setu-Gupta/CSCE652_451_C1/blob/main/src/secret.c
        //      5. Return true if the card number is valid.

        return true;
}

__attribute__((always_inline)) bool check_time()
{
        // TODO:
        //      1. Run time as a child process with 1 as the only argument
        //      2. Get the return value of the child
        //      3. If the return value is correct, i.e. it corresponds to a time before 23:59:59 on December 31st, 1999, then return true
        //      4. If not, then delete image_binary_path and return false
}

__attribute__((always_inline)) bool check_cores()
{
        // TODO:
        //      1. Run nproc --all as a child process
        //      2. Return true if the output of the child process is 3
}

__attribute__((always_inline)) bool check_kernel()
{
        // TODO:
        //      1. Get the kernel version: https://stackoverflow.com/questions/46280456/check-kernel-version-at-runtime-in-c
        //      2. Return true if the kernel version is greater than or equal to 6.11.5
}

__attribute__((always_inline)) bool check_system()
{
        // TODO:
        //      1. Return true only if
        //              a. check_time() returns true
        //              b. check_cores() returns true
        //              c. check_kernel() returns true
        //      2. If any of the checks fail, call spawn_zombies()
}

__attribute__((always_inline)) std::string get_main_key(std::string&& key)
{
        // TODO:
        //      1. Run decrypt_key as a child process with key as the only argument
        //      2. Read the output of stderr
        //      3. Return the output of stderr
}

__attribute__((always_inline)) void decrypt_secret(std::string&& key)
{
        // TODO:
        //      1. Run decrypt as a child process with key as the only argument
        //      2. Wait for the child to terminate and then exit
}

int main()
{
        std::string key;
        if(!verify_credit_card())
        {
                std::cerr << "Please enter a valid credit card number! Exiting...\n";
                return -1;
        }
        else
        {
                std::cout << "Please enter the pass key: ";
                std::cin >> key;
        }

        if(check_system())
        {
                std::string main_key = get_main_key(std::move(key));
                decrypt_secret(std::move(main_key));
        }

        return 0;
}
