CC = gcc
thread = -lpthread

target:epoll2_server

epoll2_server:epoll2_server.o
	$(CC) $^ -o $@
epoll2_server.o:epoll2_server.c

epoll_server:epoll_server.o
	$(CC) $^ -o $@
epoll_server.o:epoll_server.c

poll_server:poll_server.o
	$(CC) $^ -o $@ 
poll_server.o:poll_server.c

select_server:select_server.o
	$(CC) $^ -o $@ 
select_server.o:select_server.c

pthread_server:pthread_server.o
	$(CC) $^ -o $@ $(thread)
pthread_server.o:pthread_server.c
	$(CC) -c $^ -o $@ $(thread)

socket_client:socket_client.o
	$(CC) $^ -o $@ 
socket_client.o:socket_client.c

.PHNOY:clean
clean:
	rm *.o