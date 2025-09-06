//Mwayi Kashoko, Fundamentals of Computing, Lab 11, projectfunc.c
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "gfx3.h"
#include "projectfunc.h"

int randInt(int min, int max) {
	return rand()%(max-min+1) + min;
}

void fillRect(int x, int y, int width, int height) {
	gfx_fill_rectangle(x, y, width, height);
}

void color(char name[]) {
	//If the string entered does not fit any of the below descriptions then the color will be yellow indicating something is wrong
	gfx_color(255, 255, 0);

	//All are standard html names for colors (besides gray1, gray2, and gray3)
	if (!strcmp(name, "gray1")) {
		gfx_color(128, 128, 128);
	} else if (!strcmp(name, "gray2")) {
		gfx_color(176, 176, 176);
	} else if (!strcmp(name, "gray3")) {
		gfx_color(224, 224, 224);
	} else if (!strcmp(name, "gray4") || !strcmp(name, "white")) {
		gfx_color(255, 255, 255);
	} else if (!strcmp(name, "blue")) { 
		gfx_color(0, 0, 255);
	} else if (!strcmp(name, "green")) { 
		gfx_color(0, 255, 0);
	} else if (!strcmp(name, "red")) { 
		gfx_color(255, 0, 0);
	} else if (!strcmp(name, "darkblue")) { 
		gfx_color(0, 0, 139);
	} else if (!strcmp(name, "maroon")) { 
		gfx_color(128, 0, 0);
	} else if (!strcmp(name, "darkturquoise")) { 
		gfx_color(0, 206, 209);
	} else if (!strcmp(name, "black")) { 
		gfx_color(0, 0, 0);
	}
}

void setupBoard(int rows, int cols, Block blocks[rows][cols], int blockSize, int totalMines) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			//Initializing
			blocks[i][j].x = j*blockSize;
			blocks[i][j].y = i*blockSize;
			blocks[i][j].row = i;
			blocks[i][j].col = j;
			blocks[i][j].width = blockSize;
			blocks[i][j].height = blockSize;
			blocks[i][j].hasMine = 0;
			blocks[i][j].highlighted = 0;
			blocks[i][j].revealed = 0;
			blocks[i][j].flagged = 0;
			blocks[i][j].minesAround = 0;
			blocks[i][j].checkedForMines = 0;
			blocks[i][j].color = 1;
		}
	}
	
	int minesPlaced = 0;
	int randRow;
	int randCol;

	//Setting mines randomly
	while (minesPlaced < totalMines) {
		randRow = randInt(0, rows-1);
		randCol = randInt(0, cols-1);

		if (!blocks[randRow][randCol].hasMine) {
			blocks[randRow][randCol].hasMine = 1;
			minesPlaced++;
		}
	}
}

void drawBlocks(int rows, int cols, Block blocks[rows][cols], int lost, int won) {
	//Draws the blocks to the screen
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			//Allows me to add string to int to set color
			char colorType[6];
			sprintf(colorType, "gray%d\0", blocks[i][j].color);

			color(colorType);

			//If you lose this game this is the block that caused you to lose
			if (blocks[i][j].revealed && blocks[i][j].hasMine) {
				color("red");
			}
			
			fillRect(j*blocks[i][j].width, i*blocks[i][j].height, blocks[i][j].width, blocks[i][j].height);

			//Different number of mines around a block means different color for each
			if (blocks[i][j].revealed && blocks[i][j].minesAround > 0) {
				sprintf(colorType, "%d\0", blocks[i][j].minesAround);

				switch (blocks[i][j].minesAround) {
					case 1:
						color("blue");
					break;

					case 2:
    				color("green");
					break;

					case 3:
   				 color("red");
					break;

					case 4:
    				color("darkblue");
					break;

					case 5:
    				color("maroon");
					break;

					case 6:
    				color("darkturquoise");
					break;

					case 7:
    				color("black");
					break;

					case 8:
   					color("gray1");
					break;
				}

				gfx_text(j*blocks[i][j].width+blocks[i][j].width/2-2, i*blocks[i][j].height+blocks[i][j].height/2+5, colorType);
			}

			//If you lose it will show all of the places where a mine was
			if (blocks[i][j].hasMine && lost) {
				color("black");
				gfx_fill_circle(blocks[i][j].x+blocks[i][j].width/2, blocks[i][j].y+blocks[i][j].height/2, 10);
			}

			//Places where a block has been flagged
			if ((blocks[i][j].flagged%2 || (won && blocks[i][j].hasMine)) && !lost) {
				color("red");
				gfx_fill_circle(blocks[i][j].x+blocks[i][j].width/2, blocks[i][j].y+blocks[i][j].height/2, 10);
			}
		}
	}

	//Draws the borders of the blocks to the screen
	color("black");
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			gfx_line(j*blocks[0][0].width, 0, j*blocks[0][0].width, rows*blocks[0][0].height);
			gfx_line(0, i*blocks[0][0].height, cols*blocks[0][0].height, i*blocks[0][0].height);
		}
	}
}

void updateBlocks(int rows, int cols, Block blocks[rows][cols]) {
	//Only updates the color
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (blocks[i][j].highlighted) {
				if (blocks[i][j].revealed) {
					blocks[i][j].color = 3;
				} else {
					blocks[i][j].color = 2;
				}
			} else {
				if (blocks[i][j].revealed) {
					blocks[i][j].color = 4;
				} else {
					blocks[i][j].color = 1;
				}
			}
		}
	}
}

void drawStats(int width, int height, int minesHundreds, int minesTens, int minesOnes, int negativeMines, int surprised, int timeHundreds, int timeTens, int timeOnes, int won, int lost) {
	//If width < 150 then I cannot fit all the stats on the screen
	if (width >= 150) {
		char mines[5];
		sprintf(mines, "%d%d%d\0", minesHundreds, minesTens, minesOnes);

		if (negativeMines) {
			sprintf(mines, "-%d%d%d\0", minesHundreds, minesTens, minesOnes);
		}

		gfx_text(6, height+33, mines);
	}

	//SMILEY
	color("gray2");
	fillRect(width/2-20, height+5, 40, 40);
	color("yellow");
	gfx_fill_circle(width/2, height+24, 15);
	color("black");
	gfx_circle(width/2, height+24, 15);

	if (!won && !lost) {
		if (surprised%2) {
			gfx_fill_circle(width/2-5, height+20, 3);
			gfx_fill_circle(width/2+5, height+20, 3);
			gfx_circle(width/2, height+30, 4);
		} else {
			gfx_fill_circle(width/2-6, height+20, 2);
			gfx_fill_circle(width/2+7, height+20, 2);
			gfx_arc(width/2-7, height+24, 15, 8, 0, -150);
		}
	} else if (won) {
			gfx_fill_circle(width/2-5, height+20, 3);
			gfx_line(width/2-5, height+17, width/2-15, height+24);

			gfx_line(width/2-5, height+17, width/2+5, height+17);

			gfx_fill_circle(width/2+5, height+20, 3);
			gfx_line(width/2+5, height+17, width/2+15, height+24);

			gfx_arc(width/2-7, height+24, 15, 8, -30, -120);
	} else if (lost) {
		gfx_line(width/2-8, height+16, width/2-4, height+20);
		gfx_line(width/2-8, height+20, width/2-4, height+16);

		gfx_line(width/2+4, height+16, width/2+8, height+20);
		gfx_line(width/2+4, height+20, width/2+8, height+16);

		gfx_arc(width/2-7, height+27, 15, 8, 30, 120);
	}

	if (width >= 150) {
		color("red");	
		char theTime[4];
		sprintf(theTime, "%d%d%d\0", timeHundreds, timeTens, timeOnes);
	
		gfx_text(width-45, height+33, theTime);
	}
}

void updateStats(int *hundreds, int *tens, int *ones, int t) {
	time_t newTime;

	//Time since the game has started
	time_t counter = time(&newTime)-t;

	//Stops at 999 like the original version	
	if (counter < 1000) {
		*ones = counter%10;
		*tens = counter/10%10;
		*hundreds = counter/100;
	}
}

void revealBlocks(int rows, int cols, Block blocks[rows][cols], Block *b) {
	//Used for base case in recursion
	b->checkedForMines = 1;

	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			//Checking for valid squares
			if (b->row+i < 0 || b->row+i >= rows || b->col+j < 0 || b->col+j >= cols) {
				continue;
			}

			//Reveals the blocks around
			if (blocks[b->row+i][b->col+j].minesAround == 0 && !blocks[b->row+i][b->col+j].hasMine && blocks[b->row+i][b->col+j].flagged%2 == 0) {
				for (int k = -1; k < 2; k++) {
					for (int l = -1; l < 2; l++) {
						if (b->row+i+k < 0 || b->row+i+k >= rows || b->col+j+l < 0 || b->col+j+l >= cols) {
								continue;
						}
					
						blocks[b->row+i+k][b->col+j+l].revealed = 1;
					}
				}
			}

			//Base case as well as recursive call
			if (blocks[b->row+i][b->col+j].minesAround == 0 && !blocks[b->row+i][b->col+j].checkedForMines && blocks[b->row+i][b->col+j].flagged%2 == 0) {
				revealBlocks(rows, cols, blocks, &blocks[b->row+i][b->col+j]);
			}
		}
	}
}
