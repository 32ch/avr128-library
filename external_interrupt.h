/**
@file	external_interrupt.h
@brief		external interrupt header
@details	아두이노 외부 인터럽트 함수와 거의 동일
@author	ROLAB 32nd CSJ
@version	1.1
@date 2017-12-31
*/

#ifndef __EXTERNAL_INTERRUPT_H_
#define __EXTERNAL_INTERRUPT_H_

#include <avr/io.h>
#include <avr/interrupt.h>

/**	인터럽트	*/
///@{
#define INT0	0	///<	External Interrupt 0. PORTB.0
#define INT1	1	///<	External Interrupt 1. PORTB.1
#define INT2	2	///<	External Interrupt 2. PORTB.2
#define INT3	3	///<	External Interrupt 3. PORTB.3
#define INT4	4	///<	External Interrupt 4. PORTE.4
#define INT5	5	///<	External Interrupt 5. PORTE.5
#define INT6	6	///<	External Interrupt 6. PORTE.6
#define INT7	7	///<	External Interrupt 7. PORTE.7
///@}

/**
@brief	인터럽트 모드
@warning	External Interrupt 0,1,2,3 cannot use change mode
*/
typedef enum _INT_MODE_t {
LOW, CHANGE, FALLING, RISING
} INT_MODE_t;

/**
@brief	외부 인터럽트 등록 함수
@details	아두이노 인터럽트 함수와 거의 호환
@param	interrupt	인터럽트 종류
@param	hadler		인터럽트 핸들러(인터럽트 발생 시 실행할 함수 이름)
@param	mode		인터럽트 발동 모드
@see	INT_MODE_t
@warning	External Interrupt 0,1,2,3 cannot use change mode
*/
void attatchInterrupt(unsigned char interrupt,void (*handler)(void), INT_MODE_t mode);

/**
@brief	외부 인터럽트 해제 함수
@param	interrupt	인터럽트 종류
*/
void detatchInterrupt(unsigned char interrupt);

/*
#ifndef NULL_FUNC
#include "timer_simple.h"
#endif
*/
#endif
