CC = gcc
CFLAGS = -Wall -Wextra
TARGET = phonebook_tree
SOURCES = main.c source.c

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all
