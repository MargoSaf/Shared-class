#include <iostream>
#include "message_queue.hpp"

void* message_queue::run_thread( void *ptr ){
    message_queue* msg_q = (message_queue*)ptr;
    msg_q->broadcast_msg();
    return NULL;
}

message_queue::message_queue(){
    handlers_count = 0;
    int ret = pthread_mutex_init(&condition_mutex, NULL);
    if(ret != 0){
        std::cerr << "failed to initialize mutex " <<std::endl;
        is_obj_created = false;
        return;
    }  
    ret = pthread_cond_init(&condition_cond, NULL);
    if(ret != 0){
        std::cerr << "failed to  initialize condition variabl " <<std::endl;
        is_obj_created = false;
        pthread_mutex_destroy(&condition_mutex);
        return;
    }  


    is_obj_created = true;
}

message_queue::~message_queue(){
    is_destructor_called = true;
    pthread_cond_signal( &condition_cond );
    int ret = pthread_join(broadcast_msg_thread, NULL);
    if(ret != 0){
        std::cerr << "failed to join thread" << std::endl;
    }
    is_obj_created = false;
}

void message_queue::broadcast_msg(){
    std::string msg;
    while(!is_destructor_called){
        msg = "";
        if(pthread_mutex_lock( &condition_mutex ) == 0){
            if(msg_queue.empty())
                pthread_cond_wait( &condition_cond, &condition_mutex);
            if(!msg_queue.empty()){
                msg = msg_queue.front();
                msg_queue.pop();
            }
            pthread_mutex_unlock( &condition_mutex );
        }
        if(msg != ""){
            for(int i = 0; i< handlers_count; ++i){
                handlers_list[i](msg);
            }
        }
    }
}


bool message_queue::register_handler(void (*handler)(std::string)){
    if(handlers_count == 0){
        int ret = pthread_create( &broadcast_msg_thread, NULL, run_thread, this);
        if(ret != 0){
            std::cerr << "failed to create thread " <<std::endl;
            pthread_mutex_destroy(&condition_mutex);
            pthread_cond_destroy(&condition_cond);
            is_obj_created = false;
            return false;
        }
    }
    if(!is_obj_created)
        return false;
    if(handlers_count < HANDLERS_MAX_COUNT){
        handlers_list[handlers_count] = handler;
        handlers_count++;
        return true;
    }
    return false;
}

bool message_queue::add_message(std::string msg){
    if(!is_obj_created)
        return false;
    if(pthread_mutex_lock( &condition_mutex ) == 0){
        msg_queue.push(msg);
        pthread_cond_signal( &condition_cond );
        pthread_mutex_unlock( &condition_mutex );
        return true;
    }else
        return false;
}