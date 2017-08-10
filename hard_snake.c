#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SIZE 51
#define M (SIZE / 2)
#define UP 183
#define DOWN 184
#define RIGHT 185
#define LEFT 186

struct snake {
	int value;
	int next_x;
	int next_y;
};

int scan_keyboard(void);
void wait();
int key_stroke();
void move(char);
void print();
void get_new_block();
void draw_maze();

int head = (SIZE * SIZE) / 2 + 2; // 2 blocks ahead of middle-most block
int tail = (SIZE * SIZE) / 2 - 2; // 2 blocks behind of middle-most block
struct snake board[SIZE][SIZE];
int score = 0;
float level_time = 0.07;

int main()
{
	int i = 0;
	int j = 0;
	char direction = 'r';
	int key_pressed = 0;

	// Initialising Board
	for(i = 0; i < SIZE; i++) {
		for(j = 0; j < SIZE; j++) {
			board[i][j].value = 0;
		}
	}
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
	
	draw_maze(); // Draw any Maze Pattern
	get_new_block(); // Allot a New Block

	system("stty raw -echo");
	printf("enter 'q' to quit \n");
	
	for (i = 0; (direction != 'q'); i++) {
		
		wait();
		
		//new_value = 0;
		if (key_stroke()) {
			//direction = scan_keyboard();
			key_pressed = scan_keyboard();
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
			else if (key_pressed == 81 || key_pressed == 113)
				direction = 'q';
		}
		move(direction);	
	}
	
	system("stty cooked echo");
	return 0;
}

void move(char dir)
{
	int ans = 0;
	int new_head_x, new_head_y, tail_x, tail_y, old_head_x, old_head_y;
	
	new_head_x = old_head_x = head / SIZE;
	new_head_y = old_head_y = head % SIZE;
	tail_x = tail / SIZE;
	tail_y = tail % SIZE;
	
	if(dir == 'u') {
		new_head_x--;
	}
	else if(dir == 'd') {
		new_head_x++;
	}
	else if(dir == 'r') {
		new_head_y++;
	}
	else if(dir == 'l') {
		new_head_y--;
	}
	
	if(new_head_x < 0 || new_head_y < 0 || new_head_x >= SIZE || new_head_y >= SIZE || board[new_head_x][new_head_y].value == 1) {
		system("stty cooked echo");
		printf("Game Over!\n");
		printf("You Scored \033[33m%d\033[0m points!\n", score);
		exit(1);
	}
	
	if (board[new_head_x][new_head_y].value == 3) {
		get_new_block();
		score += 10;
	}
	
	board[old_head_x][old_head_y].value = 1; // Specify old head value
	board[old_head_x][old_head_y].next_x = new_head_x; // Tell Head's X co-ordinate
	board[old_head_x][old_head_y].next_y = new_head_y; // Tell Head's Y Co-ordinate

	board[new_head_x][new_head_y].value = 2; // Specify Head value
	
	head = new_head_x * SIZE + new_head_y; // New Head
	print();
	//printf("\r%d - %d\n", x, y);

	return;
}

void get_new_block()
{
	int i,j;
	
	i = (int)rand() % 51;
	j = (int)rand() % 51;

	while(board[i][j].value != 0) {
		i = (int)rand() % 51;
		j = (int)rand() % 51;
	}

	board[i][j].value = 3;
	
	return;
}

void print()
{
	int i;
	int j;
	
	system("clear");
	printf("\r");
	
	for(i = 0; i < SIZE + 2; i++) {
		printf("==");
	}
	printf("SCORE: %d\n\r", score);	
	for(i = 0; i < SIZE; i++) {
		printf("||");
		for(j = 0; j < SIZE; j++) {
			if(board[i][j].value == 1) { // Snake's Body
				printf("\033[42m  \033[0m");
			}
			else if(board[i][j].value == 2) { // Snake's Head
				printf("\033[41m  \033[0m");
			}
			else if(board[i][j].value == 3) { // New Block
				printf("\033[44m  \033[0m");
			}
			else {
				printf("  ");
			}
		}
		printf("||\n\r");
	}
	for(i = 0; i < SIZE + 2; i++) {
		printf("==");
	}
	printf("\n\r");	

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

void wait()
{
	clock_t now = clock();
	while((clock() - now) < (level_time * CLOCKS_PER_SEC));
	return;
}

int key_stroke()
{
	int i;
	ioctl(0, FIONREAD, &i);
	return i; /* return a count of chars available to read */
}

void draw_maze()
{



	return;
}
