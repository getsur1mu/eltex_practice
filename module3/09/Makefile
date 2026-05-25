CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -pthread
TARGET = posix_semaphore

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET) numbers.txt 10

clean:
	rm -f $(TARGET) numbers.txt

rebuild: clean all
