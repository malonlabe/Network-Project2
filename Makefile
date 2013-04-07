CFLAGS = -lm
default: sender.c common.h util.c router.c
	gcc -g $CFLAGS -o sender sender.c util.c 
	gcc -g $CFLAGS -o router router.c util.c
	gcc -g $CFLAGS-o receiver receiver.c util.c

clean:
	rm -f sender receiver router 
	rm -r *.dSYM