TARGET=main
CC=arm-linux-gcc
CFLAGS=-I ../../include/ -I ../../include/freetype2/ -L ../../lib/ -ljpeg -lfreetype -pthread
DIRS=src .
CRCS=$(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
OBJ=$(patsubst %.c,%.o, $(CRCS))

$(TARGET) : $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS)

clean:
	rm $(OBJ)