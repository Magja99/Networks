all: main.o send_receive.o update.o router.o converters.o 
	g++ main.o send_receive.o update.o router.o converters.o -o router -std=c++11 -Wall -Wextra

main.o: main.cpp router.h converters.h types.h
	g++ -c main.cpp -std=c++11 -Wall -Wextra

router.o: router.cpp router.h send_receive.h types.h
	g++ -c router.cpp -std=c++11 -Wall -Wextra

send_receive.o: send_receive.cpp send_receive.h converters.h update.h types.h
	g++ -c send_receive.cpp -std=c++11 -Wall -Wextra

update.o: update.cpp update.h converters.h types.h
	g++ -c update.cpp -std=c++11 -Wall -Wextra

converters.o: converters.cpp converters.h types.h
	g++ -c converters.cpp -std=c++11 -Wall -Wextra

clean:
	rm -f *.o

cleandist:
	rm -f *.o router
