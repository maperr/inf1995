
 
 //	8,000,000 Hz / 1024 = 7812.5 Hz ---> 0.000128 sec


#define F_CPU 8000000UL  // 8 MHz

#include <avr/io.h>
    
#include <util/delay.h>  

#include <avr/interrupt.h>

#include "../includes/son.h"

/* Routine d'interruption de la minuterie (CTC mode)
 * p.125/586
 * timer choisi est timer 3, qui correspond a des pin B7 et pin B8
 * COMnA1:0=1
 * DDR_OCnA=1
 * waveform frequency generated: fOCnA=fClk/(2*N*(1+OCRnA)
 * donc OCRnA= (fClk/(fOCnA*2*N))-1
 * Pour generer un onde 110 Hz il faut dans notre cas
 * OCRnA=(8000000/(110*2*64))-1=34.511
 * 8,000,000 Hz / 256 = 31250 Hz ---> 1 sec
 * 

 */
uint8_t calculOCRnA(uint16_t freqSortie)
{
	uint8_t duree;
	duree = (float(F_CPU) / (2.0 * 256.0 * float(freqSortie))) - 1;
	return duree;
}

void PWMmusicGenerator(uint8_t duree)
{
	TCNT0 = 0; //temps initial

	OCR0A = duree; //temps final
	/* une fois temps final = temps initial, l'interrupteur est
	 * déclenché et le routine est appelé.
	 */ 
	
	TCCR0A |= _BV(COM0B0) | _BV(WGM01); //toggle OC0B en Compare Match - CTC mode

	TCCR0B |= _BV(CS02); // clk/256 prescaling

	TIMSK0 |= _BV(OCIE0B); // call routine ISR(TIMER0_COMPB_vect)
}

void sonOn(uint8_t note)
{	
	if (note >= 45 && note <= 81)
	{		
		uint8_t duree;
		uint16_t freqSortie;
		switch(note)
		{
		case(45):
			freqSortie=110;
			break;
		case(46):
			freqSortie=117;
			break;
		case(47):
			freqSortie=123;
			break;
		case(48):
			freqSortie=131;
			break;
		case(49):
			freqSortie=139;
			break;
		case(50):
			freqSortie=147;
			break;
		case(51):
			freqSortie=156;
			break;
		case(52):
			freqSortie=165;
			break;
		case(53):
			freqSortie=175;
			break;
		case(54):
			freqSortie=185;
			break;
		case(55):
			freqSortie=193;
			break;
		case(56):
			freqSortie=208;
			break;
		case(57):
			freqSortie=220;
			break;
		case(58):
			freqSortie=233;
			break;
		case(59):
			freqSortie=247;
			break;
		case(60):
			freqSortie=262;
			break;
		case(61):
			freqSortie=277;
			break;
		case(62):
			freqSortie=294;
			break;
		case(63):
			freqSortie=311;
			break;
		case(64):
			freqSortie=330;
			break;
		case(65):
			freqSortie=349;
			break;
		case(66):
			freqSortie=370;
			break;
		case(67):
			freqSortie=392;
			break;
		case(68):
			freqSortie=415;
			break;
		case(69):
			freqSortie=440;
			break;
		case(70):
			freqSortie=466;
			break;
		case(71):
			freqSortie=494;
			break;
		case(72):
			freqSortie=523;
			break;
		case(73):
			freqSortie=554;
			break;
		case(74):
			freqSortie=587;
			break;
		case(75):
			freqSortie=622;
			break;
		case(76):
			freqSortie=659;
			break;
		case(77):
			freqSortie=678;
			break;
		case(78):
			freqSortie=740;
			break;
		case(79):
			freqSortie=784;
			break;
		case(80):
			freqSortie=830;
			break;
		case(81):
			freqSortie=880;
			break;
		default:
			break;
		}
		
		duree = calculOCRnA(freqSortie);
		PWMmusicGenerator(duree);
	}
}

void sonOff()
{
	TCCR0A=0;
	TIMSK0=0;
	TCCR0B=0;
}

