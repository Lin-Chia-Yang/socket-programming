.PHONY: all

all: client1 client2 server

client1:
	gcc -o client1 client1.c
client2:
	gcc -o client2 client2.c
server:
	gcc -o server server.c
clean:
	rm -f client1 client2 server
