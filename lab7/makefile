CC = gcc
LD = gcc

CFLAGS =  -c

PROG = hexeditplus

OBJS = task1.o


default: $(PROG)

$(PROG): $(OBJS) 
	$(LD) $(OBJS)  -o $(PROG)
	rm -f *.o

task1.o: task1.c 
	$(CC) $(CFLAGS) task1.c

clean:
	rm -rf *.o *~

real_clean:
	 rm -rf *.o $(PROG) *~