CC = gcc
CFLAGS = -Wall -Wextra
TARGET = subnet_check
SOURCES = main.c source.c

all: $(TARGET)
	./$(TARGET) 192.168.1.1 255.255.255.0 10

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all
