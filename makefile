all	: server.out	client.out

clean	:
	rm -f *.out
server.out :	server.c
	gcc  -o server.out server.c -pthread
client.out	:	client.c
	gcc -o client.out client.c