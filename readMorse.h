/*
    Created by Alex Liu
    2018-7-23
    */

#include <msp430f5529.h>

//ACLK: 32768Hz, SMCLK 1MHz, MCLK 1MHz
void initClock(void){
	UCSCTL4 |= SELA__REFOCLK | SELS__VLOCLK | SELM__VLOCLK;
	UCSCTL5 |= DIVA__2 | DIVS__1;
	UCSCTL6 &= ~XTS | ~XT1OFF;
	UCSCTL6 |= XCAP_3;
}

//TimerB is clocked by SMCLK, used for outputing sound.
void initTB(void){
	TB0CTL |= TBSSEL__SMCLK | MC__UP | TBCLR;
	TB0CCR0 = 500;
	TB0CCR6 = 0;
	TB0CCTL6 = OUTMOD_3;
}

//Buzz
void buzz(void){
    TB0CCR6 = 400;
}

void stopBuzz(void){
    TB0CCR6 = 0;
}

//Initiallize IO
void initIO(void){
    //Initiallize S1 and S2
    P1DIR &= ~(BIT2 | BIT3);
    P1REN |= BIT2 | BIT3;
    P1OUT |= BIT2 | BIT3;
    //Initiallize Buzzer
    P3DIR |= BIT6;
    P3SEL |= BIT6;
    //Initiallize L1~L6
    P8DIR |= BIT1;
    P8OUT &= ~BIT1;
    P3DIR |= BIT7 | BIT5;
    P3OUT &= ~(BIT7 | BIT5);
    P7DIR |= BIT4;
    P7OUT &= ~BIT4;
    P6DIR |= BIT3 | BIT4;
    P6OUT &= ~(BIT3 | BIT4);
}

//Turn off all lights.
void clcLight(void){
    P8OUT &= ~BIT1;
    P3OUT &= ~(BIT7 | BIT5);
    P7OUT &= ~BIT4;
    P6OUT &= ~(BIT3 | BIT4);
}

//Using LED to show Morse code
void showMorse(int a,int b,int c,int d){
    if(a==1){
        P3OUT |= BIT7;
    }
    if(b==1){
        P7OUT |= BIT4;
    }
    if(c==1){
        P6OUT |= BIT3;
    }
    if(d==1){
        P6OUT |= BIT4;
    }
}

volatile int readKey(void){
    /*
    Short - 0, Long - 1, None -2
    */
    volatile unsigned int flag[4] = {0,0,0,0};
    volatile unsigned int waitTime = 0;
    volatile unsigned key = 0;
    volatile int code[4]={2,2,2,2};
    volatile int i = 0;
    while(1){
        if(!(P1IN&BIT2)){
            waitTime = 0;
            while(!(P1IN&BIT2)){
                buzz();
                P8OUT |= BIT1;
                __delay_cycles(10000);
                flag[key] ++;
            }
            stopBuzz();
            P8OUT &= ~BIT1;
            key ++;
        }
        __delay_cycles(100);
        waitTime++;
        if((key>0 && waitTime > 8000)|(key>3)){     //Overtime break or maximum input break.
            break;
        }
    }
    for(i=0;i<key;i++){
        if(flag[i]<29){
            code[i] = 0;
        }
        else{
            code[i] = 1;
        }
    }
    return code;
}
