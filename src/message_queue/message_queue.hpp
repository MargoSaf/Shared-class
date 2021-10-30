#ifndef __MESSAGE_QUEUE__
#define __MESSAGE_QUEUE__

#include <queue>
#include <string>
#include <pthread.h>

class message_queue {
    enum {HANDLERS_MAX_COUNT = 20};
    void (*handlers_list [HANDLERS_MAX_COUNT])(std::string);
    pthread_mutex_t condition_mutex;
    pthread_cond_t condition_cond;
    pthread_t broadcast_msg_thread;
    int handlers_count;
    std::queue<std::string> msg_queue;
    void broadcast_msg();
    static void* run_thread( void *ptr );
    bool is_destructor_called = false;
    bool is_obj_created = false;
public:
    message_queue();
    ~message_queue();
    bool add_message(std::string);
    bool register_handler(void (*handler)(std::string));
};


#endif // __MESSAGE_QUEUE__