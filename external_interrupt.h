/**
@file	external_interrupt.h
@brief		external interrupt header
@details	�Ƶ��̳� �ܺ� ���ͷ�Ʈ �Լ��� ���� ����
@author	ROLAB 32nd CSJ
@version	1.1
@date 2017-12-31
*/

#ifndef __EXTERNAL_INTERRUPT_H_
#define __EXTERNAL_INTERRUPT_H_

#include <avr/io.h>
#include <avr/interrupt.h>

/**	���ͷ�Ʈ	*/
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
@brief	���ͷ�Ʈ ���
@warning	External Interrupt 0,1,2,3 cannot use change mode
*/
typedef enum _INT_MODE_t {
LOW, CHANGE, FALLING, RISING
} INT_MODE_t;

/**
@brief	�ܺ� ���ͷ�Ʈ ��� �Լ�
@details	�Ƶ��̳� ���ͷ�Ʈ �Լ��� ���� ȣȯ
@param	interrupt	���ͷ�Ʈ ����
@param	hadler		���ͷ�Ʈ �ڵ鷯(���ͷ�Ʈ �߻� �� ������ �Լ� �̸�)
@param	mode		���ͷ�Ʈ �ߵ� ���
@see	INT_MODE_t
@warning	External Interrupt 0,1,2,3 cannot use change mode
*/
void attatchInterrupt(unsigned char interrupt,void (*handler)(void), INT_MODE_t mode);

/**
@brief	�ܺ� ���ͷ�Ʈ ���� �Լ�
@param	interrupt	���ͷ�Ʈ ����
*/
void detatchInterrupt(unsigned char interrupt);

/*
#ifndef NULL_FUNC
#include "timer_simple.h"
#endif
*/
#endif
