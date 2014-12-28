// fonction qui permet de donner une puissance a chaque roue
// les valeurs possibles de roueGauche et roueDroite sont entre 0 et 255 (0 etant la plus rapide)
#include <avr/io.h>
#include "../includes/roue.h"

void roue(uint8_t roueGauche, uint8_t roueDroite,bool avancerGauche, bool avancerDroite) 
{
	DDRD |= _BV(1) | _BV(4) | _BV(5) | _BV(6); // PORT D est en mode sortie pour les roues
	
	// mise a un des etat sortie OC1A et OC1B sur comparaison
	// reussie en mode PWM 8 bits, phase correcte
	// et valeur de TOP fixe a 0xFF (mode #1 de la table 16-5
	// page 135 de la description technique du ATmega324PA)

	TCCR1A |= _BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0) | _BV(WGM10);

	OCR1A = roueDroite;

	OCR1B = roueGauche;
	
	if(avancerGauche)
		PORTD |= 0b01000000;		
	else
		PORTD &= 0b10111111;						
		
	if(avancerDroite)
		PORTD &= 0b11111101;				
	else
		PORTD |= 0b00000010;

	// division d'horloge par 8 - implique une frequence de PWM fixe
	TCCR1B = _BV(CS11);
}

void eteindreMoteur()
{
	TCCR1A = 0; 
}
