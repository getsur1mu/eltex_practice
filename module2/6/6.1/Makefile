CC = gcc
CFLAGS = -Wall -Wextra
TARGET = phonebook_static

all: $(TARGET)
	./$(TARGET)

$(TARGET): main.o source.o libphonebook.a
	$(CC) main.o source.o -L. -lphonebook -o $(TARGET)

libphonebook.a: list.o
	ar rcs libphonebook.a list.o

main.o: main.c source.h list.h
	$(CC) $(CFLAGS) -c main.c

source.o: source.c source.h list.h
	$(CC) $(CFLAGS) -c source.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

clean:
	rm -f *.o *.a $(TARGET)

rebuild: clean all
