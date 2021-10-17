DIRS=out
all : main.cpp $(DIRS)/shared_class.o share_messages.o
	g++ -g -o main main.cpp  $(DIRS)/shared_class.o -lrt -lpthread
$(DIRS)/shared_class.o : shared_class/shared_class.cpp shared_class/shared_class.hpp
	g++ -g -c shared_class/shared_class.cpp -o $(DIRS)/shared_class.o 
share_messages.o : share_messages.cpp $(DIRS)/shared_class.o 
	g++ -g -o share_messages share_messages.cpp $(DIRS)/shared_class.o -lrt -lpthread 
clean :
	rm *.o main

.PHONY: all clean

$(shell mkdir -p $(DIRS))