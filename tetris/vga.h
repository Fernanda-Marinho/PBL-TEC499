#ifndef VGA_H
#define VGA_H


/* VGA Functions  */
void clearVGA();

/* Game display Functions  */
void drawMonitor(char* points, int rows, int cols, int matriz[rows][cols], int rows2, int cols2, int currentPiece, int tetrominoes[currentPiece][rows2][cols2], int currentX, int currentY, int nextPiece);
void mainWindow();

/* Game mode Functions  */
void gamePaused();
void gameOver();

#endif