
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <curses.h>

#define true 1
#define false 0

#define BLUE_ON_CYAN 1
#define CYAN_ON_WHITE 2
#define WHITE_ON_YELLOW 3
#define WHITE_ON_BLACK 4
#define WHITE_ON_RED 5
#define WHITE_ON_GREEN 6

WINDOW *window;
int32_t windowWidth = -1;
int32_t windowHeight = -1;
int32_t blockWidth = 5;
int32_t blockHeight = 3;
int8_t boardWidth = 8;
int8_t boardHeight = 4;
int8_t *board;
int8_t currentBlock;
int8_t currentBlockWidth;
int8_t currentBlockPosX;

void clearBoard() {
    int8_t index = 0;
    while (index < boardWidth * boardHeight) {
        board[index] = 0;
        index += 1;
    }
}

void drawBackground() {
    attron(COLOR_PAIR(CYAN_ON_WHITE));
    int32_t tempPosY = 0;
    while (tempPosY < windowHeight) {
        int32_t tempPosX = 0;
        while (tempPosX < windowWidth) {
            mvaddch(tempPosY, tempPosX, ' ');
            tempPosX += 1;
        }
        tempPosY += 1;
    }
    attroff(COLOR_PAIR(CYAN_ON_WHITE));
}

void drawBlock(int32_t posX, int32_t posY, int8_t block) {
    int8_t tempColor = block + 2;
    posX += 2;
    posY += 2;
    posX *= blockWidth;
    posY *= blockHeight;
    attron(COLOR_PAIR(tempColor));
    mvprintw(posY, posX, "     ");
    refresh();
    mvprintw(posY + 1, posX, "  *  ");
    refresh();
    mvprintw(posY + 2, posX, "     ");
    refresh();
    attroff(COLOR_PAIR(tempColor));
}

void drawBoard() {
    int8_t tempPosY = 0;
    while (tempPosY <= boardHeight) {
        drawBlock(-1, tempPosY, -1);
        drawBlock(boardWidth, tempPosY, -1);
        tempPosY += 1;
    }
    int8_t tempPosX = 0;
    while (tempPosX < boardWidth) {
        drawBlock(tempPosX, -1, 0);
        drawBlock(tempPosX, boardHeight, -1);
        tempPosX += 1;
    }
    int8_t index = 0;
    tempPosX = 0;
    tempPosY = 0;
    while (tempPosY < boardHeight) {
        int8_t tempBlock = board[index];
        drawBlock(tempPosX, tempPosY, tempBlock);
        index += 1;
        tempPosX += 1;
        if (tempPosX >= boardWidth) {
            tempPosX = 0;
            tempPosY += 1;
        }
    }
}

void generateCurrentBlock() {
    currentBlock = 1 + rand() % 5;
    if (currentBlock == 5) {
        currentBlock = 1;
    }
    currentBlockWidth = 1 + rand() % 2;
    currentBlockPosX = 3;
}

void drawCurrentBlock() {
    drawBlock(currentBlockPosX, -1, currentBlock);
    if (currentBlockWidth > 1) {
        drawBlock(currentBlockPosX + 1, -1, currentBlock);
    }
}

void eraseCurrentBlock() {
    drawBlock(currentBlockPosX, -1, 0);
    if (currentBlockWidth > 1) {
        drawBlock(currentBlockPosX + 1, -1, 0);
    }
}

void drawEverything() {
    drawBackground();
    drawBoard();
    drawCurrentBlock();
}

void handleResize() {
    int32_t tempWidth;
    int32_t tempHeight;
    getmaxyx(window, tempHeight, tempWidth);
    if (tempWidth == windowWidth && tempHeight == windowHeight) {
        return;
    }
    windowWidth = tempWidth;
    windowHeight = tempHeight;
    drawEverything();
}

void moveCurrentBlock(int8_t offsetX) {
    int8_t tempPosX = currentBlockPosX + offsetX;
    if (tempPosX < 0 || tempPosX + currentBlockWidth > boardWidth) {
        return;
    }
    eraseCurrentBlock();
    currentBlockPosX = tempPosX;
    drawCurrentBlock();
}

int8_t getBlock(int8_t posX, int8_t posY) {
    return board[posX + posY * boardWidth];
}

void setBlock(int8_t posX, int8_t posY, int8_t block) {
    board[posX + posY * boardWidth] = block;
}

int8_t dropBlock() {
    int8_t tempPosY = 0;
    while (tempPosY < boardHeight) {
        int8_t tempBlock = getBlock(currentBlockPosX, tempPosY);
        if (tempBlock > 0) {
            break;
        }
        if (currentBlockWidth > 1) {
            int8_t tempBlock = getBlock(currentBlockPosX + 1, tempPosY);
            if (tempBlock > 0) {
                break;
            }
        }
        tempPosY += 1;
    }
    tempPosY -= 1;
    if (tempPosY < 0) {
        return false;
    }
    eraseCurrentBlock();
    setBlock(currentBlockPosX, tempPosY, currentBlock);
    drawBlock(currentBlockPosX, tempPosY, currentBlock);
    if (currentBlockWidth > 1) {
        setBlock(currentBlockPosX + 1, tempPosY, currentBlock);
        drawBlock(currentBlockPosX + 1, tempPosY, currentBlock);
    }
    int8_t tempStartPosX = currentBlockPosX;
    int8_t tempEndPosX = currentBlockPosX + currentBlockWidth - 1;
    while (tempStartPosX > 0) {
        int8_t tempBlock = getBlock(tempStartPosX - 1, tempPosY);
        if (tempBlock != currentBlock) {
            break;
        }
        tempStartPosX -= 1;
    }
    while (tempEndPosX < boardWidth - 1) {
        int8_t tempBlock = getBlock(tempEndPosX + 1, tempPosY);
        if (tempBlock != currentBlock) {
            break;
        }
        tempEndPosX += 1;
    }
    int8_t tempCount = tempEndPosX - tempStartPosX + 1;
    if (tempCount >= 3) {
        int8_t tempPosX = tempStartPosX;
        while (tempPosX <= tempEndPosX) {
            setBlock(tempPosX, tempPosY, 0);
            drawBlock(tempPosX, tempPosY, 0);
            tempPosX += 1;
        }
    }
    // TODO: Increase score.
    
    generateCurrentBlock();
    drawCurrentBlock();
    return true;
}

int8_t processKey() {
    int32_t tempKey = getch();
    if (tempKey == KEY_RESIZE) {
        handleResize();
    }
    if (tempKey == KEY_LEFT || tempKey == 'a') {
        moveCurrentBlock(-1);
    }
    if (tempKey == KEY_RIGHT || tempKey == 'd') {
        moveCurrentBlock(1);
    }
    if (tempKey == KEY_DOWN || tempKey == 's') {
        return dropBlock();
    }
    return true;
}

int main(int argc, const char *argv[]) {
    
    srand(time(NULL));
    
    int8_t tempBoard[boardWidth * boardHeight];
    board = tempBoard;
    clearBoard();
    generateCurrentBlock();
    
    window = initscr();
    noecho();
    curs_set(0);
    keypad(window, true);
    ESCDELAY = 50;
    start_color();
    init_pair(BLUE_ON_CYAN, COLOR_BLUE, COLOR_CYAN);
    init_pair(CYAN_ON_WHITE, COLOR_CYAN, COLOR_WHITE);
    init_pair(WHITE_ON_YELLOW, COLOR_WHITE, COLOR_YELLOW);
    init_pair(WHITE_ON_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(WHITE_ON_RED, COLOR_WHITE, COLOR_RED);
    init_pair(WHITE_ON_GREEN, COLOR_WHITE, COLOR_GREEN);
    handleResize();
    
    while (true) {
        int8_t tempResult = processKey();
        if (!tempResult) {
            break;
        }
    }
    
    endwin();
    
}