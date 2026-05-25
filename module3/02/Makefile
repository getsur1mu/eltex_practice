CC = gcc
CFLAGS = -Wall -Wextra
TARGET = mini_shell
UTILS = sum_args join_args max_value

all: $(TARGET) $(UTILS)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

sum_args: sum_args.c
	$(CC) $(CFLAGS) sum_args.c -o sum_args

join_args: join_args.c
	$(CC) $(CFLAGS) join_args.c -o join_args

max_value: max_value.c
	$(CC) $(CFLAGS) max_value.c -o max_value

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(UTILS)

rebuild: clean all
