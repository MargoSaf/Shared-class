#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "src/shared_class/shared_class.hpp"

#define PARENT "PARENT PROCESS:         "
#define CHILDE "CHILDE PROCESS:         "
bool is_msg_updated = false;
int run_share_messages_process();
int run_second_childe();
void message_handler(std::string msg);
void message_handler_childe(std::string msg);

int main (void) {
    if(!shared_class::register_callback(message_handler)){
        std::cerr << "Failed to register callback, end process" <<std::endl;
        return -1;
    }
    std::cout<< PARENT << "callback registered" <<std::endl; 
    // fork and run share_messages process (with exec)
    int share_messages_process_pid = run_share_messages_process();
    if(share_messages_process_pid == -1){
        std::cerr << PARENT << "Cannot fork!!" << std::endl;
        return -1;
    }

    // 
    int second_childe_pid = run_second_childe();
    if (second_childe_pid == -1) {
        std::cerr<< PARENT <<"Fork failed" <<std::endl;
        return -1;
    } else if (second_childe_pid != 0) {
            sleep(2);
            for(int i = 0; i< 15; ++i){
                if(is_msg_updated){
                    is_msg_updated = false;
                    sleep(1);
                }
                std::string msg = "parent msg " + std::to_string(i);
                std::cout << PARENT << "set '"<< msg <<"' message" <<std::endl;            
                shared_class::set_message(msg);   
                sleep(1); 
            }
            waitpid(second_childe_pid,0,0);
            waitpid(share_messages_process_pid,0,0);
        }
    return 0;
}

int run_share_messages_process(){
    char* args[] = {
        (char*)"share_messages",
        NULL
    };
    pid_t child_pid = fork();
    if (child_pid == 0){
        execv ("share_messages", args);
        std::cerr << "An error occurred in execvp"<< std::endl;
        return -1;
    }
    return child_pid;
}

int run_second_childe(){
    pid_t child_pid = fork();
    if (child_pid == 0){
        if(!shared_class::register_callback(message_handler_childe)){
            std::cout << CHILDE << "Failed to register callback, end process" <<std::endl; 
            return -1; 
        }
        std::cout << CHILDE << "callback registered" <<std::endl;
        sleep(1);
        for(int i = 0; i< 15; ++i){
            if(is_msg_updated){
                is_msg_updated = false;
                sleep(1);
            }
            std::string msg = "childe msg " + std::to_string(i);
            std::cout << CHILDE << "set '"<< msg <<"' message" <<std::endl;
            shared_class::set_message(msg);
            sleep(1);
        }
    }
    return child_pid;
}

void message_handler(std::string msg){
    std::cout << PARENT <<"message handler '" << msg << "' message receved" << std::endl;
    is_msg_updated = true;
}

void message_handler_childe(std::string msg){
    std::cout << CHILDE << "message handler '" << msg << "' message receved" << std::endl;
    is_msg_updated = true;
}