CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lrt
TARGETS = chat_first chat_second

all: $(TARGETS)

chat_first: chat_first.c common.h
	$(CC) $(CFLAGS) chat_first.c -o chat_first $(LDFLAGS)

chat_second: chat_second.c common.h
	$(CC) $(CFLAGS) chat_second.c -o chat_second $(LDFLAGS)

clean:
	rm -f $(TARGETS)

rebuild: clean all
