/****************************************************************************
* Fichier: capteur_distance.cpp
* Auteur: Maxime Perusse, Anas Balboul, Nam Nguyen & Kim Piché
* Date: Avril 2014
* Description: implementation des classes capteur_distance_GP2D12
*	       et capteur_distance_GP2D120
****************************************************************************/

#ifndef CAPTEUR_DISTANCE_H
#define CAPTEUR_DISTANCE_H

#include <avr/io.h>
#include "../includes/can.h"

enum obstacle
{
	AUCUN,	//0
	PRES,	//1
	LOIN	//2
};

const uint16_t  valeur_pres = 100, 
		valeur_loin = 162, 
		valeur_obstacle = 450;
			   
class capteur_distance_GP2D12 //classes pour le capteur_distance_GP2D12
{
public:

	capteur_distance_GP2D12();
	~capteur_distance_GP2D12();
	//Description:  Converti les signaux analogique du capteur 
	uint16_t getValeurNumerique();
	//Description:  Retourne la variable de distanc
	obstacle detectionObstable();
	//Description:  Retourne la variable de distance
	bool detectionRobotLoin();

private:
	can convertisseur_;// objet de type Can

};

class capteur_distance_GP2D120//classes pour le capteur_distance_GP2D120
{
public:

	capteur_distance_GP2D120();
	~capteur_distance_GP2D120();
	
	//Description:  Retourne la variable de distance
	uint16_t getValeurNumerique();
	//Description:  Retourne la variable de distance
	bool detectionRobotPres();

private:
	can convertisseur_;// objet de type Can

};

#endif /* CAPTEUR_DISTANCE_H */
