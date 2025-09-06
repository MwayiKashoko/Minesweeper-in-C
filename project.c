//Mwayi Kashoko, Fundamentals of Computing, Lab 11, project.c
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "gfx3.h"
#include "projectfunc.h"

int main() {
	srand(time(NULL));

	//Number of total rows and columns for the board
	int rows = 0;
	int cols = 0;
	int numMines = 0;
	char difficulty[9];
	
	printf("HOW TO PLAY: Start by clicking on a square on the grid, which is guaranteed to not have a mine unless you are playing on classic mode. The goal of the game is to clear every block on the screen that does not have a mine inside, and if you click on a block that has a mine, you lose. The number on a square after it is clicked indicates the number of mines directly around the square (diagonally and across). If a square has no number, that means that there are no mines surrounding that square. Press space on a square to put a flag on it. Blocks with a flag on them cannot be pressed meaning it is safe if you accidentally press it and there is a mine. Click on the smiley face for a new game. Try to go for the fastest time!\n\n");

	//It will produce a seg fault if the gfx window is unable to open
	printf("Type in the difficulty level (width x height): \nClassic 8x8 (You can lose on the first click)\nEasy 9x9 \nMedium 16x16\nHard 30x16\nCustom (any size between 2x2 and %dx%d) Note: It lags the more rows and columns there are \n\n", gfx_screenwidth()/30-2, gfx_screenheight()/30-4);
	//For if the user does not enter in a correct difficulty (line 70)
	reenterDifficulty: fgets(difficulty, 9, stdin);

	difficulty[strlen(difficulty)-1] = '\0';

	//Case insensitive
	for (int i = 0; i < strlen(difficulty); i++) {
		difficulty[i] = tolower(difficulty[i]);
	}

	//Setting up the initial conditions
	if (!strcmp(difficulty, "classic")) {
		rows = 8;
		cols = 8;
		numMines = 10;
	} else	if (!strcmp(difficulty, "easy")) {
		rows = 9;
		cols = 9; 
		numMines = 10;
	} else if (!strcmp(difficulty, "medium")) {
		rows = 16;
		cols = 16;
		numMines = 40;
	} else if (!strcmp(difficulty, "hard")) {
		rows = 16;
		cols = 30;
		numMines = 99;
	} else if (!strcmp(difficulty, "custom")) {
		printf("Since you entered custom, enter two integers. (The first integer must be between 2 and %d while the second must be between 2 and %d (inclusive)) to set up the rows and columns\n", gfx_screenwidth()/30-2, gfx_screenheight()/30-4);

		//Boundary checking
		while (rows < 2 || rows > gfx_screenheight()/30-4 || cols < 2 || cols > gfx_screenwidth()/30-2) {
			printf("Please enter the correct values: ");
			scanf("%d %d", &cols, &rows);
		}

		printf("Enter the number of mines between 1 and %d you want on the board]\n", (rows-1)*(cols-1));

		while (numMines < 1 || numMines > (rows-1)*(cols-1)) {
			printf("Please enter the correct value: ");
			scanf("%d", &numMines);
		}
	} else {
		printf("Please enter a valid difficulty\n");
		//Goes back to line 26
		goto reenterDifficulty;
	}

	int blockSize = 30;
	int width = blockSize*cols;
	//The height of the grid, not the height of the window because it also needs to display the number of mines, if the player has won or not, and the time
	int height = blockSize*rows;
	int totalBlocks = rows*cols;

	gfx_open(width, height+50, "Minesweeper");

	Block blocks[rows][cols];

	//At the end when the player presses q, the terminal will display the fastest time in which the difficulty has been completed
	int fastestTime = 999;

	//For if the user decides to reset the game by clicking the smiley face
	reset:;

	//Variables that determine whether or not the player has won or lost
	int won = 0;
	int lost = 0;

	//Displays the number of mines left in the bottom left corner. I could have made it one variable but I wanted it to more closely resemble the original version
	int minesHundreds = numMines/100;
	int minesTens = numMines/10%10;
	int minesOnes = numMines%10;
	//If the number of squares flagged is greater than the total number of mines then it will show in the bottom left corner
	int negativeMines = 0;

	//For the smiley in the middle of the screen
	int isSurprised = 0;

	//Displays the time in the bottom right corner
	int timeHundreds = 0;
	int timeTens = 0;
	int timeOnes = 0;

	//Allows me to find the time since the game has started
	time_t rawtime;
	int seconds = time(&rawtime);

	//To check if the user has pressed q or not to stop the program
	int loop = 1;

	int blocksRevealed = 0;

	//Position of the mouse on the screen
	int mouseX = 0;
	int mouseY = 0;
	
	//You cannot lose on the first click unless you are in classic mode. This is why this variable is needed. If the player clicks on the board and there is a mine there, it will move to the upper left corner. If there is a mine there, it will move one column right and so on until it gets placed.
	int firstClick = 1;

	//Mines left to place on the board.
	int minesLeftToPlace = numMines;

	setupBoard(rows, cols, blocks, blockSize, numMines);

	//Used to get user input
	char c;	

	while (loop) {
		//Draws blocks and lines separating blocks
		updateBlocks(rows, cols, blocks);
		gfx_changefont("-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso8859-1");
		drawBlocks(rows, cols, blocks, lost, won);

		color("red");
		gfx_changefont("-sony-fixed-medium-r-normal--24-170-100-100-c-120-iso8859-1");

		drawStats(width, height, minesHundreds, minesTens, minesOnes, negativeMines, isSurprised, timeHundreds, timeTens, timeOnes,  won, lost);
		
		if (!won && !lost && !firstClick) {
			updateStats(&timeHundreds, &timeTens, &timeOnes, seconds);
		}

		if (rows*cols-numMines == blocksRevealed && !lost) {
			won = 1;
			isSurprised = 0;
			
			if (100*timeHundreds + 10*timeTens + timeOnes < fastestTime) {
				fastestTime = 100*timeHundreds + 10*timeTens + timeOnes;
			}
		}

		int event = gfx_event_waiting();

		//Event handling
		if (event != 0) {
			c = gfx_wait();
			
			if (event%2 == 1 && event < 5 && gfx_ypos() < height) {
				switch (c) {
					case 1:
						if (!won && !lost) {
							mouseX = gfx_xpos();
							mouseY = gfx_ypos();
							int xPos = mouseX/blockSize;
							int yPos = mouseY/blockSize;

							isSurprised = 1;
					
							if (blocks[yPos][xPos].flagged%2) {
								//Goes to line because flagged squares cannot be clicked on
								goto next;
							}

							blocks[yPos][xPos].revealed = 1;

							if (firstClick) {
								firstClick = 0;

								if (strcmp(difficulty, "classic")) {
									//If the difficulty is not classic, and the user clicks on a square that has mines around it, this sends the mines around that block to a different location
									for (int i = -1; i < 2; i++) {
										for (int j = -1; j < 2; j++) {
											if (yPos+i < 0 || yPos+i >= rows || xPos+j < 0 || xPos+j >= cols) {
												continue;
											}

											if (blocks[yPos+i][xPos+j].hasMine && rows*cols > 9) {
												blocks[yPos+i][xPos+j].hasMine = 0;
										
												int randRow = randInt(0, rows-1);
												int randCol = randInt(0, cols-1);

												while (randRow >= yPos-1 && randRow <= yPos+1 && randCol >= xPos-1 && randCol <= xPos+1) {
													randRow = randInt(0, rows-1);
													randCol = randInt(0, cols-1);
												}

												blocks[randRow][randCol].hasMine = 1;
											}
										}
									}
								}
			
								//Counting the mines around each square after the first click
								for (int i = 0; i < rows; i++) {
									for(int j = 0; j < cols; j++) {
										for (int k = -1; k < 2; k++) {
											for (int l = -1; l < 2; l++) {
												//Checking for valid squares
												if (i+k < 0 || i+k >= rows || j+l < 0 || j+l >= cols || (k == 0 && l == 0)) {
													continue;
												}

												blocks[i][j].minesAround += blocks[i+k][j+l].hasMine;
											}
										}
									}
								}
							}  

							//If you click on a block that has a mine you lose	
							if (blocks[yPos][xPos].hasMine ) {
								lost = 1;
							}

							revealBlocks(rows, cols, blocks, &blocks[yPos][xPos]);
						}

						break;

					case 32:
						//Allows you to flag squares
						if (!won && !lost) {
							mouseX = gfx_xpos();
							mouseY = gfx_ypos();
	
							if (!blocks[mouseY/blockSize][mouseX/blockSize].revealed) {				
								blocks[mouseY/blockSize][mouseX/blockSize].flagged++;
							}

							int flags = 0;

							for (int i = 0; i < rows; i++) {
								for (int j = 0; j < cols; j++) {
									if (blocks[i][j].flagged%2 == 1) {
										flags++;
									}
								}
							}

							if (numMines-flags < 0) {
								negativeMines = 1;
							} else {
								negativeMines = 0;
							}

						minesHundreds = abs((numMines-flags)/100);
						minesTens = abs((numMines-flags)/10%10);
						minesOnes = abs((numMines-flags)%10);
					}

					break;

					case 'q':
						loop = 0;
						break;

					default:
						//Goes to line 331
						goto next;
						break;
				}
			} else if (event%2 == 1 && event < 5 && gfx_ypos() > height) {
				c = gfx_wait();

				switch (c) {
					case 1:
						mouseX = gfx_xpos();
						mouseY = gfx_ypos();

						if (mouseX >= width/2-20 && mouseX <= width/2+20 && mouseY >= height+5 && mouseY <= height+45) {
							//Goes to line 87 in order to reset the game
							goto reset;
						}

						break;

					case 'q':
						loop = 0;
						break;

					default:
						//Goes to line 331
						goto next;
						break;
				}
			}  else if (event == 5) {
				if (!won && !lost) {
					mouseX = gfx_xpos();
					mouseY = gfx_ypos();

					if (mouseY < height) {
						for (int i = 0; i < rows; i++) {
							for (int j = 0; j < cols; j++) {
								blocks[i][j].highlighted = 0;
							}
						}

						//Highlights the block the mouse is hovering over to make it easier to see which block is selected
						blocks[mouseY/blockSize][mouseX/blockSize].highlighted = 1;
					}
				}
			} else if (event == 4) {
				isSurprised = 0;
			}

			//Counting total mines revealed
			blocksRevealed = 0;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					if (blocks[i][j].revealed) {
						blocksRevealed++;
					}
				}
			}
		}
	
		//For moving onto the next iteration faster
		next:;

		gfx_flush();
		gfx_clear();

		//600 fps for classic, easy, and custom levels around the size of easy and below. 60 fps for medium, hard, and custom levels around the size of medium and up. Mainly to try and reduce lag in the lower levels. I'm not exactly sure what else I should do to decrease lag
		if (rows*cols <= 196) {
			usleep(1666.67);
		} else {
			usleep(16666.67);
		}
	}

	//Stats once the user closes the game
	if (strcmp(difficulty, "custom")) {
		printf("Fastest time on %s: %d seconds\n", difficulty, fastestTime);
	} else {
		printf("Fastest time on %s %dx%d: %d seconds\n", difficulty, cols, rows, fastestTime);
	}

	return 0;
}
