CC = gcc
CFLAGS = -Wall -Wextra
TARGET = tcp_select_server

all: $(TARGET)

$(TARGET): server_select.c
	$(CC) $(CFLAGS) server_select.c -o $(TARGET)

clean:
	rm -f $(TARGET) received_*

rebuild: clean all
