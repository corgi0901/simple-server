TARGET = simple-server
SRC = $(shell ls *.c)
CC = gcc
CFLAGS = -O2 -g

TARGET: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm $(TARGET)
