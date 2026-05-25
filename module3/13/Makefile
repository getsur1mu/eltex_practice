CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = tcp_server tcp_client

all: $(TARGETS)

tcp_server: server.c common.h
	$(CC) $(CFLAGS) server.c -o tcp_server

tcp_client: client.c common.h
	$(CC) $(CFLAGS) client.c -o tcp_client

clean:
	rm -f $(TARGETS) received_*

rebuild: clean all
