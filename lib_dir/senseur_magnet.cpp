#include "../includes/senseur_magnet.h"

senseur_magnetique::senseur_magnetique()
{

}

senseur_magnetique::~senseur_magnetique()
{

}

uint16_t senseur_magnetique::getValeurNumerique()
{
	return convertisseur_.lecture(6);
}

bool senseur_magnetique::magnetPresent()
{
	if(getValeurNumerique()>SEUIL_UPPER || getValeurNumerique()<SEUIL_LOWER)
		return true;
	else
		return false;
}

/* Documents: Senseur magnetique en interrupteur
 * Analog comparator: p.241-243/586
 * Vector utilisé:ANALOG_COMP_vect (ATmega324P)
 * http://www.avr-tutorials.com/comparator/utilizing-avr-analog-comparator-interrupt-feature
 */
