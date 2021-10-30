DIRS=out
all : main.cpp $(DIRS)/message_queue.o $(DIRS)/shared_class.o  share_messages.o
	g++ -g -o main main.cpp  $(DIRS)/shared_class.o $(DIRS)/message_queue.o -lrt -lpthread
$(DIRS)/message_queue.o : src/message_queue/message_queue.cpp src/message_queue/message_queue.hpp
	g++ -g -c src/message_queue/message_queue.cpp -o $(DIRS)/message_queue.o 
$(DIRS)/shared_class.o : src/shared_class/shared_class.cpp src/shared_class/shared_class.hpp src/message_queue/message_queue.hpp
	g++ -g -c src/shared_class/shared_class.cpp -o $(DIRS)/shared_class.o

share_messages.o : share_messages.cpp $(DIRS)/shared_class.o $(DIRS)/message_queue.o
	g++ -g -o share_messages share_messages.cpp $(DIRS)/message_queue.o $(DIRS)/shared_class.o -lrt -lpthread 
clean :
	rm *.o main

.PHONY: all clean

$(shell mkdir -p $(DIRS))