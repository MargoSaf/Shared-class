#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <cstring>
#include "shared_class.hpp"
#include <semaphore.h>

shared_memory::shared_memory(std::string nm){
    name = nm;  
    shm_fd = -1;
    semptr = sem_open(name.c_str(), O_CREAT, 0660, 0);      
    if (semptr == NULL) 
        std::cerr<<"failed to create semaphore"<<std::endl;  
    if (sem_post(semptr) < 0) std::cout<<"failed to post semaphore"<<std::endl;
}

shared_memory::~shared_memory(){
    /*TODO:  Add some logic for call 'sem_unlink' and 'shm_unlink'
     only one there is no running process joined to shared memory*/
    shm_unlink(name.c_str());
    sem_unlink(name.c_str());
}

uint8_t shared_memory::join_shared_memory(){
    if(shm_fd > 0){
        return MEMORY_OPENED;
    }
    int join_status = MEMORY_OPENED;
    /* open shared memory if it's already created*/
    shm_fd = shm_open(name.c_str(), O_EXCL | O_RDWR, 0666);
     if(shm_fd == -1){
        /* create shared memory*/
        shm_fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        if(shm_fd == -1){
            join_status = FAILED_TO_JOIN_MEMORY;
            return join_status;
        }else{
            join_status = MEMORY_CREATED;
        }
    }
    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE);
 
    /* memory map the shared memory object */
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
     if(ptr == NULL){
        std::cerr<<"ptr to shared memory object is NULL"<<std::endl;
    }
    return join_status;
}

bool shared_memory::block_shared_memory(){
    /* wait until semaphore != 0 */
    if (!sem_wait(semptr)) {
        return true;
    }
    return false;
}

void shared_memory::unblock_shared_memory(){
     /* increment the semaphore */
     sem_post(semptr);
}

void shared_memory::shared_memory_write(const char* data, int len, int index){
    char* buff = (char*)ptr;
    /* put data into shared memory */
    memcpy(&buff[index],data,len);
}

void shared_memory::shared_memory_read(char * data, int len, int index){
    char* buff = (char*)ptr;
    /* get data from shared memory */
    memcpy(data, &buff[index], len);
}

std::string shared_class::get_message(){
    /* wait for shared memory blocking before start read */
    if(!memory.block_shared_memory())
        return NULL;
    int message_size = 0;
    memory.shared_memory_read((char*)&message_size, sizeof(message_size), MESSAGE_SIZE_INDEX);
    if(message_size <= 0)
        return NULL;
    char msg_buff[message_size + 1];
    memset(msg_buff, 0 , message_size + 1);
    memory.shared_memory_read(msg_buff, message_size, MESSAGE_START_INDEX);
    memory.unblock_shared_memory();
    return msg_buff;
}

void shared_class::set_message(std::string msg){
     /* wait for shared memory blocking before start write */
    if(!memory.block_shared_memory())
        return;
    int message_size = msg.size();
    memory.shared_memory_write((char*)&message_size, sizeof(message_size), MESSAGE_SIZE_INDEX);
    memory.shared_memory_write(msg.c_str(), message_size, MESSAGE_START_INDEX);
    notify_all(MESSAGE_CHANGED);
    memory.unblock_shared_memory();
}

void shared_class::signal_handler(int sig) {
    if (sig == MESSAGE_CHANGED)
        ptr_callback(get_message());
    signal(sig, signal_handler);
}

bool shared_class::register_process(int pid){
    /* wait for shared memory blocking before doing any changes in shared memory */
    if(!memory.block_shared_memory())
        return false;
    uint8_t join_status = memory.join_shared_memory();
    if(join_status == memory.FAILED_TO_JOIN_MEMORY){
        std::cerr << "Failed to join shared memory" << std::endl;
        return false;
    }
    uint32_t pid_count = 0;
    __pid_t pid_list[CALLBACK_COUNT_MAX];
    memset(pid_list, 0 , sizeof(pid_list));
    
    /* If the join status is 'MEMORY_OPENED' then the new process ID will be added to existing list,
     otherwise the new list will be saved*/
    if(join_status == memory.MEMORY_OPENED){
        memory.shared_memory_read((char*)&pid_count, sizeof(pid_count), PID_COUNT_INDEX);
        if(pid_count > 0)
            memory.shared_memory_read((char*)&pid_list, PID_LIST_SIZE, PID_LIST_START_INDEX);
    }

    if(pid_count == CALLBACK_COUNT_MAX){
        std::cerr << "Failed to register a callback, callbacks max count has expired" << std::endl;
    }else{
        pid_list[pid_count] = pid;
        pid_count++;
        memory.shared_memory_write((char*)&pid_count, sizeof(pid_count), PID_COUNT_INDEX);
        memory.shared_memory_write((char*)&pid_list, PID_LIST_SIZE, PID_LIST_START_INDEX);

    }
    memory.unblock_shared_memory();
    return true;
}

bool shared_class::register_callback(void (*f) (std::string)){

    if(register_process(getpid())){
        ptr_callback = f;
        signal(MESSAGE_CHANGED, signal_handler);
        return true;
    }
    return false;
}

void shared_class::notify_all(int event_type){
    int my_pid = getpid();
    uint32_t pid_count = 0;
    __pid_t pid_list[CALLBACK_COUNT_MAX];
    memory.shared_memory_read((char*)&pid_count, sizeof(pid_count), PID_COUNT_INDEX);
    if(pid_count > 0){
        memory.shared_memory_read((char*)&pid_list, PID_LIST_SIZE, PID_LIST_START_INDEX);    
        if(event_type == MESSAGE_CHANGED){
            for(int i = 0; i < pid_count; ++i){
                if(pid_list[i] != my_pid)
                    kill(pid_list[i], event_type);                
            }
        }
    }   
}

shared_memory shared_class::memory("shared_classs");

void (*shared_class::ptr_callback) (std::string) = NULL; 