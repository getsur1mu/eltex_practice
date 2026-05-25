CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = producer consumer

all: $(TARGETS)

producer: producer.c common.h
	$(CC) $(CFLAGS) producer.c -o producer

consumer: consumer.c common.h
	$(CC) $(CFLAGS) consumer.c -o consumer

clean:
	rm -f $(TARGETS) numbers.txt numbers.txt.pos

rebuild: clean all
