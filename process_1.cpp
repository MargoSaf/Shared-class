#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "shared_class/shared_class.hpp"

bool is_msg_updated = false;

void message_handler(std::string msg){
    std::cout <<"PARENT: message handler '" << msg << "'message receved" << std::endl;
    is_msg_updated = true;
}

void message_handler_childe(std::string msg){
    std::cout <<"CHILDE: message handler " << msg << "'message receved" << std::endl;
    is_msg_updated = true;
}

int main (void) {
    if(!shared_class::register_callback(message_handler)){
        std::cerr<<"Failed to register callback, end process" <<std::endl;
        return -1;
    }
    std::cout<<"Callback registered" <<std::endl; 
    sleep(3);
    
    int pid = fork();
    if (pid == -1) {
        std::cerr<<"Fork failed" <<std::endl;
        return -1;
    } else {
        if (pid != 0) {
            for(int i = 0; i< 15; ++i){
                if(is_msg_updated){
                    is_msg_updated = false;
                    sleep(1);
                }
                std::string msg = "parent msg " + std::to_string(i);
                std::cout<<"PARENT: set '"<< msg <<"' message" <<std::endl;            
                shared_class::set_message(msg);   
                sleep(1); 
            }
            waitpid(pid,0,0);
        }else{
            if(!shared_class::register_callback(message_handler_childe)){
                std::cout<<"CHILDE: Failed to register callback, end process" <<std::endl; 
                return -1; 
            }
            std::cout<<"CHILDE: Callback registered" <<std::endl;
            for(int i = 0; i< 15; ++i){
                if(is_msg_updated){
                    is_msg_updated = false;
                    sleep(1);
                }
                std::string msg = "childe msg " + std::to_string(i);
                std::cout<<"CHILDE: set '"<< msg <<"' message" <<std::endl;
                shared_class::set_message(msg);
                sleep(1);
            }
        }
    }
    return 0;
}