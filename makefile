# Matt Myers cs 440


all:	prog3

prog3: main.o
	g++ -std=c++11 -g -ldl -Wall -Wextra -pedantic main.o -o prog3

main.o: main.cpp SharedPtr.hpp
	g++ -std=c++11 -c -g -ldl -Wall -Wextra -pedantic main.cpp SharedPtr.hpp

clean: 
	rm -f *.o *.gch prog3
