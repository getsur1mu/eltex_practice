CC = gcc
CFLAGS = -Wall -Wextra
TARGET = phonebook_file

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) contacts.dat

rebuild: clean all
