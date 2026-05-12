CC = gcc
CFLAGS = -Wall -Wextra
TARGET = calculator_dynamic
PLUGINS = plugins/add.so plugins/sub.so plugins/mul.so plugins/div.so plugins/exp.so

all: $(TARGET) $(PLUGINS)
	./$(TARGET)

$(TARGET): main.o source.o
	$(CC) main.o source.o -ldl -o $(TARGET)

main.o: main.c source.h operation.h
	$(CC) $(CFLAGS) -c main.c

source.o: source.c source.h operation.h
	$(CC) $(CFLAGS) -c source.c

plugins/%.so: plugins/%.c operation.h
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

clean:
	rm -f *.o $(TARGET) plugins/*.so

rebuild: clean all
