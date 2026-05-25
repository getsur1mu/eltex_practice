CC = gcc
CFLAGS = -Wall -Wextra
TARGET = signal_counter

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) counter.log

rebuild: clean all
