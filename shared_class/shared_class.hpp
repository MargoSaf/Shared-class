#ifndef _SHARED_CLASS_
#define _SHARED_CLASS_

#include <string>
#include <semaphore.h>

class shared_memory{
    std::string name;           /**< Name of the shared memory object */
    const int SIZE = 4096;      /**< The size (in bytes) of shared memory object */
    int shm_fd;                 /**< Shared memory file descriptor */
    void* ptr;                  /**< Pointer to shared memory object */
    sem_t* semptr;              /**< Pointer to semaphore to lock the shared mem */
 public:
    enum {MEMORY_CREATED = 0, MEMORY_OPENED = 1, FAILED_TO_JOIN_MEMORY = 2};
    shared_memory(std::string nm);
    ~shared_memory();
    uint8_t join_shared_memory();
    bool block_shared_memory();
    void unblock_shared_memory();
    void shared_memory_read(char * data, int len, int index);
    void shared_memory_write(const char * data, int len, int index);
    void unlink_shared_memory();
};

class shared_class{
    static shared_memory memory;
    static void (*ptr_callback) (std::string);
    shared_class(){}
    static void notify_all(int event_type);
    static bool register_process(int pid);
    static void *signal_handler( void *ptr );
    static    sigset_t signals;

    #define PID_LIST_SIZE           sizeof(__pid_t) * CALLBACK_COUNT_MAX
    #define PID_COUNT_INDEX         0
    #define PID_LIST_START_INDEX    PID_COUNT_INDEX + 4
    #define MESSAGE_SIZE_INDEX      PID_LIST_START_INDEX + PID_LIST_SIZE
    #define MESSAGE_START_INDEX     MESSAGE_SIZE_INDEX + 4
    #define CALLBACK_COUNT_MAX      20
    #define MESSAGE_CHANGED         SIGUSR1
public:

    static std::string get_message();
    static void set_message(std::string);
    static bool register_callback(void (*f) (std::string));
};

#endif