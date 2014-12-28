//Fonction permettant d'allumer la led d'une certaine couleur pour quelques secondes
//Arguments : aucun
//return : bool
//Il faut utiliser les broches B0 et B1 pour la LED. Lorsque B0 eest à 1,
//la lumière est rouge.
#include "../includes/LED.h"
#include <avr/io.h>
#define F_CPU 8000000UL  // 8 MHz    
#include <util/delay.h>
// 1/4 incr _delay_loop_2 : 0.000000125 sec = 0.000125 m sec = 0.125 u sec
// 1 inc _delay_loop_2 : 0.0000005 sec = 0.0005 m sec = 0.5 u sec
// 1 sec = 2,000,000 inc
// 1 m sec = 2,000 inc
// 1 u sec = 2 inc

void LED(unsigned int couleur, unsigned int seconde)
{
	DDRB = 0xFF; // PORTB est en mode sortie
	if(couleur == AMBRE)
	{
		for(unsigned int i = 0; i < seconde*1000; i++)
		{
			if (i % 20 == 0)
				PORTB = ROUGE;
			else
				PORTB = VERT;
				
			_delay_ms(1);			
		}
	}
	else
	{
		PORTB = couleur;
		for(int i=0;i<seconde;i++)	{
			for (int j = 0; j < 40; j++)	{
				_delay_loop_2(50000);	
			}
		}		
		PORTB = ETEINT;
	}
}

void allumerLED()
{
	DDRB=0xff;
	PORTB=1;

}

void eteindreLED()
{
	DDRB=0xff;
	PORTB=0;

}
