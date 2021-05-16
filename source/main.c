/*	Author: lab
 *  	Partner(s) Name: Jaired Jawed
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *
 *	Demo Link: 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;


void TimerISR() { TimerFlag = 1; }

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char threeLEDs = 0x00;
enum ThreeLEDStates { ThreeLEDStart, ThreeLEDB0, ThreeLEDB1, ThreeLEDB2 } ThreeLED_state;

void ThreeLEDsSM() {
	switch (ThreeLED_state) {
		case ThreeLEDStart:
			ThreeLED_state = ThreeLEDB0;
			break;
		case ThreeLEDB0:
			ThreeLED_state = ThreeLEDB1;
			break;
		case ThreeLEDB1:
			ThreeLED_state = ThreeLEDB2;
			break;
		case ThreeLEDB2:
			ThreeLED_state = ThreeLEDB0;
			break;
		default:
			ThreeLED_state = ThreeLEDStart;
			break;
	}

	switch (ThreeLED_state) {
		case ThreeLEDB0:
			threeLEDs = 0x01;
			break;
		case ThreeLEDB1:
			threeLEDs = 0x02;
			break;
		case ThreeLEDB2:
			threeLEDs = 0x04;
			break;
		default:
			break;
	}

	// set bit 0 to 1
	// then set bit 1 to 1
	// then set bit 2 to 1
	// 1 second each
	//	
}


unsigned char blinkingLED = 0x00;
enum BlinkingLEDStates { BlinkLEDStart, BlinkLEDB2_ON, BlinkLEDB2_OFF } BlinkingLED_state;

void BlinkingLEDSM() {
	switch (BlinkingLED_state) {
		case BlinkLEDStart:
			BlinkingLED_state = BlinkLEDB2_ON;
			break;
		case BlinkLEDB2_ON:
			BlinkingLED_state = BlinkLEDB2_OFF;
			break;
		case BlinkLEDB2_OFF:
			BlinkingLED_state = BlinkLEDB2_ON;
			break;
		default:
			BlinkingLED_state = BlinkLEDStart;
			break;
	}

	switch (BlinkingLED_state) {
		case BlinkLEDB2_ON:
			blinkingLED = 0x01;
			break;
		case BlinkLEDB2_OFF:
			blinkingLED = 0x00;
			break;
		default:
			break;
	}
	// set bit 3 to 1 for 1 second
	// then set to 0 for another second
}

void CombineLEDsSM() {

	// combine both variables from the other SM functions to PORTB
	//
	
	PORTB = (blinkingLED << 3) | (threeLEDs);
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(1000);
	TimerOn();



    /* Insert your solution below */
    while (1) {
	ThreeLEDsSM();
	BlinkingLEDSM();
	CombineLEDsSM();
	while (!TimerFlag) {};
	TimerFlag = 0;
    }
    return 1;
}
