#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
	while((clock() - now) < (1 * CLOCKS_PER_SEC));
	return;
}

int main()
{
	int i;
	char d;
	
	for (; d != 82; ) {
		
		printf("Looping ... \n");
		wait();
				
		if (key_stroke()) /* if any key is pressed */{
			d = scan_keyboard(); /* scan keyboard for input */
		}	
	}
	
	return 0;
}
