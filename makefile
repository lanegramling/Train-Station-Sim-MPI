trains: main.o
	g++ -std=c++11 -lpthread -g -Wall main.o -o trains
main.o: main.cpp Train.h Barrier.h 
	g++ -std=c++11 -lpthread -g -Wall -c main.cpp
clean:
	rm *.o trains
