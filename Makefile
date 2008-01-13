CC = gcc
all :
	$(CC) -Wall -ansi -pedantic growl.c libgrowl.c -o growl

debug :
	$(CC) -Wall -g -ansi -pedantic growl.c libgrowl.c -o growl
	
clean :
	rm -f growl