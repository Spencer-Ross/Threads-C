CFLAGS=-c -std=c99 -Wall -pedantic -g
EFLAGS=-Wall -pedantic
AFLAGS=-std=c99 -Wall -pedantic -g

philo: assignment6.o
	cc -o philo $(EFLAGS) assignment6.o -lm -lpthread

assignment6.o: assignment6.c
	cc $(CFLAGS) assignment6.c

all:
	cc -o philo $(AFLAGS) assignment6.c -lm -lpthread

clean:
	rm -f assignment6.o && rm philo && rm -f -R philo.dSYM

run: philo
	./philo