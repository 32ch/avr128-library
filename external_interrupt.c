/**
@file	external_interrupt.c
@brief		external interrupt source
@author	ROLAB 32nd CSJ
@version	1.1
@date 2017-12-31
*/
#include "external_interrupt.h"

static inline void (* _exti_handler_arr[8])(void);

#ifndef NULL_FUNC
//extern void _nullFunc(void){};
void (*_nullFunc)(void);
#define NULL_FUNC	(_nullFunc)
#endif

#define EXTI_MODE_BIT	2	//< 인터럽트 모드 비트 개수

void attatchInterrupt(unsigned char interrupt,void (*handler)(void), INT_MODE_t mode)
{
	unsigned char old_sreg = SREG;
	cli();
	EIMSK |= (1<<interrupt);
	if(interrupt<4){
		EICRA |= (mode)<<((interrupt)*EXTI_MODE_BIT);
		DDRD &= ~(1<<interrupt);
		/*
		if(mode == CHANGE){
		# warning "External Interrupt 0,1,2,3 cannot use CHANGE mode"
		//# error "error" : error 발생
		}
		*/
	}
	else{
		EICRB |= (mode)<<((interrupt-4)*EXTI_MODE_BIT);
		DDRE &= ~(1<<interrupt);
	}
	_exti_handler_arr[interrupt] = handler;
	//sei();
	SREG |= (old_sreg) & 0x80;
}

void detatchInterrupt(unsigned char interrupt)
{
	unsigned char old_sreg = SREG;
	cli();
	EIMSK &= ~(1<<interrupt);
	_exti_handler_arr[interrupt] = NULL_FUNC;
	//sei();
	SREG |= (old_sreg) & 0x80;
}


ISR(INT0_vect){	_exti_handler_arr[INT0]();}
ISR(INT1_vect){	_exti_handler_arr[INT1]();}
ISR(INT2_vect){	_exti_handler_arr[INT2]();}
ISR(INT3_vect){	_exti_handler_arr[INT3]();}
ISR(INT4_vect){	_exti_handler_arr[INT4]();}
ISR(INT5_vect){	_exti_handler_arr[INT5]();}
ISR(INT6_vect){	_exti_handler_arr[INT6]();}
ISR(INT7_vect){	_exti_handler_arr[INT7]();}
