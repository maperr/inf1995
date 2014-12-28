/****************************************************************************
* Fichier: timer_materiel.cpp
* Auteur: Maxime Perusse, Anas Balboul, Nam Nguyen & Kim Piché
* Date: Avril 2014
* Description: declaration de la classe timer_materiel
****************************************************************************/
#ifndef TMAT_H
#define TMAT_H

class timer_materiel
{
public:

   timer_materiel();
   ~timer_materiel();
   
	// Retour:	uint8_t : le nombre de centiemes de secondes
   uint8_t getCentiemesSecondes() const;
	//Description:	retourne le nombre de secondes
   uint16_t getSecondes() const;
	//Description:	incremente le nombre de centiemes de secondes
   void incCentiemesSecondes();
	//Description:	incremente le nombre de secondes et annule la valeur de centiemes de secondes
   void incSecondes();
	//Description:	initialise la minuterie
   void resetMinuterie();
	//Description:	demarre la minuterie
   void partirMinuterie();
	//Description:	retourne si oui ou non la minuterie roule presentement
   bool isActive();

private:
	volatile uint8_t centiemesSecondes_;//sauvgarder le compteur de centiemes de secondes
	volatile uint16_t secondes_;//sauvgarder le compteur de secondes
	bool active_; //si le timer est active ou non 
};

#endif /* TMAT_H */
