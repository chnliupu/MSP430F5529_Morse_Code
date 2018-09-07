/****************************ͷ�ļ�����************************************/
#include <msp430f5529.h>
#include <driverlib.h>

/****************************Ħ��˹�������*********************************/
/**
 * @brief �Զ���Ħ��˹������ձ�����
 * a:12222 b:100022 c:101022 d:100222 e:22222 f:1022 g:110222 h:22 i:2222 j:11122 k:101222 l:10022 m:112222
 * n:102222 o:111222 p:11022 q:110122 r:10222 s:222 t:122222 u:1222 v:122 w:11222 x:100122 y:101122 z:110022
 * 1:11112 2:1112 3:112 4:12 5:2 6:100002 7:110002 8:111002 9:111102 0:111112 ?:001100 /:100102 ():101101 -:100001 .:10101
 */
#define DICTSIZE 41 //26+10+5
const unsigned char dict[41]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                        '1','2','3','4','5','6','7','8','9','0',
                        '?','/','()','-','.'};
const long MorseCode[41]={12222,100022,101022,100222,22222,1022,110222,22,2222,11122,101222,10022,112222,102222,111222,11022,110122,10222,222,122222,1222,122,11222,100122,101122,110022,
                   11112,1112,112,12,2,100002,110002,111002,111102,111112,
                   1100,100102,101101,100001,10101};


/************************ʱ��Դ��ʼ������********************************/
/**
 * @brief ����Ч����ACLK: 32768Hz, SMCLK 1MHz, MCLK 1MHz
 */
void initClock(void){
    //Selects the ACLK source:REFOCLK;SMCLK source:VLOCLK;MCLK source:VLOCLK
    UCSCTL4 |= SELA__REFOCLK | SELS__VLOCLK | SELM__VLOCLK;
    //ACLK��DCOCLKDIV��2��Ƶ�õ���SMCLK��VLOCLK��1��Ƶ�õ�
    UCSCTL5 |= DIVA__2 | DIVS__1;
    //Open XT1
    //XT1 mode:Low-frequency mode. XCAP bits define the capacitance at the XIN and XOUT pins.
    UCSCTL6 &= ~XTS | ~XT1OFF;
    //Oscillator capacitor selection.
    UCSCTL6 |= XCAP_3;
}

/************************��ʱ����ʼ������********************************/
/**
 * @brief TimerB is clocked by SMCLK, used for output sound.
 */
void initTB(void){

    //MC__UP:Up mode: Timer counts up to TBxCCR0
    //TBSSEL_1:Timer_B clock source:SMCLK
    //TBCLR:Timer_B clear
	TB0CTL |= TBSSEL__SMCLK | MC__UP | TBCLR;
	TB0CCR0 = 500;
	TB0CCR6 = 0;
	TB0CCTL6 = OUTMOD_3; //Output mode: Set/reset
}

/************************�������򿪺͹رպ���******************************/
void buzz(void){
    TB0CCR6 = 400;
}

void stopBuzz(void){
    TB0CCR6 = 0;
}

/*************************IO��ʼ������*********************************/
void initIO(void){
    //Initialize S1 and S2
    P1DIR &= ~(BIT2 | BIT3);
    P1REN |= BIT2 | BIT3;
    P1OUT |= BIT2 | BIT3;
    //Initialize Buzzer
    P3DIR |= BIT6;
    P3SEL |= BIT6;
    //Initialize L1~L6
    P8DIR |= BIT1;
    P8OUT &= ~BIT1;
    P3DIR |= BIT7 | BIT5;
    P3OUT &= ~(BIT7 | BIT5);
    P7DIR |= BIT4;
    P7OUT &= ~BIT4;
    P6DIR |= BIT3 | BIT4;
    P6OUT &= ~(BIT3 | BIT4);
}

/*************************LED��ָʾ�رպ���*********************************/
void clcLight(void){
    P8OUT &= ~BIT1;
    P3OUT &= ~(BIT7 | BIT5);
    P7OUT &= ~BIT4;
    P6OUT &= ~(BIT3 | BIT4);
}

/*************************LED��ָʾ����*********************************/
/**
 * @brief Using LED to show Morse code.
 * ���ݴ���Ľ���������code/word�����źš�ડ�(Da)���ڵ�λ������Ӧ��LED�ƣ�������Ӧ��LED�ƹر�
 * @param a ���β���1
 * @param b ���β���2
 * @param c ���β���3
 * @param d ���β���4
 * @param e ���β���5
 * @param f ���β���6
 */
void showMorse(int a,int b,int c,int d,int e,int f){
    if(a==1){
        P8OUT |= BIT1;
    }
    if(b==1){
        P3OUT |= BIT7;
    }
    if(c==1){
        P7OUT |= BIT4;
    }
    if(d==1){
        P6OUT |= BIT3;
    }
    if(e==1){
        P6OUT |= BIT4;
    }
    if(f==1){
        P3OUT |= BIT5;
    }
}

/*************************Ħ��˹�������뺯��*********************************/
/**
 * @brief ͨ���û����̻��������źŲɼ����жϡ��Ρ���ડ��źţ������н������������Ӧ�Ľ�������code
 * ���հ���ʱ�����ࣺShort - 0, Long - 1, None - 2
 * @return code[] ���ν�������
 */
volatile int readKey(void){

    //�����������־λ�ͽ�������
    volatile unsigned int flag[6] = {0,0,0,0,0,0};
    volatile unsigned int waitTime = 0;
    volatile unsigned key = 0;
    volatile int code[6]={2,2,2,2,2,2};
    volatile int i = 0;

    while(1){
        if(!(P1IN&BIT2)){
            waitTime = 0;
            while(!(P1IN&BIT2)){
                buzz();
                P8OUT |= BIT1;
                __delay_cycles(10000);
                flag[key] ++;
            }//while(!(P1IN&BIT2)
            stopBuzz();
            P8OUT &= ~BIT1;
            key ++;
        }
        __delay_cycles(100);
        waitTime++;

        //��ʱ���ߴﵽMorse code�����λ��ʱbreak
        if((key>0 && waitTime > 8000)|(key>5)){
            break;
        }
    }//while(1)

    //���ݰ���ʱ�����ɶ�Ӧ��������
    for(i=0;i<key;i++){
        if(flag[i]<29){
            code[i] = 0;
        }
        else{
            code[i] = 1;
        }
    }//for
    return code;
}


/*************************������غ궨��*********************************/
#define BAUD_RATE                               9600
//#define RECEIVE_DATA_COUNT                      0x06
#define RECEIVE_DATA_COUNT                      0x02
#define USCI_A_UART_MULTIPROCESSOR_MODE_ADDRESS        0xAA

/*************************���ڳ�ʼ������*********************************/
void initUart()
{

    //    //P3.3,4 = USCI_A0 TXD/RXD
    //    GPIO_setAsPeripheralModuleFunctionInputPin(
    //        GPIO_PORT_P3,
    //        GPIO_PIN3 + GPIO_PIN4
    //        );

    //P4.4,5 = USCI_A1 TXD/RXD��ʹ�ÿ������Դ��Ĵ��ڣ����ڰ��������ԭ���ڴ��ڵ��������л�����������ڣ�����һ������ʾ���
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P4,
        GPIO_PIN4 + GPIO_PIN5
        );

    //Initialize USCI UART
    //Baudrate = 9600, clock freq = 1.048MHz
    //UCBRx = 109, UCBRFx = 0, UCBRSx = 2, UCOS16 = 0
    USCI_A_UART_initParam param = {0};
    param.selectClockSource = USCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 109;
    param.firstModReg = 0;
    param.secondModReg = 2;
    param.parity = USCI_A_UART_NO_PARITY;
    param.msborLsbFirst = USCI_A_UART_LSB_FIRST;
    param.numberofStopBits = USCI_A_UART_ONE_STOP_BIT;
    param.uartMode = USCI_A_UART_IDLE_LINE_MULTI_PROCESSOR_MODE;
    param.overSampling = USCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;
//    if ( STATUS_FAIL == USCI_A_UART_init(USCI_A0_BASE, &param)){
    if ( STATUS_FAIL == USCI_A_UART_init(USCI_A1_BASE, &param)){
        return;
    }

    //Enable UART module for operation
    //USCI_A_UART_enable(USCI_A0_BASE);
    USCI_A_UART_enable(USCI_A1_BASE);

    //USCI_A_UART_transmitData(USCI_A0_BASE,0x40);


//    //Set USCI UART in sleep mode
//    USCI_A_UART_setDormant(USCI_A0_BASE);
//
//    //Enable Receive Interrupt
//    USCI_A_UART_clearInterrupt(USCI_A0_BASE,
//        USCI_A_UART_RECEIVE_INTERRUPT
//        );
//    USCI_A_UART_enableInterrupt(USCI_A0_BASE,
//        USCI_A_UART_RECEIVE_INTERRUPT
//        );

    //Set USCI UART in sleep mode
    USCI_A_UART_setDormant(USCI_A1_BASE);

    //Enable Receive Interrupt
    USCI_A_UART_clearInterrupt(USCI_A1_BASE,
        USCI_A_UART_RECEIVE_INTERRUPT
        );
    USCI_A_UART_enableInterrupt(USCI_A1_BASE,
        USCI_A_UART_RECEIVE_INTERRUPT
        );


}

/*************************��ʱ����**************************************/
void delay(unsigned int time)
{
    unsigned int i,j;
    for(i=0;i<255;i++)
        for(j=0;j<time;j++)
            _nop();
}

/*************************�������ݷ���**************************************/
void send_buf(unsigned char *ptr)
{
    while(*ptr!='\0')
    {
        while(!(UCA1IFG&UCTXIFG));
        //UCA0TXBUF=*ptr;
        UCA1TXBUF=*ptr;
        *ptr++;
        delay(50);
    }
}

/*************************Ħ��˹������뺯��*********************************/
unsigned char decode(int *code)
{
    long temp=0;
    char i=0;
    temp=code[0]*100000+code[1]*10000+code[2]*1000+code[3]*100+code[4]*10+code[5];
    _nop();
    for(i=0;i<DICTSIZE;i++)
    {
        if(MorseCode[i]==temp)
            return dict[i];
    }
    return 0;
}


