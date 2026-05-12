CC = gcc
CFLAGS = -Wall -Wextra
TARGET = phonebook_dynamic

all: $(TARGET)
	./$(TARGET)

$(TARGET): main.o source.o libphonebook.so
	$(CC) main.o source.o -L. -lphonebook -Wl,-rpath=. -o $(TARGET)

libphonebook.so: list.o
	$(CC) -shared list.o -o libphonebook.so

main.o: main.c source.h list.h
	$(CC) $(CFLAGS) -c main.c

source.o: source.c source.h list.h
	$(CC) $(CFLAGS) -c source.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -fPIC -c list.c

clean:
	rm -f *.o *.so $(TARGET)

rebuild: clean all
