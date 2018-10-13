TARGET=server
CC=gcc
CFLAGS=-I ../../include/
CRCS=$(wildcard *.c)

OBJ=$(patsubst %.c,%.o,$(CRCS))

$(TARGET):$(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS)
	
clean:   #用于清除
	rm $(OBJ)