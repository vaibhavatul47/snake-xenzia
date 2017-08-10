#include <stdio.h>
#include <time.h>
#include <termios.h> 
#include <unistd.h>
#include <stdlib.h>

#define END_FILE_CHARACTER 0x04  /* ctrl-d is unix-style eof input key*/

int linux_getch(void);
char wait();
char dir = 'r';

int main()
{
	int i = 0;
	char ch = 'a';
	
	printf("Counting:\n");

	for(i = 1; i <= 1000; i++) {
		if(linux_getch()) {
			printf("Yes\n");
		}
		else {
			printf("No\n");
		}
		dir = wait();
	}
	printf("\n");

	return 0;
}

char wait()
{
	//int key = linux_getch();

	float waiting_time = 0.10;
	clock_t now = clock();
	
	while((clock() - now) < (waiting_time * CLOCKS_PER_SEC));
	fflush(stdout);

	
	return 'm';
}

int linux_getch(void) 
{
  struct termios oldstuff;
  struct termios newstuff;
  int    inch;

  tcgetattr(STDIN_FILENO, &oldstuff);
  newstuff = oldstuff;                  /* save old attributes               */
  newstuff.c_lflag &= ~(ICANON | ECHO); /* reset "canonical" and "echo" flags*/
  tcsetattr(STDIN_FILENO, TCSANOW, &newstuff); /* set new attributes         */
  inch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldstuff); /* restore old attributes     */

  if (inch == END_FILE_CHARACTER) {
    inch = EOF;
  }
  
  return inch;
}

