CC = gcc
CFLAGS = -levent
target:server1_event client1_event

server1_event:server1_event.o
	gcc $^ -o $@ $(CFLAGS)
server1_event.o:server1_event.c
	gcc -c $^ -o $@ $(CFLAGS)

client1_event:client1_event.o
	gcc $^ -o $@ $(CFLAGS)
client1_event.o:client1_event.c
	gcc -c $^ -o $@ $(CFLAGS)