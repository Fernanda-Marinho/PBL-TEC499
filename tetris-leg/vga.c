#include <stdio.h>
#include "vga.h"
#include <intelfpgaup/video.h>


/* Defines colors for video graphics */
#define video_WHITE              0xFFFF 
#define video_YELLOW             0xFFE0
#define video_RED                0xF800
#define video_GREEN              0x07E0
#define video_BLUE               0x041F
#define video_CYAN               0x07FF
#define video_MAGENTA            0xF81F
#define video_GREY               0xC618
#define video_PINK               0xFC18
#define video_ORANGE             0xFC00

#define BLOCK_SIZE 11


//points se refere a pontuação, por isso alteracao da funcao 
void drawMonitor(char* points, int rows, int cows, int matriz[rows][cows], int rows2, int cows2, int currentPiece, int tetrominoes[currentPiece][rows2][cows2], int currentX, int currentY, int nextPiece){
    
    if(video_open() == 1){
        video_clear();
        video_erase();

        video_box(88, 0, 208, 239, video_WHITE); // desenha o retangulo branco lá
        

        // desenha a matriz principal
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cows; j++){
                switch(matriz[i][j]){
                   
                    case 1: // desenha a peça da matriz do jogo
                        video_box(
                            (j*BLOCK_SIZE+90+(j-1)), 
                            (i*BLOCK_SIZE+(i-1)), 
                            (j*BLOCK_SIZE+10+90+(j-1)), 
                            (i*BLOCK_SIZE+10+(i-1)), video_ORANGE); // cor da peca
                        continue;
                    
                }
            }
        }
        
        // O PROBLEMA ESTÁ AQUI!!!!!!
        // Desenha a peça temporariamente (antes de colidir)
        int pieceHeight = (currentPiece == 1) ? 2 : 4;
        for (int i = 0; i < pieceHeight; i++) {
            for (int j = 0; j < 4; j++) {
                if (tetrominoes[currentPiece][i][j]) {
                    video_box(
                            (j*BLOCK_SIZE+(BLOCK_SIZE*currentX+currentX)+90+(j-1)), 
                            (i*BLOCK_SIZE+(BLOCK_SIZE*currentY+currentY)+(i-1)), 
                            (j*BLOCK_SIZE+(BLOCK_SIZE*currentX+currentX)+10+90+(j-1)), 
                            (i*BLOCK_SIZE+(BLOCK_SIZE*currentY+currentY)+10+(i-1)), 
                            video_GREEN);
                }
            }
        }

        // desenha a proxima peca no tabuleiro
        video_text(60, 25, "SCORE: ");
        video_text(70, 25, points);
        video_box(240,30,290,80, video_GREY);
        int nextPieceHeight = sizeof(tetrominoes[nextPiece]) / sizeof(tetrominoes[nextPiece][0]);
        video_text(60, 5, "NEXT: ");
        for (int i = 0; i < nextPieceHeight; i++) {
            for (int j = 0; j < 4; j++) {
                if (tetrominoes[nextPiece][i][j]) {
                    video_box(
                    (j*BLOCK_SIZE+250+(j-1)), // x1
                    (i*BLOCK_SIZE+40+(i-1)),  // y1
                    (j*BLOCK_SIZE+10+250+(j-1)), // x2
                    (i*BLOCK_SIZE+40+10+(i-1)), // y2
                    video_PINK); // cor do bloco
                }
            }
        }

        
        video_show();                
        video_close();


    } else {
        printf("Video nao conectado!\n");
    }
}

void gamePaused(){
    video_open();
    video_box(75, 40, 220, 100, video_GREY); // desenha o retangulo lá
    video_text(33, 15, "PAUSED");
    video_text(28, 20, "press KEY0 to resume");
    video_show();
    video_close();
}

void gameOver(){
    video_open();
    video_box(75, 40, 220, 100, video_GREY); // desenha o retangulo lá
    video_text(30, 15, "GAME OVER!");
    video_show();
    video_open();
    video_box(75, 40, 220, 100, video_GREY); // desenha o retangulo lá
    video_text(30, 15, "GAME OVER!");
    video_show();
    video_close();
}

void mainWindow(){
    video_open();
    // clearVGA();
    // video_box(100, 10, 130, 20, video_RED);
    video_box(80, 40, 240, 100, video_GREY); // desenha o retangulo lá
    video_text(33, 15, "WELCOME, PLAYER!");
    // video_box(100, 10, 133, 20, video_RED);
    video_text(30, 20, "PRESS KEY1 TO START!!!");

    video_box(80, 40, 240, 100, video_GREY); // desenha o retangulo lá
    video_text(33, 15, "WELCOME, PLAYER!");
    video_text(30, 20, "PRESS KEY1 TO START!!!");

    video_show();
    video_close();
}

void clearVGA(){
    video_open();
    video_erase();
    video_clear();
    video_show();
    
    video_erase();
    video_clear();
    video_show();
    video_close();
}
