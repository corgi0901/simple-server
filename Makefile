TARGET = simple-server
SRC = $(shell ls *.c)
CC = gcc
CFLAGS = -O2 -Wall -Wextra
CFLAGS_DBG = -O0 -g -Wall -Wextra

TARGET: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

debug: $(SRC)
	$(CC) $(CFLAGS_DBG) $(SRC) -o $(TARGET)

clean:
	rm $(TARGET)
