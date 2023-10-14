simulador: simulador.o leitor_ficheiros.o
	gcc -lpthread simulador.o leitor_ficheiros.o -o simulador
	./simulador simulador_config.txt

simulador.o: simulador.c simulador.h leitor_ficheiros.h
	gcc -c simulador.c

leitor_ficheiros.0: leitor_ficheiros.c leitor_ficheiros.h
	gcc -c leitor_ficheiros.c


clean:
	rm -f *.o
