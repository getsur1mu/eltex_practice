CC = gcc
CFLAGS = -Wall -Wextra
TARGET = arguments

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET) 10 text 3.14 hello -7

clean:
	rm -f $(TARGET)

rebuild: clean all
