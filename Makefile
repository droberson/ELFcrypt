all: ELFcrypt ELFcrypt2 example

ELFcrypt:
	gcc -Wall -o ELFcrypt ELFcrypt.c

ELFcrypt2:
	gcc -Wall -o ELFcrypt2 ELFcrypt2.c
	gcc -Wall -o ELFcrypt2-stub ELFcrypt2-stub.c

example:
	gcc -Wall -o example example.c

clean:
	rm -rf *~ *.o example ELFcrypt ELFcrypt2 ELFcrypt2-stub

