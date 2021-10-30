#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "src/shared_class/shared_class.hpp"

bool is_msg_updated = false;
void message_handler(std::string msg){
	std::cout <<"SHARE MESSAGES PROCESS: message handler '"<< msg << "' message receved" << std::endl;
	is_msg_updated = true;
}

int main (void) {
    if(!shared_class::register_callback(message_handler)){
        std::cerr<<"Failed to register callback, end process" <<std::endl;
        return -1;
    }
    std::cout<<"SHARE MESSAGES PROCESS: callback registered" <<std::endl; 
    sleep(3);
	int pid = getpid();
	sleep(5);
	for(int i = 0; i< 15; ++i){
		if(is_msg_updated){
			is_msg_updated = false;
			sleep(1);
		}
		std::string msg = "hello from " + std::to_string(pid) + " processs";
		std::cout<<"SHARE MESSAGES PROCESS: set' " << msg << "' message"<<std::endl;            
		shared_class::set_message(msg);    
		sleep(1);
	}
    return 0;
}