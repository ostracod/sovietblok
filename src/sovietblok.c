
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <curses.h>

#define true 1
#define false 0

#define BLACK_ON_CYAN 1

WINDOW *window;
int32_t windowWidth = -1;
int32_t windowHeight = -1;

void drawBackground() {
    attron(COLOR_PAIR(BLACK_ON_CYAN));
    int32_t tempPosY = 0;
    while (tempPosY < windowHeight) {
        int32_t tempPosX = 0;
        while (tempPosX < windowWidth) {
            mvaddch(tempPosY, tempPosX, ' ');
            tempPosX += 1;
        }
        tempPosY += 1;
    }
    attroff(COLOR_PAIR(BLACK_ON_CYAN));
}

void drawEverything() {
    drawBackground();
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

void handleResize();
int8_t getTestKey();

int8_t processKey() {
    int32_t tempKey = getch();
    if (tempKey == KEY_RESIZE) {
        handleResize();
    }
    if (tempKey == KEY_LEFT || tempKey == 'a') {
        
    }
    if (tempKey == KEY_RIGHT || tempKey == 'd') {
        
    }
    if (tempKey == KEY_DOWN || tempKey == 's') {
        
    }
    return true;
}

int main(int argc, const char *argv[]) {
    
    srand(time(NULL));
    
    window = initscr();
    noecho();
    curs_set(0);
    keypad(window, true);
    ESCDELAY = 50;
    start_color();
    init_pair(BLACK_ON_CYAN, COLOR_BLACK, COLOR_CYAN);
    handleResize();
    
    while (true) {
        int8_t tempResult = processKey();
        if (!tempResult) {
            break;
        }
    }
    
    endwin();
    
}