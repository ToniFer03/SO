simulador: simulador.o leitor_ficheiros.o escrita_ficheiros.o cliente.o
	gcc -g -lpthread simulador.o leitor_ficheiros.o escrita_ficheiros.o cliente.o -o simulador
	./simulador Config/simulador_config.txt
	make clean

simulador.o: simulador.c simulador.h leitor_ficheiros.h
	gcc -g -c simulador.c

leitor_ficheiros.o: leitor_ficheiros.c leitor_ficheiros.h simulador.h
	gcc -g -c leitor_ficheiros.c

escrita_ficheiros.o: escrita_ficheiros.c escrita_ficheiros.h simulador.h
	gcc -g -c escrita_ficheiros.c

cliente.o: cliente.c cliente.h escrita_ficheiros.h
	gcc -g -c cliente.c

clean:
	rm -f *.o
