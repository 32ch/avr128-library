/**
@mainpage
@brief	timer 라이브러리와 interrupt 라이브러리 동시 사용
@author	ROLAB 32nd CSJ
@date	2018-01-09
@details
0번 버튼 : 1,2,3 mode
- 1 : 조절
- 2 : 밝기
- 3 : 서보\n
2,3번 버튼은 각각 +,-
어느 모드던 동일, 가변저항 모드일 시 해제
가변저항 모드 : 0번버튼 몇초 이상 누를시 전환
*/
#include <avr/io.h>
#include "timer_simple.h"
#include "external_interrupt.h"
#define F_CPU	16000000
#include <util/delay.h>

#define LED_PORT	PORTA
#define LED_DDR		DDRA

#define sbi(PORTX, BITX)   (PORTX) |= (1<<(BITX))
#define cbi(PORTX, BITX)   (PORTX) &= ~(1<<(BITX))


#define ADC_PIN	7
unsigned int Read_ADC(unsigned char ADC_Input);
void ADC_Init(void);
unsigned int adc_read = 0;

unsigned int * degree;

unsigned int delay_time=0;
#define DELAY	2500	//누르고 있는 시간. 단위 : ms

//timer0
void count_ms(void)
{
	delay_time++;
}


//------------------------
//timer 1
void ovf_fnc(void)
{
	LED_PORT = 0xff;
}

void comp_fnc(void)
{	
	LED_PORT = 0x00;
}
//-----------------------

unsigned char operation_mode=0;
unsigned char P_B_mode=0;
#define tbi(x,n)	(x)^=(1<<(n))

int count=0;

void button1(void)
{	
	count++;
}

void button2(void)
{
	count--;
}

//---------------------------
//button 0
void removePress(void);

void onClick(void)
{
	changeTimerSet(0,PRESCALE,4);	//start
	attatchInterrupt(INT0,removePress,FALLING);
}

void removePress(void)
{
	changeTimerSet(0,PRESCALE,stop);
	if(delay_time>DELAY)	// Potentiometer mode/Button mode change
	{
		tbi(P_B_mode,0);
		if(P_B_mode)
		{
			detatchInterrupt(INT1);
			detatchInterrupt(INT2);
		}
		else
		{
			attatchInterrupt(INT1,button1, RISING);
			attatchInterrupt(INT2,button2, RISING);
		}
	}
	else	//Operation mode change
	{
		(operation_mode ==2)	?	operation_mode = 0 : operation_mode++;
		switch(operation_mode)
		{
			case 0:
			changeTimerSet(1,PRESCALE,stop);
			break;
			case 1:
			setTimerInit(1,7,prescale_8);
			setTimerInterrupt(1,INT_COMPA,comp_fnc);
			setTimerInterrupt(1,INT_OVF,ovf_fnc);
			break;
			case 2:
			changeTimerSet(1,WGM,14);
			changeTimerSet(1,OCR_CHANNEL,OCR_A|OC_CLR);
			setTimerLimit(1,40000);
			
			clearTimerInterrupt(1,INT_COMPA);
			clearTimerInterrupt(1,INT_OVF);
			break;
		}
	}
	delay_time=0;
	attatchInterrupt(INT0,onClick,RISING);
}
//---------------------------------------

void mode0(void)	///<작동 모드 0
{
	if(P_B_mode)
	{
		LED_PORT = (1<<(Read_ADC(ADC_PIN)/128));
	}
	else
	{
		if(count>7)		count=0;
		if(count<0)		count = 7;
		LED_PORT = (1<<count);
	}
}

void mode1(void)	///<작동 모드 1
{
	if(P_B_mode)
	{
		*degree = Read_ADC(ADC_PIN);
	}
	else
	{
		if(count>1023)		count=1023;
		*degree = count;
	}
}

void mode2(void)	///<작동 모드 2
{
	if(P_B_mode)
	{
		*degree=1400+Read_ADC(ADC_PIN)*3;
	}
	else
	{
		*degree = (count*3)-1400;
	}
}

int main(void)
{
	LED_DDR = 0xff;
	LED_PORT = 0x00;
	cbi(SREG,7);
	/////////ADC_INIT////////////
	ADMUX = 0x00;
	ADCSRA = 0x87;
	cbi(DDRF, ADC_PIN);
	//////////////////////////////

	attatchInterrupt(INT0,onClick,RISING);
	attatchInterrupt(INT1,button1,RISING);
	attatchInterrupt(INT2,button2,RISING);

	/**
		Timer0
		WGM : 2, CTC
		Interrupt : Compare match interrupt
		prescaler : 64
	*/
	setTimerInit(0,CTC,0);
	setTimerInterrupt(0,INT_COMP,count_ms);
	setTimerLimit(0,250);
	
	/**
		Timer1
		WGM : 14 , FastPWM_ICR
		Interrupt : A match interrupt, Overflow interrupt enable
		Clear Match
		prescaler ; 8
	*/
	
	setTimerInit(1,14,stop,OCR_A|OC_CLR);
	
	setTimerInterrupt(1,INT_COMPA,comp_fnc,&degree);
	setTimerInterrupt(1,INT_OVF,ovf_fnc);
	setTimerLimit(1,40000);	//ICR1=40000;

	struct{
		void (*fnc[3])(void);
	} _fnc;
	_fnc.fnc[0] = mode0;
	_fnc.fnc[1] = mode1;
	_fnc.fnc[2] = mode2;
	sbi(SREG,7);
	while(1)
	{
		_fnc.fnc[operation_mode]();
	}
}


unsigned int Read_ADC(unsigned char ADC_Input)
{
   ADMUX = 0x40|ADC_Input;
   ADCSRA |=0x40;
   while((ADCSRA & 0x10)==0);
   return ADC;
}
