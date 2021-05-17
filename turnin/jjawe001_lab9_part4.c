/*	Author: lab
 *  	Partner(s) Name: Jaired Jawed
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #4
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
unsigned int ThreeLED_time = 0;

void ThreeLEDsSM() {
	if (ThreeLED_time >= 300) {
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

		ThreeLED_time = 0;
	}
	ThreeLED_time += 1;
	// set bit 0 to 1
	// then set bit 1 to 1
	// then set bit 2 to 1
	// 1 second each
	//	
}


unsigned char blinkingLED = 0x00;
enum BlinkingLEDStates { BlinkLEDStart, BlinkLEDB2_ON, BlinkLEDB2_OFF } BlinkingLED_state;
unsigned int BlinkLED_time = 0;

void BlinkingLEDSM() {
	if (BlinkLED_time >= 1000) {
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

		BlinkLED_time = 0;
	}
	BlinkLED_time += 1;
	// set bit 3 to 1 for 1 second
	// then set to 0 for another second
}

unsigned char speaker = 0x00;
enum SpeakerStates { SpeakerStart, SpeakerON, SpeakerWait, SpeakerDown, SpeakerUp, SpeakerClear } speaker_state;
unsigned int speaker_time = 0;
unsigned int sound = 1;

void SpeakerSM() {
	unsigned char button1 = (~PINA & 0x07) == 0x01;
	unsigned char button2 = (~PINA & 0x07) == 0x02;
	unsigned char button3 = (~PINA & 0x07) == 0x04;
	unsigned char noButton = (~PINA & 0x07) == 0x00;

	switch (speaker_state) {
		case SpeakerStart:
			speaker_state = SpeakerWait;
			break;
		case SpeakerWait:
			if (button3) {
				speaker_state = SpeakerON;
			}
			else if (button2) {
				speaker_state = SpeakerUp;
			}
			else if (button1) {
				speaker_state = SpeakerDown;
			}
			else {
				speaker_state = SpeakerWait;
			}
			break;
		case SpeakerON:
			if (noButton) {
				speaker_state = SpeakerWait;
			}
			else {
				speaker_state = SpeakerON;
			}
			break;
		case SpeakerClear:
			if (noButton) {
				speaker_state = SpeakerWait;
			}
			else {
				speaker_state = SpeakerClear;
			}
			break;
		case SpeakerUp:
		case SpeakerDown:
			speaker_state = SpeakerClear;
			break;
		default:
			speaker_state = SpeakerStart;
			break;
	}

	switch (speaker_state) {
		case SpeakerWait:
			speaker_time = 0;
			speaker = 0x00;
			break;
		case SpeakerUp:
			if (sound < 6) {
				sound++;
			}
			break;
		case SpeakerDown:
			if (sound > 1) {
				sound--;
			}
			break;
		case SpeakerON:
			if (speaker_time > (sound * 2)) {
				speaker_time = 0;
			}
			else if (speaker_time > sound) {
				speaker = 0x00;
			}
			else {
				speaker = 0x01;
			}

			speaker_time++;
			break;
		default:
			speaker = 0x00;
			break;
	}
}

void CombineLEDsSM() {

	// combine all variables from the other SM functions to PORTB
	PORTB = (speaker << 4) | (blinkingLED << 3) | (threeLEDs);
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(1);
	TimerOn();



    /* Insert your solution below */
    while (1) {
	ThreeLEDsSM();
	BlinkingLEDSM();
	SpeakerSM();
	CombineLEDsSM();
	while (!TimerFlag) {};
	TimerFlag = 0;
    }
    return 1;
}
