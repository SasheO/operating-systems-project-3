server:  server.c list.c server_client.c
	gcc server.c server_client.c room.c list.c connections.c -lpthread -Wformat -Wall -o server