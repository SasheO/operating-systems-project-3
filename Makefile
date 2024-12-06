server:  server.c list.c server_client.c
	gcc server.c server_client.c room.c list.c -lpthread -Wformat -Wall -o server