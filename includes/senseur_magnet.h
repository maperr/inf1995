/****************************************************************************
* Fichier: senseur_magnet.cpp
* Auteur: Maxime Perusse, Anas Balboul, Nam Nguyen & Kim Piché
* Date: Avril 2014
* Description: Declaration des fonctions sur les roues
****************************************************************************/

#ifndef SENSEUR_MAGNET_H
#define SENSEUR_MAGNET_H

#include <avr/io.h>
#include "../includes/can.h"

using namespace std;

class senseur_magnetique
{
public:
	//constructeur par defaut
	senseur_magnetique();
	//destructeur
	~senseur_magnetique();
	//Description:  retourne la distance a laquelle l'aimant se trouve
	uint16_t getValeurNumerique();
	//Description:  Retourne la presence ou non d'un aimant
	bool magnetPresent();
private:
	can convertisseur_;//l'objet de type class
	uint16_t SEUIL_UPPER;
	uint16_t SEUIL_LOWER;

};

#endif /* SENSEUR_MAGNET_H */
