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


/* Clears the buffers of monitor  */
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

/* Renders the main game state on the screen, including the game grid, 
the active piece, and the next piece. It also displays the player's score. 
Receives the player's score, the dimensions of the game grid (rows and columns), 
a 2D array representing the game grid, the dimensions of the tetromino pieces, 
the index of the current piece, a 3D array of all pieces, the x and y coordinates 
of the current piece on the grid, and the index of the next piece.      */ 
void drawMonitor(char* points, int rows, int cols, int matriz[rows][cols], int rows2, int cols2, int currentPiece, int tetrominoes[currentPiece][rows2][cols2], int currentX, int currentY, int nextPiece){
    
    if(video_open() == 1){
        
        video_clear();
        video_erase();
        video_box(88, 0, 208, 239, video_WHITE);
        
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                switch(matriz[i][j]){ // main matrix of the game
                    case 1: // current piece
                        video_box(
                            (j*BLOCK_SIZE+90+(j-1)), 
                            (i*BLOCK_SIZE+(i-1)), 
                            (j*BLOCK_SIZE+10+90+(j-1)), 
                            (i*BLOCK_SIZE+10+(i-1)), video_ORANGE);
                        continue;
                    
                }
            }
        }

        int pieceHeight = (currentPiece == 1) ? 2 : 4;
        for (int i = 0; i < pieceHeight; i++) {
            for (int j = 0; j < 4; j++) {
                if (tetrominoes[currentPiece][i][j]) { // renders the current piece 
                    video_box(
                            (j*BLOCK_SIZE+(BLOCK_SIZE*currentX+currentX)+90+(j-1)), 
                            (i*BLOCK_SIZE+(BLOCK_SIZE*currentY+currentY)+(i-1)), 
                            (j*BLOCK_SIZE+(BLOCK_SIZE*currentX+currentX)+10+90+(j-1)), 
                            (i*BLOCK_SIZE+(BLOCK_SIZE*currentY+currentY)+10+(i-1)), 
                            video_GREEN);
                }
            }
        }

        video_text(60, 25, "SCORE: "); //displays the score 
        video_text(70, 25, points);

        video_box(240,30,290,80, video_GREY);
        video_text(60, 5, "NEXT: ");
        for (int i = 0; i < nextPieceHeight; i++) {
            for (int j = 0; j < 4; j++) {
                if (tetrominoes[nextPiece][i][j]) { // displays the next piece 
                    video_box(
                    (j*BLOCK_SIZE+250+(j-1)), 
                    (i*BLOCK_SIZE+40+(i-1)),  
                    (j*BLOCK_SIZE+10+250+(j-1)), 
                    (i*BLOCK_SIZE+40+10+(i-1)), 
                    video_PINK); 
                }
            }
        }

        video_show();                
        video_close();

    } else {
        printf("ERROR: video not connected.\n");
    }
}

/* Displays the main welcome screen for the game. Shows text prompting 
the player to press a key to start the game.   */
void mainWindow(){
    video_open();
    video_box(80, 40, 240, 100, video_GREY);
    video_text(33, 15, "WELCOME, PLAYER!");
    video_text(30, 20, "PRESS KEY1 TO START!!!");
    video_box(80, 40, 240, 100, video_GREY); 
    video_text(33, 15, "WELCOME, PLAYER!");
    video_text(30, 20, "PRESS KEY1 TO START!!!");
    video_show();
    video_close();
}

/* Informes the player that the game is paused and providing instructions 
on how to resume.  */
void gamePaused(){
    video_open();
    video_box(75, 40, 220, 100, video_GREY); 
    video_text(33, 15, "PAUSED");
    video_text(28, 20, "press KEY0 to resume");
    video_show();
    video_close();
}

/* Appears when a collision occurs at the top, notifying the player 
that the game has ended.   */
void gameOver(){
    video_open();
    video_box(75, 40, 220, 100, video_GREY); 
    video_text(30, 15, "GAME OVER!");
    video_show();
    video_open();
    video_box(75, 40, 220, 100, video_GREY);
    video_text(30, 15, "GAME OVER!");
    video_show();
    video_close();
}


