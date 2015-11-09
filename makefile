CC=gcc
CFLAGS=-c
OFLAGS=-o
OBJ=main.o GraphLib.o Query1.o Query2.o Query3.o Query4.o Part3.o Metrics.o
SOURCE=main.c GraphLib.c Query1.c Query2.c Query3.c Query4.c Part3.c Metrics.c
EXEC=sd

$(EXEC): $(OBJ)
	$(CC) $(OFLAGS) $(EXEC) $(OBJ) -lpthread

$(OBJ): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE)

clean:
	rm -f $(OBJ) $(EXEC)
