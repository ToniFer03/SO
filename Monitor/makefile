monitor: monitor.o server.o
	gcc -g -lpthread monitor.o server.o -o monitor

monitor.o: monitor.c monitor.h server.h
	gcc -g -c monitor.c

server.o: server.c server.h
	gcc -g -c server.c

clean:
	rm -f *.o
