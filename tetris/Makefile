CC = gcc
CFLAGS = -std=c99
SRC = tetris.c vga.c button.c adxl345.c
TRG = main
TAGS = lintelfpgaup

all: $(TRG)

$(TRG): tetris.o vga.o button.o adxl345.o
	$(CC) $(CFLAGS) -o $(TRG) tetris.o vga.o button.o adxl345.o -$(TAGS) -lpthread

tetris.o: tetris.c vga.h adxl345.h
	$(CC) $(CFLAGS) -c tetris.c -$(TAGS) -lpthread

vga.o: vga.c vga.h
	$(CC) $(CFLAGS) -c vga.c -$(TAGS) -lpthread

button.o: button.c button.h
	$(CC) $(CFLAGS) -c button.c -$(TAGS) -lpthread

adxl345.o: adxl345.c adxl345.c
	$(CC) $(CFLAGS) -c adxl345.c -$(TAGS) -lpthread

run: $(TRG)
	./$(TRG)

clean:
	rm -f $(TRG) *.o
