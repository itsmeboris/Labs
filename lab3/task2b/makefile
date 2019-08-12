CC = gcc
LD = gcc

CFLAGS =  -c

PROG = task2b

OBJS = toy_printf.o main.o


default: $(PROG)

$(PROG): $(OBJS) 
	$(LD) $(OBJS)  -o $(PROG)
	rm -f *.o

toy_printf.o: toy_printf.c 
	$(CC) $(CFLAGS) toy_printf.c
	
main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o *~

real_clean:
	 rm -rf *.o $(PROG) *~
 
