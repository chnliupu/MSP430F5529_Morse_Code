#include <msp430f5529.h> 
#include <readMorse.h>

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	initClock();
	initTB();
	initIO();
	volatile int *p;
	volatile int i=0;
	volatile int word[4] = {2,2,2,2};
	while(1){
		if(!(P1IN&BIT2)){
			clcLight();
			p = readKey();
			for (i=0;i<4;i++){
			word[i] = *(p+i);
			}
			showMorse(word[0],word[1],word[2],word[3]);
			while(!(P1IN&BIT2));
		}
	}
	return 0;
}
