/*
snake.c 4/9/2012
by Atul Vaibhav <vaibhavatul47@gmail.com>

I have checked it on ubuntu/Fedora TERMINAL. I dont think
it will run properly on other compilers like DevC, Turbo-C.

The concept of my game is that, I have started with we can say
a type of 2D array of integers with all values initialised
to 0 i.e. a value of 0 in a cell means it is an empty box, 
value of 1 means body of Snake is at that box, value of 2
means Snake's Head, and value of 4 means a bonus block.

5100
*/

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SIZE 31 // Size of Board
#define UP_SHIFT "\e[33A" // SIZE + 2
#define DOWN_SHIFT "\e[34B" // SIZE + 3
#define M (SIZE / 2)
#define UP 183
#define DOWN 184
#define RIGHT 185
#define LEFT 186

struct snake {
	int value;
	int next_x;
	int next_y;
	clock_t born; // For Bonus Boxes
};

int scan_keyboard(void);
void wait();
int key_stroke();
void move(char*);
void print();
void get_new_block(int);
void give_bonus();
void draw_maze();
void paus();

int head = (SIZE * SIZE) / 2 + 2; // 2 blocks ahead of middle-most block
int tail = (SIZE * SIZE) / 2 - 2; // 2 blocks behind of middle-most block
struct snake board[SIZE][SIZE];
int score = 0;
int level = 1;
float level_time = 0.13;
int length_snake = 0;

int main()
{
	int i = 0;
	int j = 0;
	char direction = 'g';
	int key_pressed = 0;
	char prev_direction;
	
	// Seeking rand() from system clock
	srand(time(NULL));

	// Initialising Board
	for(i = 0; i < SIZE; i++) {
		for(j = 0; j < SIZE; j++) {
			board[i][j].value = 0;
		}
	}
	
	//draw_maze(); // Draw the Maze Pattern
	
	// Initially Snake
	board[M][M + 2].value = 2; // Snake's Head
	board[M][M - 2].value = board[M][M - 1].value = board[M][M].value = board[M][M + 1].value = 1; // Snake's body
	
	// Snakes next locations
	board[M][M - 2].next_x = board[M][M - 1].next_x = board[M][M].next_x = board[M][M + 1].next_x = M;
	board[M][M - 2].next_y = M - 1;
	board[M][M - 1].next_y = M;
	board[M][M].next_y = M + 1;
	board[M][M + 1].next_y = M + 2;
	board[M][M + 2].next_x = -1; // head!
	board[M][M + 2].next_y = -1; // head!
	
	get_new_block(0); // Allot a New Block
	
	system("stty raw -echo"); // To avoid printing of arrow keys when user plays the game.
	printf("\n\n");
	//system("clear"); // Clear screen before starting game

	for (; direction != 'q'; ) {
		
		if (key_stroke()) /* if any key is pressed */{
			
			key_pressed = scan_keyboard(); /* scan keyboard for input */
			
			if(key_pressed == 27) {
				key_pressed += scan_keyboard();
				if(key_pressed == 118) {
					key_pressed += scan_keyboard();
					if(key_pressed == UP && direction != 'd')
						direction = 'u';
					else if(key_pressed == DOWN && direction != 'u')
						direction = 'd';
					else if(key_pressed == RIGHT && direction != 'l')
						direction = 'r';
					else if(key_pressed == LEFT && direction != 'r')
						direction = 'l';
				}
			}
			else if (key_pressed == 80 || key_pressed == 112) {
				print();
				paus(); // Pause The GAME
			}
			else if (key_pressed == 81 || key_pressed == 113) {
				direction = 'q';
				printf(DOWN_SHIFT); // 24 = SIZE + 3
				system("stty cooked echo"); // Resume terminal property to display keyboard keystrokes. :-)
				printf("\n\033[31mTerminated By User.\033[0m, ");
			}
		}
		move(&direction);	
		wait();
		print();
		
	}
	
	system("stty cooked echo");
	//print();
	printf(DOWN_SHIFT);
	printf("\033[33mGame Over!     \033[0m\n");
	printf("You Scored \033[32m%d\033[0m points!\n\n", score);
	
	return 0;
}

void move(char* dir)
{
	// New an old positions of Snakes Head.
	int new_head_x, new_head_y, tail_x, tail_y, old_head_x, old_head_y;
	
	new_head_x = old_head_x = head / SIZE;
	new_head_y = old_head_y = head % SIZE;
	tail_x = tail / SIZE;
	tail_y = tail % SIZE;
	

	// Now moving New Head in proper direction
	if(*dir == 'u') {
		new_head_x--;
	}
	else if(*dir == 'd') {
		new_head_x++;
	}
	else if(*dir == 'r') {
		new_head_y++;
	}
	else if(*dir == 'l') {
		new_head_y--;
	}
	else {
		return;
	}
	
	if(new_head_x < 0 || new_head_y < 0 || new_head_x >= SIZE || new_head_y >= SIZE || board[new_head_x][new_head_y].value == 1 || board[new_head_x][new_head_y].value == -1) {
		*dir = 'q';
		system("aplay -qd1 sounds/finish.wav");
		return;
	}

	if (board[new_head_x][new_head_y].value == 4) {
		system("aplay -q sounds/test.wav&");
		score *= 2; // BONUS!!!
	}

	if (board[new_head_x][new_head_y].value == 3) {
		system("aplay -q sounds/complete.wav&");
		length_snake++;
		score += 10;
		
		if(length_snake % 10 == 0) {
			level++;
			if(level_time > 0.1100) {
				level_time -= 0.05; // Insrease speed of Snake
			}
			else {
				level_time = 0.07;
			}
		}
		if(length_snake % 5 == 0) {
			system("aplay -qd2 sounds/bonus.wav&");
			get_new_block(1); // Award a Bonus Block
		}

		get_new_block(0); // Give a normal Box
	}
	else if(board[new_head_x][new_head_y].value == 0) { /* Update Tail if New head
										is a valid Empty Box */
		board[tail_x][tail_y].value = 0;
		tail = board[tail_x][tail_y].next_x * SIZE + board[tail_x][tail_y].next_y;
	}
	board[old_head_x][old_head_y].value = 1; // Specify old head value
	board[old_head_x][old_head_y].next_x = new_head_x; // Tell Head's X co-ordinate
	board[old_head_x][old_head_y].next_y = new_head_y; // Tell Head's Y Co-ordinate

	board[new_head_x][new_head_y].value = 2; // Specify Head value
	
	head = new_head_x * SIZE + new_head_y; // New Head
	//printf("\r%d - %d\n", x, y);

	return;
}

void get_new_block(int bonus)
{
	int i,j;
	
	i = (int)rand() % SIZE;
	j = (int)rand() % SIZE;

	while(board[i][j].value != 0) {
		i = (int)rand() % SIZE;
		j = (int)rand() % SIZE;
	}

	board[i][j].value = 3 + bonus;
	
	if(bonus == 1) {
		board[i][j].born = clock();
	}

	return;
}

void print()
{
	int i;
	int j;
	
	//system("clear");

	printf("\033[47m\r");
	for(i = 0; i < SIZE + 2; i++) {
		printf("  ");
	}
	printf("\033[0m");
	
	printf("\033[36m\tSCORE\033[0m: %d  \033[36mLEVEL\033[0m: %d\n\r", score, level);	
	for(i = 0; i < SIZE; i++) {
		printf("\033[47m  \033[0m");
		for(j = 0; j < SIZE; j++) {
			
			//printf(" %d", board[i][j].value);
			if(board[i][j].value == 1) { // Snake's Body
				printf("\033[42m  \033[0m");
			}
			else if(board[i][j].value == 2) { // Snake's Head
				printf("\033[41m  \033[0m");
			}
			else if(board[i][j].value == 3) { // New Block
				printf("\033[44m  \033[0m");
			}
			else if(board[i][j].value == 4) { // Bonus Block
				if(clock() - board[i][j].born < (int)(SIZE / 7) * CLOCKS_PER_SEC) {
					printf("\033[43m  \033[0m");
				}
				else {
					printf("  ");
					board[i][j].value = 0;
				}
			}
			else if(board[i][j].value == -1) { // Maze
				printf("\033[40m  \033[0m");
			}
			else {
				printf("  ");
			}
		}
		printf("\033[47m  \033[0m\n\r");
	}
	printf("\033[47m");
	for(i = 0; i < SIZE + 2; i++) {
		printf("  ");
	}
	printf("\033[0m");
	printf("\tQ: Quit  P: Pause  R: Resume");
	printf("\n\r");
	
	printf(UP_SHIFT);

	return;
}

int scan_keyboard(void) 
{
	struct termios oldstuff;
	struct termios newstuff;
	int inch;

	tcgetattr(STDIN_FILENO, &oldstuff);
	newstuff = oldstuff;                  /* save old attributes               */
	newstuff.c_lflag &= ~(ICANON | ECHO); /* reset "canonical" and "echo" flags*/
	tcsetattr(STDIN_FILENO, TCSANOW, &newstuff); /* set new attributes         */
	inch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldstuff); /* restore old attributes     */

	return inch;
}

int key_stroke()
{
	int i;
	ioctl(0, FIONREAD, &i);
	return i; /* return a count of chars available to read */
}

void wait()
{
	clock_t now = clock();
	while((clock() - now) < (level_time * CLOCKS_PER_SEC));
	return;
}

void paus()
{
	int j;
	while(1) {
		if (key_stroke()) {
			j = scan_keyboard();
			if(j == 82 || j == 114) {
				break;
			}
		}
	}

	return;
}

void draw_maze()
{
	int unit = SIZE / 10;
	int i;
	int j;
	int k;

	if(SIZE < 11) {
		return;
	}
	
	// Smaller Vertical Lines
	for(i = unit * 2 + 1; i <= unit * 8 - 1; i++) {
		j = unit;
		k = unit * 9;
		if(i < unit * 4 || i > unit * 6 + 1) {
			board[i][j].value = -1;
			board[i][k].value = -1;
		}
	}

	// Bigger Vertical Lines
	for(i = unit; i <= unit * 9; i++) {
		j = unit * 4 - 1;
		k = unit * 6 + 1;
		if(i < unit * 4 || i > unit * 6) {
			board[i][j].value = -1;
			board[i][k].value = -1;
		}
	}

	// Smaller Horizontal Lines
	for(j = unit * 2 + 1; j <= unit * 8 - 1; j++) {
		i = unit;
		k = unit * 9;
		if(j < unit * 4 || j > unit * 6 + 1) {
			board[i][j].value = -1;
			board[k][j].value = -1;
		}
	}

	// Bigger Horizontal Lines
	for(j = unit; j <= unit * 9; j++) {
		i = unit * 4 - 1;
		k = unit * 6 + 1;
		if(j < unit * 4 || j > unit * 6) {
			board[i][j].value = -1;
			board[k][j].value = -1;
		}
	}

	return;
}
