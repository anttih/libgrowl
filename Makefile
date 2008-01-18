CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

growl : md5.o libgrowl.o growl.c

libgrowl : libgrowl.c

md5 : md5.c
	
clean :
	rm -f md5.o libgrowl.o growl