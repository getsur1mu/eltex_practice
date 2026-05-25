CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lrt
TARGET = posix_shared_memory

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all
