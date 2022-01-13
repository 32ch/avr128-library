/**
 * @file	timer_simple.h
 * @version	1.1
 * @date	Created: 2017-12-05 오전 1:07:47
 * @auther 	Author: ch
 */ 


#ifndef __TIMER_SIMPLE_H__
#define __TIMER_SIMPLE_H__



#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////
/*		Waveform Genenration Mode			*/
#define Normal 0
typedef enum _WGM8_t {PhaseCorrect=1, CTC, FastPWM} WGM8_t;
	
typedef enum _WGM16_t {
		PhaseCorrect_8=1, PhaseCorrect_9, PhaseCorrect_10, CTC_OCR, FastPWM_8, FastPWM_9, FastPWM_10,
		Phase_FrequencyCorrect_ICR, Phase_FrequencyCorrect_OCR, PhaseCorrect_ICR, PhaseCorrect_OCR, CTC_ICR, Reserved,FastPWM_ICR, FastPWM_OCR
	} WGM16_t;

/////////////////////////////////////////////////////////////////////////


typedef enum {
	stop, prescale_1, prescale_8, prescale_64, prescale_256, prescale_1024, Exclk_falling, Exclk_rising
} CS_t;


///////////////////////////////////////////////////////
/*				register							*/
// 선언 순서대로 하위비트부터 채워짐
typedef struct {
	uint8_t _WGML :2 ;	//Waveform  Generation Mode bit low
	uint8_t _COMC :2 ;	//Compare Output ModeC
	uint8_t _COMB :2 ;	//Compare Output ModeB
	uint8_t _COMA :2 ;	//Compare Output ModeA
}_TCCRA;

typedef struct {
	uint8_t _CS		:3 ;	//Clock Select
	uint8_t _WGMH	:2 ;	//Waveform  Generation Mode bit High
	uint8_t _r		:1 ;	//reserved
	uint8_t _ICES	:1 ;	//Input Capture Edge Select
	uint8_t _ICNC	:1 ;	//Input Capture Noise Canceler
	} _TCCRB;
	
typedef struct  {
	uint8_t _CS		:3 ;	//Clock Select
	uint8_t _WGMH	:1 ;	//Waveform Generation Mode bit High
	uint8_t _COM	:2 ;	//Compare Output Mode
	uint8_t	_WGML	:1 ;	//Waveform Generation Mode bit Low
	uint8_t	_FOC	:1 ;	//Force Output Compare
	}_TCCR8_t;

typedef struct 
{
	_TCCRB b;
	_TCCRA a;
}_TCCR16_t;


#define _TIMER0	0x53	//TCCR0
#define _TIMER2	0x45	//TCCR2
#define _TIMER1 0x4E	//TCCR1B
#define _TIMER3	0x8A	//TCCR3B
	

#ifndef _REG16(X)
typedef union
{
	struct
	{
		uint8_t _L;
		uint8_t _H;
	}_8REG;
	uint16_t _val;
} _16REG_t;
#define _REG16(X)	(*(volatile _16REG_t *)(X))
#endif

//////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
/*						TCCR Setting					*/
////////////////////////////////////////////////////////////

//-----------------------------------------------------------
/*	output mode		*/
#define OC_DISABLE	0
#define OC_TOGGLE	1
#define OC_CLR		2
#define OC_SET		3
/*	channel		*/
#define OCR_A		4
#define OCR_B		8
#define OCR_C		12

void setTimerInit(unsigned char timer, unsigned char wgm, unsigned char prescale, ...);	// 예전이름 : setSimpleTimer
//-------------------------------------------------------------

///---------------------------------------------------------------
#define WGM			1	///<Waveform Generation Mode
#define OCR_CHANNEL	2	///<Compare channel
#define PRESCALE	3	///<prescale

void changeTimerSet(unsigned char timer, unsigned char what,...);
///----------------------------------------------------------------

//////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
//			Interrupt Setting
//---------------------------------------------------------
#define INT_OVF		0	///<	overflow interrupt
#define INT_COMP	1	///<	compare match interrupt(8bit-timer0,2)
#define INT_COMPA	2	///<	compare match interrupt(16bit-timer1,3). A channel
#define INT_COMPB	1	///<	compare match interrupt(16bit-timer1,3). B channel
#define INT_COMPC	4	///<	compare match interrupt(16bit-timer1,3). C channel
#define INT_CAPT	3	///<	input capture interrupt(16bit-timer1,3).


void setTimerInterrupt(unsigned char timer, unsigned char interrupt_type, void (* handler)(void),...);
//void setAltTimerInterrupt(uint8_t timer, uint8_t , void (*handler)(void), uint16_t start, uint16_t)
void clearTimerInterrupt(unsigned char timer, unsigned char interrupt_type);
/////////////////////////////////////////////////////////////////////////////////////

char setTimerLimit(unsigned char timer, unsigned int val);	//예전이름 : setMaxTimer
unsigned int getLimitInfo(unsigned char timer);	//예전이름 : getMaxTimer

void setCompValue(unsigned char timer, unsigned char channel, void * value);
#define changeCompValue(ptr, val)		*(ptr) = val

/*
#ifndef NULL_FUNC
#include "external_interrupt.h"
#endif
*/
#endif /* __TIMER_SIMPLE_H_ */
