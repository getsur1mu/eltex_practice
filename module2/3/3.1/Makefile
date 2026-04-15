CC = gcc
CFLAGS = -Wall -Wextra
TARGET = personal_chmod
SOURCES = main.c source.c

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all