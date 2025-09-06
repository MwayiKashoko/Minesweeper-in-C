//Mwayi Kashoko, Fundamentals of Computing, Lab 11, projectfunc.h

typedef struct {
	//x and y location on screen
	int x;
	int y;

	//row and column in the grid
	int row;
	int col;

	//width and height are 30 for all blocks
	int width;
	int height;

	//Condition that checks if a block has a mine
	int hasMine;

	//If the mouse is hovered over the block on the screen then this will be true
	int highlighted;

	//Is the block revealed
	int revealed;

	//If the user presses space then the block will be flagged. Flagged blocks cannot be clicked
	int flagged;

	//Total mines around a block
	int minesAround;

	//Used for recrusive function revealBlocks
	int checkedForMines;

	//Current color of the block
	int color;
} Block;

//Returns a random integer between the min and max (inclusive)
int randInt(int, int);

//Only used to shorten the function gfx_fill_rectangle
void fillRect(int x, int y, int width, int height);

//Enter a color as a string and this function will find it's corresponding value in rgb
void color(char []);

//Sets up the board by declaring Blocks and placing mines
void setupBoard(int rows, int cols, Block [rows][cols], int, int);

//Draws blocks to the screen
void drawBlocks(int rows, int cols, Block [rows][cols], int, int);

//Updates the blocks every frame
void updateBlocks(int rows, int cols, Block [rows][cols]);

//Draws the stats located at the bottom of the screen
void drawStats(int, int, int, int, int, int, int, int, int, int, int, int);

//Updates the time since the game has started
void updateStats(int *, int *, int *, int);

//Recursive function that reveals blocks
void revealBlocks(int rows, int cols, Block [rows][cols], Block *);
