#include <avr/io.h>
#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h> 
#include <avr/interrupt.h>
#include "../includes/timer_materiel.h"

timer_materiel::timer_materiel()
	: centiemesSecondes_(0), secondes_(0), active_(false)
{}

timer_materiel::~timer_materiel(){}

uint8_t timer_materiel::getCentiemesSecondes() const
{
	return centiemesSecondes_;
}

uint16_t timer_materiel::getSecondes() const
{
	return secondes_;
}

void timer_materiel::incCentiemesSecondes()
{
	centiemesSecondes_++;
}

void timer_materiel::incSecondes()
{
	secondes_++;
	centiemesSecondes_ = 0;
}

void timer_materiel::resetMinuterie()
{
	centiemesSecondes_ = 0;
	secondes_ = 0;
	TCCR2B = 0;
	active_ = false;
}

//	8,000,000 Hz / 1024 = 7812 .5 Hz
// 7812 : 1 seconde
//	781	: 1/10 seconde
// 78 : 1/100 seconde
void timer_materiel::partirMinuterie()
{
	cli ();	
	sei ();
	
	TCNT2 = 0;
	
	OCR2A = 78;

	TCCR2A |= _BV(WGM21);

	TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);

	TIMSK2 |= _BV(OCIE2A);
	
	active_ = true;
}

bool timer_materiel::isActive()
{
	return active_;
}
