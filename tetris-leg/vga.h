#ifndef VGA_H
#define VGA_H


/* VGA Functions  */
void clearVGA();

/* Game display Functions  */
void drawMonitor(char* points, int rows, int cows, int matriz[rows][cows], int rows2, int cows2, int currentPiece, int tetrominoes[currentPiece][rows2][cows2], int currentX, int currentY, int nextPiece);
void mainWindow();

/* Game mode Functions  */
void gamePaused();
void gameOver();

#endif