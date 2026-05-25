CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = udp_server udp_client

all: $(TARGETS)

udp_server: server.c
	$(CC) $(CFLAGS) server.c -o udp_server

udp_client: client.c
	$(CC) $(CFLAGS) client.c -o udp_client

clean:
	rm -f $(TARGETS)

rebuild: clean all
