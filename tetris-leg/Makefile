CC = gcc
CFLAGS = -std=c99
SRC = tetris.c vga.c button.c ADXL345.c
TRG = main
TAGS = lintelfpgaup

all: $(TRG)

$(TRG): tetris.o vga.o button.o ADXL345.o
	$(CC) $(CFLAGS) -o $(TRG) tetris.o vga.o button.o ADXL345.o -$(TAGS) -lpthread

tetris.o: tetris.c vga.h ADXL345.h
	$(CC) $(CFLAGS) -c tetris.c -$(TAGS) -lpthread

vga.o: vga.c vga.h
	$(CC) $(CFLAGS) -c vga.c -$(TAGS) -lpthread

button.o: button.c button.h
	$(CC) $(CFLAGS) -c button.c -$(TAGS) -lpthread

ADXL345.o: ADXL345.c ADXL345.c
	$(CC) $(CFLAGS) -c ADXL345.c -$(TAGS) -lpthread

run: $(TRG)
	./$(TRG)

clean:
	rm -f $(TRG) *.o
