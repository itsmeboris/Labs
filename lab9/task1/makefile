edit : client.o server.o line_parser.o common.o
	gcc client.o line_parser.o common.o -o client
	gcc server.o line_parser.o common.o -o server
	rm -rf *.o *~

common.o: common.c
	gcc -c -o common.o common.c

line_parser.o: line_parser.c
	gcc -c -o line_parser.o line_parser.c

client.o : client.c
	gcc -m64 -g -ansi -Wall -c -o client.o client.c
	
server.o : server.c common.c line_parser.c
	gcc -m64 -g -ansi -Wall -c -o server.o server.c

clean : 
	rm -rf *.o *~
	
real_clean:
	 rm -rf *.o client *~
	 rm -rf *.o server *~
