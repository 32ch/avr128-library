/**
 * @file	timer_simple.c
 * @version	1.1
 * @date	Created: 2017-12-05 ���� 1:09:20
 * @author 	Author: ch
 */ 

#include "timer_simple.h"
inline unsigned char _returnTimerAddress(unsigned char timer)
{
	switch(timer)
	{
		case 0:
		return _TIMER0;
		case 1:
		return _TIMER1;
		case 2:
		return _TIMER2;
		case 3:
		return _TIMER3;
	}
	/*# error "Choose timer 0 to 3. Atmega128 has only 4 timer/counter"*/
}



/**
@brief	TCCR setting
@details	Output Channel ������ �� �ڵ����� �� ��Ʈ ����(DDRx)�� ������� ����,
@param	timer	timer ����
@param	wgm		waveform generation mode. 
@param	prescale	prescaler set. \n prescaler bit�� �־ ��
@param	...		output channel ����� �� ��������.\n 16bit timer�� ���ÿ� 3������ ���� ����.\n output channel | compare output mode
@section	example
@code
setInitTimer(1,14,2,OCR_A|OC_CLR, OCR_B|OC_SET);	//WGM : 14(FastPWM_ICR). Channel A : Clear mode. Channel B : Set mode
@endcode
*/
void setTimerInit(unsigned char timer, unsigned char wgm, unsigned char prescale,...)
{
	unsigned char old_sreg = SREG;
	cli();
	va_list setBit;
	va_start(setBit,prescale);
	switch(timer)
	{
		case 0: case 2:
		{
			volatile _TCCR8_t *const timer8 = (void *)(_returnTimerAddress(timer));
			//Error	a label can only be part of a statement and a declaration is not a statement
			//case���� ���� ������ �� �Ͼ�� error. �ذ����� case �� ���̿� �߰�ȣ �־���� ��
			timer8->_WGML = wgm & 0b01;
			timer8->_WGMH = wgm & 0b10;
			timer8->_CS = prescale & 0b111;
			if(setBit){
				timer8->_COM = va_arg(setBit, int) & 0b11;
				PORTB |= _BV(	(timer)	?	7	:	4);
			}	
		}	break;
		case 1: case 3:	{
			volatile _TCCR16_t * const timer16 = (void *)_returnTimerAddress(timer);
			timer16->a._WGML = wgm & 0b0011;
			timer16->b._WGMH = (wgm & 0b1100)>>2;
			timer16->b._CS = prescale & 0b111;
			if(setBit){
				unsigned char i;
				volatile uint8_t * const ddr = ((timer==1) ?	 0x37 : 0x22);	//&DDRB	:	&DDRE
				for(i=0; i<3 && setBit; i++)
				{
					unsigned char temp = (unsigned char)va_arg(setBit, int);
					switch(temp & 0b1100){
						case OCR_A:
						timer16->a._COMA = (temp & 0b0011);
						*ddr |= _BV(6-timer);
						break;
						case OCR_B:
						timer16->a._COMB = (temp & 0b0011);
						*ddr |= _BV(7-timer);
						break;
						case OCR_C:
						timer16->a._COMC = (temp & 0b0011);
						*ddr |= _BV(8-timer);
						break;
					}
				}
			}
		}	break;
		default:
		/*# error "Choose timer 0 to 3. Atmega128 has only 4 timer/counter"*/
		break;
	}
	va_end(setBit);
	//sei();
	SREG |= (old_sreg) & 0x80;
}


/**
@brief	TCCR�� ���� �Լ�
@param	timer	timer ����
@param	what	� ���� ������ ������(wgm, ocr channel, prescale)
@param	...		�������� : � ������ ������ ������
-	WGM	������ mode �� �Է�
-	OCR_CHANNEL	output channel | compare output mode������ �Է�. 16bit timer�� ���ÿ� 3�� ���� ����
-	PRESCALE	������ prescaler bit�� �Է�
*/
void changeTimerSet(unsigned char timer, unsigned char what,...)
{
	unsigned char old_sreg = SREG;
	cli();
	va_list setBit;
	va_start(setBit,what);
	switch(timer){
		case 0:	case 2:{
			volatile _TCCR8_t *const timer8 = (void *)(_returnTimerAddress(timer));
			unsigned char temp=0;
			if(setBit){
				temp = va_arg(setBit,unsigned int);
				switch(what)
				{
					case WGM:
					timer8->_WGML = temp & 0b01;
					timer8->_WGMH = temp & 0b10;
					break;
					case OCR_CHANNEL:
					timer8->_COM = temp & 0b11;
					break;
					case PRESCALE:
					timer8->_CS = temp & 0b111;
					break;
					default:
					break;
				}
			}
		}	break;
		case 1:	case 3:{
			volatile _TCCR16_t * const timer16 = (void *)_returnTimerAddress(timer);
			switch(what)
			{
				case WGM:
				if(setBit)
				{
					unsigned char wgm = (unsigned char)va_arg(setBit,unsigned int);
					timer16->a._WGML = wgm & 0b0011;
					timer16->b._WGMH = (wgm & 0b1100)>>2;
				}
				break;
				case OCR_CHANNEL:
				if(setBit){
					unsigned char i;
					for(i=0; i<3 && setBit; i++)
					{
						unsigned char temp = (unsigned char)va_arg(setBit, int);
							switch(temp & 0b1100){
								case OCR_A:
								timer16->a._COMA = (temp & 0b0011);
								break;
								case OCR_B:
								timer16->a._COMB = (temp & 0b0011);
								break;
								case OCR_C:
								timer16->a._COMC = (temp & 0b0011);
								break;
						}
					}
				}
				break;
				case PRESCALE:
				timer16->b._CS = (unsigned char)va_arg(setBit,int);
				break;
				default:
				break;
			}
		}	break;
	}
	va_end(setBit);
	//sei();
	SREG |= (old_sreg) & 0x80;
}


#ifndef NULL_FUNC
//extern void _nullFunc(void){};
void (*_nullFunc)(void);
#define NULL_FUNC	(_nullFunc)
#endif

static struct 
{
	void (*_timer8_interrupt[2][2])(void);
	void (*_timer16_interrupt[2][5])(void);
} _timer_interrupt;


/**
@brief	timer interrupt setting
@param	timer	timer����
@param	interrupt_type	������ Ÿ�̸� ���ͷ�Ʈ ����
@param	handler	timer interrupt �߻� �� ������ �Լ�
@param	...	�������� : Compare Match Interrupt �߻��� �񱳰����� �� pointer. &(������ ����) �������� ����
@section	example
@code
unsigned int * pwm;
setTimerInterrupt(3,INT_COMPB,comp_fnc, &pwm);		//Timer3�� compare match B�� �񱳰��� pwm���� ����
@endcode
@todo	�ٽ� Ȯ�� �ٶ�
*/
void setTimerInterrupt(unsigned char timer, unsigned char interrupt_type, void (* handler)(void),...)
{
	unsigned char old_sreg = SREG;
	cli();
	va_list setBit;
	va_start(setBit,handler);
	switch(timer){
		case 0:	case 2:{
		_timer_interrupt._timer8_interrupt[timer/2][interrupt_type] = handler;
		TIMSK |= _BV(timer*3 + interrupt_type);
		if(setBit){
			unsigned char ** ptr = va_arg(setBit, unsigned int);
			*ptr = _returnTimerAddress(timer)-2;//?
			}
		} break;
		case 1:	case 3:
		_timer_interrupt._timer16_interrupt[timer/2][interrupt_type] = handler;
		if(interrupt_type==INT_COMPC){
			ETIMSK |= _BV(timer/2);
		}
		else{
			if(setBit)
			{
				unsigned int ** ptr = va_arg(setBit, unsigned int);
				switch(interrupt_type)
				{
					case INT_COMPA:
					*ptr = _returnTimerAddress(timer)-4;
					break;
					case INT_COMPB:
					*ptr = _returnTimerAddress(timer)-6;
					break;
					default:
					break;
				}
			}
			//(timer ==1)? TIMSK |= _BV(2+interrupt_type) : ETIMSK |= _BV(2+interrupt_type);	//�ȵ�
			if(timer==1)
				TIMSK |= _BV(2+interrupt_type);
			else
				ETIMSK |= _BV(2+interrupt_type);
			}
		break;
		default:
		//# error "Choose timer 0 to 3. Atmega128 has only 4 timer/counter"
		break;
	}
	//sei();
	SREG |= (old_sreg) & 0x80;
}


/**
@brief	���ͷ�Ʈ ���� �Լ�
@param	timer	Ÿ�̸� ����
@param	interrupt_type	������ ���ͷ�Ʈ ����
*/
void clearTimerInterrupt(unsigned char timer, unsigned char interrupt_type)	//�۵� Ȯ�� �ؾ���
{
	unsigned char old_sreg = SREG;
	cli();
		switch(timer){
			case 0:	case 2:{
				_timer_interrupt._timer8_interrupt[timer/2][interrupt_type] = NULL_FUNC;
				TIMSK &= ~_BV(timer*3 + interrupt_type);
			} break;
			case 1:	case 3:
			_timer_interrupt._timer16_interrupt[timer/2][interrupt_type] = NULL_FUNC;
			if(interrupt_type == INT_COMPC)
				ETIMSK &= ~_BV(timer/2);
			else{
			//	(timer ==1) ? TIMSK &= ~_BV(2+interrupt_type): ETIMSK &= ~_BV(2+interrupt_type);	//�ȵ�
				if(timer==1)
					TIMSK &= ~_BV(2+interrupt_type);
				else
					ETIMSK &= ~_BV(2+interrupt_type);
				}
			break;
			default:
			break;
		}
		//sei();
		SREG |= (old_sreg) & 0x80;
}


/**
@brief	timer �ִ�(TOP) ����
@param 	timer	timer ����
@param	val		������ ��
@return error ����
@warning	�ݺ��� �ȿ� ������ �������� �Ͼ
*/
char setTimerLimit(unsigned char timer, unsigned int val)
{
	switch(timer)
	{
		case 0: case 2:
		{
			volatile _TCCR8_t *const timer8 = (void *)(_returnTimerAddress(timer));
			unsigned char temp = (timer8->_WGMH << 1) | (timer8->_WGML);
			if(temp !=0)
				(*(volatile uint8_t *)(_returnTimerAddress(timer)-2)) = (unsigned char) val;
		}	break;
		case 1: case 3:
		{
			volatile _TCCR16_t * const timer16 = (void *)_returnTimerAddress(timer);
			unsigned char temp = ((timer16->b._WGMH)<<2) | (timer16->a._WGML);
			switch(temp)
			{
				case Normal:
				case PhaseCorrect_8:	case PhaseCorrect_9:	case PhaseCorrect_10:
				case FastPWM_8:	case FastPWM_9:	case FastPWM_10:
				break;
				case CTC_OCR:	case Phase_FrequencyCorrect_OCR:
				case PhaseCorrect_OCR: case FastPWM_OCR:
					_REG16((_returnTimerAddress(timer)-4))._val = val;
				break;
				case CTC_ICR:	case Phase_FrequencyCorrect_ICR:
				case PhaseCorrect_ICR:	case FastPWM_ICR:
					_REG16(_returnTimerAddress(timer)-(7+timer))._val = val;
				break;
				default:
				return -1;
			}
		}	break;
	}
	return 0;
}


/**
@brief timer �ִ�(TOP) ���� �޾ƿ�
@param	timer	timer ����
@return	�� timer�� �ִ�(TOP) ��ȯ
*/
unsigned int getLimitInfo(unsigned char timer)
{
		switch(timer)
		{
			case 0: case 2:
			{
				volatile _TCCR8_t *const timer8 = (void *)(_returnTimerAddress(timer));
				unsigned char temp = (timer8->_WGMH << 1) | (timer8->_WGML);
				if(temp !=0)
					return (*(volatile uint8_t *)(_returnTimerAddress(timer)-2));
					//return _REG16(_returnTimerAddress(timer)-2)._val;
				return 0xff;
			}
			case 1: case 3:
			{
				volatile _TCCR16_t * const timer16 = (void *)_returnTimerAddress(timer);
				unsigned char temp = ((timer16->b._WGMH)<<2) | (timer16->a._WGML);
				switch(temp)
				{
					case Normal:
					return 0xffff;
					case PhaseCorrect_8:	case FastPWM_8:
					return 0x00ff;
					case PhaseCorrect_9:	case FastPWM_9:	
					return 0x01ff;
					case PhaseCorrect_10:		case FastPWM_10:
					return 0x03ff;
					case CTC_OCR:	case Phase_FrequencyCorrect_OCR:
					case PhaseCorrect_OCR: case FastPWM_OCR:
					return _REG16(_returnTimerAddress(timer)-4)._val;	//OCRnA value return
					break;
					case CTC_ICR:	case Phase_FrequencyCorrect_ICR:
					case PhaseCorrect_ICR:	case FastPWM_ICR:
					return _REG16(_returnTimerAddress(timer)-(7+timer))._val;	//ICRn value return
					break;
					default:
					return 0;
				}
			}	break;
		}
		return 0;
}


ISR(TIMER0_OVF_vect){
	_timer_interrupt._timer8_interrupt[0][INT_OVF]();
}

ISR(TIMER0_COMP_vect){
	_timer_interrupt._timer8_interrupt[0][INT_COMP]();
}

ISR(TIMER1_OVF_vect){
	_timer_interrupt._timer16_interrupt[0][INT_OVF]();
}

ISR(TIMER1_COMPA_vect){
	_timer_interrupt._timer16_interrupt[0][INT_COMPA]();
}

ISR(TIMER1_COMPB_vect){
	_timer_interrupt._timer16_interrupt[0][INT_COMPB]();
}

ISR(TIMER1_COMPC_vect){
	_timer_interrupt._timer16_interrupt[0][INT_COMPC]();
}

ISR(TIMER1_CAPT_vect){
	_timer_interrupt._timer16_interrupt[0][INT_CAPT]();
}

ISR(TIMER2_OVF_vect){
	_timer_interrupt._timer8_interrupt[1][INT_OVF]();
}

ISR(TIMER2_COMP_vect){
	_timer_interrupt._timer8_interrupt[1][INT_COMP]();
}

ISR(TIMER3_OVF_vect){
	_timer_interrupt._timer16_interrupt[1][INT_OVF]();
}

ISR(TIMER3_COMPA_vect){
	_timer_interrupt._timer16_interrupt[1][INT_COMPA]();
}

ISR(TIMER3_COMPB_vect){
	_timer_interrupt._timer16_interrupt[1][INT_COMPB]();
}

ISR(TIMER3_COMPC_vect){
	_timer_interrupt._timer16_interrupt[1][INT_COMPC]();
}

ISR(TIMER3_CAPT_vect){
	_timer_interrupt._timer16_interrupt[1][INT_CAPT]();
}

