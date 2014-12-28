#define F_CPU 8000000UL  // 8 MHz  
#include <avr/io.h>
#include <util/delay.h>
#include "../includes/timer_logiciel.h"

void delaiSecondesLogiciel(int secondes)
{
	for(int i = 0; i < secondes; i++)
		for (int j = 0; j < 20; j++)
			_delay_loop_2(50000);
}

void delaiMsLogiciel(int ms)
{
	for (int j = 0; j < ms; j++)
		_delay_ms(1);
}




