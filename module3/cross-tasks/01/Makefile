CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = raw_server raw_client

all: $(TARGETS)

raw_server: server.c common.h
	$(CC) $(CFLAGS) server.c -o raw_server

raw_client: client.c common.h
	$(CC) $(CFLAGS) client.c -o raw_client

clean:
	rm -f $(TARGETS)

rebuild: clean all
