CC = gcc
LD = gcc

CFLAGS =  -c

PROG = wordCounter

OBJS = wordCounter.o


default: $(PROG)

$(PROG): $(OBJS) 
	$(LD) $(OBJS)  -o $(PROG)
	rm -f *.o

wordCounter.o: wordCounter.c 
	$(CC) $(CFLAGS) wordCounter.c

clean:
	rm -rf *.o *~

real_clean:
	 rm -rf *.o $(PROG) *~
