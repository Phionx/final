all: server client

server: server.c
	gcc server.c -o server.out

client: client.c
	gcc client.c -o server.out

clean:
	-rm *.out *.o
