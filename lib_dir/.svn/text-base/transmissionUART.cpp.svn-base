#include <avr/io.h>
#include "../includes/memoire_24.h"

void initialisationUART ( void ) {

// 2400 bauds. Nous vous donnons la valeur des deux

// premier registres pour vous éviter des complications

UBRR0H = 0;

UBRR0L = 0xCF;

// permettre la reception et la transmission par le UART0

//UCSR0A = 'modifier ici' ; valeur initiale déjà OK

UCSR0B |= _BV(RXEN0) | _BV(TXEN0);

// Format des trames: 8 bits, 1 stop bits, none parity

//UCSR0C = 'modifier ici' ; valeur initiale déjà OK

}
  // De l'USART vers le PC

void transmissionUART ( uint8_t donnee ) {

while (!(UCSR0A & (1 << UDRE0)))
	;
	
UDR0 = donnee;
}
