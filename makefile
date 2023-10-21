simulador: simulador.o leitor_ficheiros.o
	gcc -g -lpthread simulador.o leitor_ficheiros.o -o simulador
	./simulador simulador_config.txt

simulador.o: simulador.c simulador.h leitor_ficheiros.h
	gcc -g -c simulador.c

leitor_ficheiros.o: leitor_ficheiros.c leitor_ficheiros.h
	gcc -g -c leitor_ficheiros.c

clean:
	rm -f *.o
