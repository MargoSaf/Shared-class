DIRS=out
all : process_1.cpp $(DIRS)/shared_class.o process_2.o
	g++ -g -o process_1 process_1.cpp  $(DIRS)/shared_class.o -lrt -lpthread
$(DIRS)/shared_class.o : shared_class/shared_class.cpp shared_class/shared_class.hpp
	g++ -g -c shared_class/shared_class.cpp -o $(DIRS)/shared_class.o 
process_2.o : process_2.cpp $(DIRS)/shared_class.o 
	g++ -g -o process_2 process_2.cpp $(DIRS)/shared_class.o -lrt -lpthread 
clean :
	rm *.o process_1

.PHONY: all clean

$(shell mkdir -p $(DIRS))