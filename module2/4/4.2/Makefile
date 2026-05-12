CC = gcc
CFLAGS = -Wall -Wextra
TARGET = priority_queue
SOURCES = main.c source.c

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all
