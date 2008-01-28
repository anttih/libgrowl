CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

all : md5.o libgrowl.o
	ar -r libgrowl.a md5.o libgrowl.o

libgrowl :
	$(CC) -Wall -ansi -pedantic -c -o libgrowl.o libgrowl.c

md5 :
	$(CC) -Wall -ansi -pedantic -c -o md5.o md5.c

clean :
	rm -f md5.o libgrowl.o libgrowl.a