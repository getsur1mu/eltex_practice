CC = gcc
CFLAGS = -Wall -Wextra
TARGET = udp_sniffer

all: $(TARGET)

$(TARGET): sniffer.c
	$(CC) $(CFLAGS) sniffer.c -o $(TARGET)

clean:
	rm -f $(TARGET) udp_dump.txt

rebuild: clean all
