CC=gcc -g

all: clean server client

client: client.o comms.o data.o
	$(CC) client.o comms.o data.o -o client.out

server: server.o comms.o data.o
	$(CC) server.o comms.o data.o -o server.out

comms.o: comms.c
	$(CC) -c comms.c

server.o: server.c
	$(CC) -c server.c

client.o: client.c
	$(CC) -c client.c

data.o: data.c
	$(CC) -c data.c

clean:
	-rm *.out *.o
