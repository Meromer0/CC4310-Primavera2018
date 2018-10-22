CFLAGS = -g -O3

default: tailog

tailog.o: clase2.c
	gcc $(CFLAGS) -c clase2.c -o tailog.o


tailog: tailog.o
	gcc tailog.o -o tailog

clean: 
	-rm -f tailog.o
	-rm -f tailog