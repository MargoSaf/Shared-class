# Shared-class

# Description

Shared class

Implement a class which can be shared between processes with the following interface:

    class Shared {

    public:

    std::string GetMessage();

    void SetMessage();

    void RegisterCallback(void (*f) (std::string) );

    } ;

Callback functions must be called in all the interedted processes if the message gets set in any other process

# Known issues

* Some processes may lose messages if messages have changed very intensively.
* When one of the joined processes ended the new process can't join to memory (when the program terminates shared memory segment removed, by a destructor), the processes that joined to memory before that can use it as usual. 
