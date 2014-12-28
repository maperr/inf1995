/****************************************************************************
* Fichier: capteur_distance.cpp
* Auteur: Maxime Perusse, Anas Balboul, Nam Nguyen & Kim Piché
* Date: Avril 2014
* Description: implementation des classes capteur_distance_GP2D12
*	       et capteur_distance_GP2D120
****************************************************************************/
#include <capteur_distance.h>

/****************************************************************************
* Fonction:	capteur_distance_GP2D12()
* Description:  Constructeur par defaut 
****************************************************************************/
capteur_distance_GP2D12::capteur_distance_GP2D12()
{

}

/****************************************************************************
* Fonction:	capteur_distance_GP2D12()
* Description:  Destructeur par defaut 
****************************************************************************/
capteur_distance_GP2D12::~capteur_distance_GP2D12()
{

}

/****************************************************************************
* Fonction:	capteur_distance_GP2D12()
* Description:  Converti les signaux analogique du capteur 
* Retour:	uint16_t : valeur numérique de la distance
****************************************************************************/
uint16_t capteur_distance_GP2D12::getValeurNumerique()
{
	return convertisseur_.lecture(0);
}

/****************************************************************************
* Fonction:	detectionObstable()
* Description:  Retourne la variable de distance
* Retour:	uint16_t : valeur numérique de la distance
****************************************************************************/
obstacle capteur_distance_GP2D12::detectionObstable()
{
	uint16_t valeurNumerique = convertisseur_.lecture(0);
	
	if (valeurNumerique > valeur_obstacle)
		return PRES;
	else if (valeurNumerique < valeur_loin)
		return LOIN;
	else
		return AUCUN;
}

bool capteur_distance_GP2D12::detectionRobotLoin()
{
	uint16_t valeurNumerique = convertisseur_.lecture(0);
	if (valeurNumerique > valeur_loin) 
			return true;
	else
		return false;
}



/****************************************************************************
* Fonction:	capteur_distance_GP2D120()
* Description:  Constructeur par defaut 
****************************************************************************/
capteur_distance_GP2D120::capteur_distance_GP2D120()
{

}

/****************************************************************************
* Fonction:	capteur_distance_GP2D120()
* Description:  Destructeur par defaut 
****************************************************************************/
capteur_distance_GP2D120::~capteur_distance_GP2D120()
{

}

uint16_t capteur_distance_GP2D120::getValeurNumerique()
{
	return convertisseur_.lecture(1);
}

bool capteur_distance_GP2D120::detectionRobotPres()
{
	uint16_t valeurNumerique = convertisseur_.lecture(1);
	
	if (valeurNumerique > valeur_pres) 
		return true;
	else
		return false;
}


