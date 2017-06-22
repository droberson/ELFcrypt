all: ELFcrypt example

ELFcrypt:
	gcc -Wall -o ELFcrypt ELFcrypt.c

example:
	gcc -Wall -o example example.c

clean:
	rm -rf *~ *.o example ELFcrypt

