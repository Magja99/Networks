all: main.o receiving.o sending.o viewing.o traceroute.o
	g++ main.o receiving.o sending.o viewing.o traceroute.o -o traceroute -std=c++11 -Wall -Wextra

main.o: main.cpp traceroute.h
	g++ -c main.cpp -std=c++11 -Wall -Wextra

receiving.o: receiving.cpp receiving.h
	g++ -c receiving.cpp -std=c++11 -Wall -Wextra

sending.o: sending.cpp sending.h
	g++ -c sending.cpp -std=c++11 -Wall -Wextra

viewing.o: viewing.cpp viewing.h
	g++ -c viewing.cpp -std=c++11 -Wall -Wextra

traceroute.o: traceroute.cpp traceroute.h viewing.h sending.h receiving.h
	g++ -c traceroute.cpp -std=c++11 -Wall -Wextra

clean:
	rm -f *.o

cleandist:
	rm -f *.o traceroute
