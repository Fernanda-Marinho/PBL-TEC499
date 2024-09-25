#ifndef VGA_H
#define VGA_H

void drawMonitor(int rows, int cows, int matriz[rows][cows], int rows2, int cows2, int currentPiece, int tetrominoes[currentPiece][rows2][cows2], int currentX, int currentY, int nextPiece); // desenha o jogo no monitor
void clearVGA(); // limpa os buffers do monitor
void gamePaused();
void gameOver();
void mainWindow();

#endif