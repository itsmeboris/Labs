edit : task2.o task1.o
	gcc -m64 -g -Wall -o myELF task2.o -lm
	gcc -m64 -g -Wall -o myELF1 task1.o -lm
	rm -rf *.o *~

task2.o : task2.c
	gcc -m64 -g -ansi -Wall -c -o task2.o task2.c
	
task1.o : task1.c
	gcc -m64 -g -ansi -Wall -c -o task1.o task1.c

clean : 
	rm -rf *.o *~
	
real_clean:
	 rm -rf *.o myELF *~
	 rm -rf *.o myELF1 *~