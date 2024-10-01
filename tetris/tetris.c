#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include "vga.h"
#include <stdbool.h>
#include "button.h"
#include "adxl345.h"
#include <pthread.h>
#include <string.h>

// piece specifications
#define WIDTH 10
#define HEIGHT 20
#define TETROMINOES_QUANT 13


// game board 
int board[HEIGHT][WIDTH] = {0};

// current position of the active and next tetromino piece  
int currentX, currentY; 
int currentPiece;   
int nextPiece;     

// game state flags
bool isPaused = false; // estado de pausa do jogo
bool isStarted = false;

// score tracking
int score = 0;
char scoreStr[50] = "0";

// threads variables 
pthread_mutex_t pauseMutex;


/* Tetromino array. 
Receives an array of tetrominoes.    */ 
int tetrominoes[TETROMINOES_QUANT][4][4] = {
    { 
        {1, 1},
        {0, 0}
    },

    { 
        {1, 0},
        {1, 0}  
    },
    
    { 
        {0, 1},
        {1, 1}
    }, 
    
    { 
        {1, 0},
        {1, 1}
    },
    
    { 
        {1, 1},
        {1, 0}
    },
    
    { 
        {1, 1},
        {0, 1}
    },
    
    { 
        {1, 1},
        {1, 1}
    },
    
    { 
        {0, 0, 0},
        {1, 1, 1},
        {0, 0, 0}
    },
    
    { 
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0}
    },
    
    {
        {0, 1, 0},
        {1, 1, 1},
        {0, 0, 0}
    },
    
    {
        {0, 1, 0},
        {0, 1, 1},
        {0, 1, 0}
    },
    
    {
        {0, 1, 0},
        {1, 1, 0},
        {0, 1, 0}
    },
    
    {
        {0, 0, 0},
        {1, 1, 1},
        {0, 1, 0}
    }
};

/* Converts an integer to a string.
Receives the integer to convert and the character array (string) where the converted 
number will be stored.     */
void intToStr(int N, char *str) {
    int i = 0;
    int sign = N;

    while (N > 0) {
        str[i++] = N % 10 + '0';
      	N /= 10;
    } 

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

/* Monitors the state of a button in a continuous loop and determines the pause 
and start states of the game.      */
void* monitorButton() {
    while (1) { 
        int buttonState = buttonPressed(); // current state of the button

        pthread_mutex_lock(&pauseMutex); // lock the mutex

        if (buttonState == 1) {
            isPaused = !isPaused; // toggle the pause state
        } else if (buttonState == 2) {
            isStarted = !isStarted; // toggle the start state
        } 
        
        pthread_mutex_unlock(&pauseMutex); // unlock the mutex

        usleep(10000);
    }
    return NULL;
}


/* Gets a random piece.          */
int getRandomPiece() {
    return rand() % TETROMINOES_QUANT; // Escolhe aleatoriamente um índice entre 0 e 6
}

/* Initializes the next piece.    */
void initializeNextPiece() {
    nextPiece = getRandomPiece();
}

/* Initializes the current tetromino piece.    */
void initializeCurrentPiece() {
    currentPiece = nextPiece; // current becomes the next   
    initializeNextPiece();    // defines a new next piece
}

/* Checks for collision of a piece with the board boundaries or 
other pieces. 
Receives the x and y position os the piece (x, y).
Returns true if there is a collision; otherwise, returns false.   */
bool checkCollision(int x, int y) {
    int pieceHeight = (currentPiece == 1) ? 2 : 4;
    for (int i = 0; i < pieceHeight; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetrominoes[currentPiece][i][j] && 
                (x + j < 0 || x + j >= WIDTH || y + i >= HEIGHT || board[y + i][x + j])) {
                return true;
            }
        }
    }
    return false;
}

/* Checks if there is a full line, remove it and update the score.    */
void removeFullLines() {
    for (int i = HEIGHT - 1; i >= 0; i--) {
        bool fullLine = true;
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j] == 0) {  // found an empty cell 
                fullLine = false;
                break;
            }
        }

        if (fullLine) { //found a full line 
            
            // update score and convert in a string to display in vga monitor
            score += 100; 
            intToStr(score, scoreStr);
            
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < WIDTH; j++) {
                    board[k][j] = board[k - 1][j]; // move all lines above one row down
                }
            }

            for (int j = 0; j < WIDTH; j++) {
                board[0][j] = 0;  // clear the topmost line
            }
            i++; 
        }
    }
}

/* Places the current piece into the game board. After that, 
check and remove any completed lines.      */
void placePiece() {
    int pieceHeight = (currentPiece == 1) ? 2 : 4; 
    for (int i = 0; i < pieceHeight; i++) {
        for (int j = 0; j < 4; j++) {
            if (tetrominoes[currentPiece][i][j]) {
                board[currentY + i][currentX + j] = 1;
            }
        }
    }
    removeFullLines();
}

/* Moves the current piece based on the specified changes in x (dx) 
and y (dy) coordinates.  */
void movePiece(int dx, int dy) {
    if (!checkCollision(currentX + dx, currentY + dy)) { 
        // updated accordingly currentX and currentY
        currentX += dx;
        currentY += dy;
    } else if (dy != 0) { // collision in vertical movement
        // put the current piece and initializes the next piece
        placePiece();
        initializeCurrentPiece();

        // piece reset to the starting position in the center of the board at the top
        currentX = WIDTH / 2 - 2;
        currentY = 0;
    }
}

/* Initializes the game by placing the current piece in the center position of 
the board at the top.   */
void initGame() {
    currentX = WIDTH / 2 - 2;
    currentY = 0;
}

/* Checks if the game is over by determining if there is a collision 
at the top of the board.  */
bool isGameOver() {
    return checkCollision(currentX, currentY); // Se houver colisão no topo, o jogo acaba
}

/* Main function of the game loop.    */
void  gameLoop() {
    
    int ch;
    int speed = 500;  

    uint8_t idAccel; 
    int16_t XYZ[3];
    int16_t direction;

    clock_t lastMoveTime = clock();

    idAccel = ADXL345_ConfigureToGame(); 

    initGame();
    
    if (idAccel == 0xE5){ // correct address of adxl345
        ADXL345_Init();
        while (1) {

        buttonPressed();
        if (isGameOver()) { // check if the game has ended
            gameOver();
            usleep(3000000); 
            exit(0); 
        }

        // check the state of the game pause 
        if (!isPaused) {
            drawMonitor(scoreStr, HEIGHT, WIDTH, board, 4, 4, currentPiece, tetrominoes, currentX, currentY, nextPiece);
        
            clock_t currentTime = clock();
            
            if ((currentTime - lastMoveTime) * 10000 / CLOCKS_PER_SEC > speed) {
                movePiece(0, 1); // move the piece down
                lastMoveTime = currentTime; // update the last drop time
            }
        } else {
            gamePaused();
        }

        if (ADXL345_WasActivityUpdated()){
            ADXL345_XYZ_Read(XYZ);
            direction = movement((XYZ[0]*4)); //4 because of the mg_per_lsb

            switch (direction) { 
                case 1: 
                    if (!isPaused) movePiece(1, 0);  // move to the right 
                    break;
                case 2: 
                    if (!isPaused) movePiece(-1, 0); // move to the left
                    break;
                }
            }       
            usleep(50000); 
        }
    }
    
}


void main() {
    clearVGA();

    // declare thread and initialize the mutex 
    pthread_t buttonThread; 
    pthread_mutex_init(&pauseMutex, NULL);

    // create the button thread 
    if (pthread_create(&buttonThread, NULL, monitorButton, NULL) != 0) {
        perror("ERROR: could not create the button thread...");
        return 1;
    }

    while(true){
        // game not started yet
        if(!isStarted){
            mainWindow();
        }

        // game has started  
        else {
            initializeNextPiece();
            isStarted = false;
            gameLoop(); 
        }
    }
}
