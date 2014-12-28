/****************************************************************************
* Fichier: boutonEnfonce.cpp
* Auteur: Maxime Perusse, Anas Balboul, Nam Nguyen & Kim Piché
* Date: Avril 2014
* Description: implementation de la fonction boutonEnfonce
****************************************************************************/
#include <avr/io.h>
#define F_CPU 8000000UL  // 8 MHz    
#include <util/delay.h> 
#include <boutonEnfonce.h>

/****************************************************************************
* Fonction:	boutonEnfonce()() 
* Description:  Permet de tester si le bouton est enfoncé ou non 
* Return:     	bool, oui si enfoncé
****************************************************************************/
bool boutonEnfonce()
{  
  if ( PIND & 0x04 )
  {
    _delay_ms(10);
    if ( PIND & 0x04 )
    {
      return true;
    }
  }
  return false;
}
