CC=gcc -g -std=gnu11

all: clean server client

client: client.o comms.o
	$(CC) client.o comms.o -o client.out

server: server.o comms.o
	$(CC) server.o comms.o -o server.out

comms.o: comms.c
	$(CC) -c comms.c

server.o: server.c
	$(CC) -c server.c

client.o: client.c
	$(CC) -c client.c

clean:
	-rm *.out *.o
