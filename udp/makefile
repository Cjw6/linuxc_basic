target:udp_socket_server  udp_socket_client

udp_socket_server:udp_socket_server.o
	gcc $^ -o $@
udp_socket_server.o:udp_socket_server.c

udp_socket_client:udp_socket_client.o
	gcc $^ -o $@
udp_socket_client.o:udp_socket_client.c