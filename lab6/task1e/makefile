all: myshell

myshell: line_parser.o job_control.o myshell.o
	gcc myshell.o job_control.o line_parser.o -o myshell
	rm -rf *.o *~

line_parser.o: line_parser.c
	gcc -c -g -Wall -Werror -o line_parser.o line_parser.c
	
job_control.o: job_control.c
	gcc -c -g -Wall -Werror -o job_control.o job_control.c

myshell.o: myshell.c
	gcc -c -g -Wall -Werror -o myshell.o myshell.c

clean:
	rm -rf *.o *~

real_clean:
	 rm -rf *.o myshell *~
