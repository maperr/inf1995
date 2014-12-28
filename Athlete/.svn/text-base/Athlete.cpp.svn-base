/****************************************************************************
* Fichier: Athlete.cpp
* Auteur: Maxime Perusse, Anas Balboul, Nam Nguyen & Kim Piché
* Date: Avril 2014
* Description: Programme main de l'athlete
****************************************************************************/

#define F_CPU 8000000UL
#include <avr/io.h> 
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../includes/LED.h"
#include "../includes/roue.h"
#include "../includes/timer_materiel.h"
#include "../includes/timer_logiciel.h"
#include "../includes/boutonEnfonce.h"
#include "../includes/transmissionUART.h"
#include "../includes/memoire_24.h"
#include "../includes/can.h"
#include "../includes/son.h"
#include "../includes/senseur_magnet.h"

using namespace std;

/*
 * Valeurs possibles de la variable compteurObstacle
 * 1 : ligne depart
 * 2 : premier obstacle ligne vide
 * 3 : premier obstacle ligne pleine
 * 4 : premier detour 90
 * 5 : deuxieme detour 90
 * 6 : deuxieme obstacle ligne pleine
 * 7 : deuxieme obstacle ligne vide
 * 8 : ligne d'arrivee
 */
 volatile uint8_t compteurObstacle;
 
// enum pour les différentes lectures du capteurs
enum lectureCapteur
{
	CENTRE,
	TOURNER_GAUCHE, 
	TOURNER_DROITE, 
	TOURNER_GAUCHE_FORT, 
	TOURNER_DROITE_FORT, 
	AUCUN
};
volatile lectureCapteur directionCapteur;

// enum pour les différents états principaux du robot
enum etat
{
	ETAT_DEPART, 
	ETAT_LIGNE_DROITE, 
	ETAT_OBSTACLE_PLEIN, 
	ETAT_OBSTACLE_VIDE, 
	ETAT_DETOUR_90, 
	ETAT_FIN
};
volatile etat etatRobot;

// enum pour les différentes étapes de l'obstacle "ligne vide"
enum etatVide
{
	LIGNE_VIDE_DETOUR_1, 
	LIGNE_VIDE_RECHERCHE_LIGNE_1, 
	LIGNE_VIDE_DETOUR_2, 
	LIGNE_VIDE_SUIVEUR_LIGNE, 
	LIGNE_VIDE_RECHERCHE_LIGNE_2, 
	LIGNE_VIDE_DETOUR_FINAL
};
volatile etatVide etapeObstacleLigneVide;

// enum pour les différentes étapes de l'obstacle "ligne pleine"
enum etatPlein
{
	LIGNE_PLEINE_AVANCER, 
	LIGNE_PLEINE_RECHERCHE_LIGNE_1,  
	LIGNE_PLEINE_SUIVEUR_LIGNE, 
	LIGNE_PLEINE_RECHERCHE_LIGNE_2, 
	LIGNE_PLEINE_DETOUR_FINAL,
	LIGNE_PLEINE_STABILISATION
};
volatile etatPlein etapeObstacleLignePleine;

// booléens servant à la logistique de la fonction Detour
volatile bool detectionAucun;
volatile bool detectionFinDetour;

// booléen servant à la logistique de la fonction codeObstacleVide
volatile bool detectionLigneFinObstacleLignePleine;

//Pour poteaux P1 à P4
//poteaux[0] : 0 si obstacle sur P1, 1 si obstacle sur P2
//poteaux[1] : 0 si obstacle sur P2, 1 si obstacle sur P4
volatile uint8_t poteaux[2];

// Variable permettant de modifier la vitesse globale du robot
int8_t deltaV;

// timer pour trouver la ligne discontinue
timer_materiel timer;

/****************************************************************************
* Fonction:	ISR(TIMER2_COMPA_vect)
* Description:  Interruption pour le timer 2
* Paramètres:	TIMER2_COMPA_vect : l'interruption pour timer 2
* Remarque:     Routine d'interruption. Utilisé pour calculé le temps
*				durant lequel le robot ne vois plus la ligne. Permet de
*				trouver l'obstacle "ligne vide" au bon endroit (car les
*				capteurs ont tendance à perdre la ligne de vue même si le
*				robot est centré)
****************************************************************************/
ISR(TIMER2_COMPA_vect)
{	
	// incrémente le nombre de secondes à chaque 100 centièmes de seconde
	if (timer.getCentiemesSecondes() == 99)
	{
		timer.incSecondes();
	}
	else
		timer.incCentiemesSecondes();	
	
	TCNT2 = 0;
}

// 1 : bouton poussoir est enfoncé, 0 : pas enfoncé
volatile uint8_t boutonPoussoir;

/****************************************************************************
* Fonction:	ISR(INT0_vect)
* Description:  Interruption pour le bouton poussoir
* Paramètres:	INT0_vect : l'interruption pour le bouton poussoir
* Remarque:     Routine d'interruption.
****************************************************************************/
ISR (INT0_vect) 
{
	_delay_ms(30);
	
	if (!(PIND & 0x04))
	{	
		boutonPoussoir = 1;
	}
}

// Déclaration des fonctions
void testCapteur();
void codeSuiveurLigne();
void codeDetour(bool,bool);
void codeObstaclePlein();
void codeObstacleVide();
void initialisation();

int main()
{		
	initialisation();
	senseur_magnetique sensorMagnetic;
	
	// tableau pour les 14 bits du message RC-5
	uint8_t bitsReception[14];
	
	// reste à false tant que le premier bit à 1 n'est pas détecté
	bool detection = false;
	
	// reste à false tant que le message trouvé n'a pas la bonne adresse 
	// (0x9 pour notre équipe). Permet d'éviter de prendre le mauvais message
	bool bonneAdresse = false;
	
	// on ne sort pas de la boucle tant que le bon message a été capté,
	// ou si le bouton pressoir est enfoncé
	do
	{
		// on ne sort pas de la boucle tant que le premier bit à 1
		// n'as pas été capté,ou si le bouton pressoir est enfoncé
		do
		{
			// nous utilisons ici un "anti-rebond" double pour éviter
			// de sortir de la boucle à cause d'une fluctuation de tension<
			// ou d'un bruit
			if (!(PINB & 0x01))
			{
				_delay_us(200);
				if (!(PINB & 0x01))
				{
					_delay_us(200);
					if (!(PINB & 0x01))
					{
						detection = true;
					}
				}	
			}
		}
		while (detection == false && boutonPoussoir == 0);
		
		// lorsqu'une adresse a été détecté, nous vérifions si l'adresse
		// a une bonne clé.
		if (boutonPoussoir == 0)
		{
			// détection des 14 bits
			for (int i = 0; i < 14; i++)
			{
				if (PINB & 0x01)
					bitsReception[i] = 0;
				else
					bitsReception[i] = 1;
				
				// délai un peu supérieur afin d'éviter d'arriver
				// entre 2 bits
				_delay_us(1700);
			}
			
			if (bitsReception[0] == 1 &&
				bitsReception[1] == 1 &&
				bitsReception[2] == 0 &&
				bitsReception[3] == 0 &&
				bitsReception[4] == 1 &&
				bitsReception[5] == 0 &&
				bitsReception[6] == 0 &&
				bitsReception[7] == 1 )
			{
				bonneAdresse = true;
			}
			else
				detection = false;
		}	
	}
	while (bonneAdresse == false && boutonPoussoir == 0);
	
	// si le bouton a été enfoncé (plan B)
	if (bonneAdresse == false)
	{
		poteaux[0] = 0;
		poteaux[1] = 0;
		// DEL rouge
		PORTC |= _BV(6);
	}
	// si la transmission a fonctionné
	else
	{
		//seuls les bits 13 et 14 du message RC-5 sont utilisés pour nos obstacles
		poteaux[0] = bitsReception[12];
		poteaux[1] = bitsReception[13];
		// DEL verte
		PORTC |= _BV(7);		
	}
	
	// boucle principale du fonctionnement du robot
	while (etatRobot != ETAT_FIN)
	{
		// le sensor magnétique est testé par scrutation ici
		if (sensorMagnetic.magnetPresent()==false)
		{
			testCapteur();
					
			switch (etatRobot)
			{
				//état très court qui change dès que le robot quitte la ligne de départ
				//permet de ne pas incrémenter son compteur d'obstacles
				case ETAT_DEPART :
				{
					roue(127,127,true,true);
					break;
				}
				
				case ETAT_LIGNE_DROITE :
				{
					codeSuiveurLigne();				
					break;
				}
				
				case ETAT_OBSTACLE_PLEIN :
				{
					codeObstaclePlein();
					break;
				}
				
				case ETAT_OBSTACLE_VIDE :
				{
					codeObstacleVide();
					break;
				}
				
				case ETAT_DETOUR_90 :
				{
					codeDetour(true, false);
					break;
				}
				
				case ETAT_FIN :
				{
					eteindreMoteur();
					break;
				}
				
				default :
					eteindreMoteur();
					etatRobot = ETAT_FIN;
					break;
			}
		}	
		else
		{
			eteindreMoteur();
			_delay_ms(100);
		}
	}
	
	return 0;
}

/****************************************************************************
* Fonction:		testCapteur()
* Description:  Vérifie les données des capteurs du suiveur de ligne, change 
*				l'état du robot et appèle les différentes fonctions selon le cas.
* Paramètres:	aucun
* Retour:		aucun
* Remarque:		est appelé dans la boucle principale du main à chaque itération.
*				Le capteur LSS05 est utilisé sur ce robot.
*				1 : le capteur détecte du noir, 0 : il voit du blanc 
****************************************************************************/
void testCapteur()
{		
	// au départ : 11111
	// on change état lorsque 0XXXX ou XXXX0
	if (etatRobot == ETAT_DEPART && (!(PINA & 0X01) || !(PINA & 0X10)))
	{
		etatRobot = ETAT_LIGNE_DROITE;
		compteurObstacle++;
	}
		
	if (((PINA & 0X01) && (PINA & 0X02) && (PINA & 0X04) && !(PINA & 0X08) && !(PINA & 0X10)) 	||
		((PINA & 0X01) && (PINA & 0X02) && (PINA & 0X04) && (PINA & 0X08) && !(PINA & 0X10)) 	||
		((PINA & 0X01) && (PINA & 0X02) && (PINA & 0X04) && (PINA & 0X08) && (PINA & 0X10))		)
	{
		//	11100
		//	11110
		//	11111
		
		//	Dans ce cas, le robot est soit face à un obstacle ligne pleine, un détour à 90 degrée,
		// 	ou bien la ligne d'arrivé. On utilise le compteur d'obstacle afin de savoir où on se
		//  trouve dans le trajet.
		
		// permet d'éviter de changer d'état lorsque le robot croise des lignes pleines à l'intérieur
		// d'une de ses routines de contour d'obstacle
		if (etatRobot == ETAT_LIGNE_DROITE)
		{			
			switch (compteurObstacle)
			{					
				case 3 :
				case 6 :
					// on entre en état d'obstacle ligne pleine seulement si le poteau à été détecté
					if ((compteurObstacle == 3 && poteaux[1] == 1) || (compteurObstacle == 6 && poteaux[1] == 0))
					{
						etatRobot = ETAT_OBSTACLE_PLEIN;
						etapeObstacleLignePleine = LIGNE_PLEINE_AVANCER;
					}
					// sinon, on continue un peut afin de dépasser la ligne transversale
					else
					{
						roue(127 + deltaV,129 + deltaV,true,true);
						_delay_ms(500);
						compteurObstacle++;
					}
					break;
				case 4 :
				case 5 :
					etatRobot = ETAT_DETOUR_90;
					break;
				case 8 :					
				default :
					// ligne d'arrivé, on éteint la DEL
					etatRobot = ETAT_FIN;
					PORTC &= ~(_BV(6)) & ~(_BV(7));
					break;							
			}
		}
		
		// On utilise le timer lors de la dernière étape de l'obstacle ligne pleine, afin de donner
		// quelques instants au robot pour qu'il se stabilise sur la ligne avant de passer dans l'état
		// suiveur de ligne normal. Ce timer est aussi utilisé dans le mode suiveur de ligne normal afin
		// de calculer le temps passé lorsque les capteurs ne voit que du blanc, qui nous permet de trouver
		// l'obstacle ligne vide. Ce timer est normalement réinitialisé chaque fois qu'un des capteurs détecte
		// du noir. Durant la période de stabilisation, nous ne voulons pas réinitialiser le timer.
		if (etapeObstacleLignePleine != LIGNE_PLEINE_STABILISATION)	
			timer.resetMinuterie();
	}
	else if (!(PINA & 0X01) && !(PINA & 0X02) && !(PINA & 0X04) && !(PINA & 0X08) && !(PINA & 0X10))
	{
		//	00000
		
		//on évite que le robot croit qu'il y a un obstacle vide
		//si ses capteurs s'éteigne momentanément sur une ligne droite
		//Pour ce faire, nous utilisons un timer par interruption
		
		int8_t centiemesSecondes = timer.getCentiemesSecondes();
		
		if (centiemesSecondes > 30 && etapeObstacleLignePleine != LIGNE_PLEINE_STABILISATION)
		{
			// obstacle ligne vide
			if (etatRobot == ETAT_LIGNE_DROITE && (compteurObstacle == 2 || compteurObstacle == 7))
			{
				// on entre en état d'obstacle ligne vide seulement si le poteau à été détecté
				if ((compteurObstacle == 2 && poteaux[0] == 1) || (compteurObstacle == 7 && poteaux[0] == 0))
				{
					etatRobot = ETAT_OBSTACLE_VIDE;
					etapeObstacleLigneVide = LIGNE_VIDE_DETOUR_1;
					timer.resetMinuterie();
				}
				// sinon, on continue un peut afin de retrouver la ligne
				else
				{
					timer.resetMinuterie();
					roue(127 + deltaV,129 + deltaV,true,true);
					_delay_ms(500);
					compteurObstacle++;				
				}
			}
			// le robot est dans l'obstacle ligne vide et est arrivé à la fin de la ligne parallèle
			else if (etatRobot == ETAT_OBSTACLE_VIDE && etapeObstacleLigneVide == LIGNE_VIDE_SUIVEUR_LIGNE)
			{
				etapeObstacleLigneVide = LIGNE_VIDE_RECHERCHE_LIGNE_2;
				timer.resetMinuterie();
			}
			// le robot est dans l'obstacle ligne pleine et est arrivé à la fin de la ligne parallèle
			else if (etatRobot == ETAT_OBSTACLE_PLEIN && etapeObstacleLignePleine == LIGNE_PLEINE_SUIVEUR_LIGNE)
			{
				etapeObstacleLignePleine = LIGNE_PLEINE_RECHERCHE_LIGNE_2;
				detectionLigneFinObstacleLignePleine = false;
				timer.resetMinuterie();
			}
		}	
		else
		{
			// en temps normal, lorsque les capteurs ne voient que du blanc, on continue l'action précédente
			directionCapteur = AUCUN;
			
			//partir le timer si il n'est pas déjà actif
			if (!timer.isActive())
				timer.partirMinuterie();
		}
	}
	else if (PINA & 0X04)
	{
		// XX1XX
		
		// on s'assure que le robot se tienne un peu plus à gauche de la ligne à l'arrivé des détour à 90 degrés
		if (etatRobot == ETAT_LIGNE_DROITE && (compteurObstacle == 4 ||compteurObstacle == 5))
			directionCapteur = TOURNER_GAUCHE;
		// on s'assure que le robot penche un peu plus vers la droite alors qu'il suit la ligne courbe
		else if (etatRobot == ETAT_OBSTACLE_PLEIN && etapeObstacleLignePleine == LIGNE_PLEINE_SUIVEUR_LIGNE)
			directionCapteur = TOURNER_DROITE;
		else
			directionCapteur = CENTRE;
			
		// voir le commentaire à la ligne 378
		if (etapeObstacleLignePleine != LIGNE_PLEINE_STABILISATION)	
			timer.resetMinuterie();
	}	
	else if ( (!(PINA & 0X01) && (PINA & 0X02) && !(PINA & 0X04) && !(PINA & 0X08) && !(PINA & 0X10))	||
			  ((PINA & 0X01) && (PINA & 0X02) && !(PINA & 0X04) && !(PINA & 0X08) && !(PINA & 0X10))	)
	{		
		// 01000
		// 11000 
		
		directionCapteur = TOURNER_GAUCHE;
		
		// voir le commentaire à la ligne 378
		if (etapeObstacleLignePleine != LIGNE_PLEINE_STABILISATION)	
			timer.resetMinuterie();
	}	
	else if ((PINA & 0X01) && !(PINA & 0X02) && !(PINA & 0X04) && !(PINA & 0X08) && !(PINA & 0X10))
	{
		//	10000
		directionCapteur = TOURNER_GAUCHE_FORT;
		
		// voir le commentaire à la ligne 378
		if (etapeObstacleLignePleine != LIGNE_PLEINE_STABILISATION)	
			timer.resetMinuterie();
	}
	else if ( (!(PINA & 0X01) && !(PINA & 0X02) && !(PINA & 0X04) && (PINA & 0X08) && !(PINA & 0X10))	||
			  (!(PINA & 0X01) && !(PINA & 0X02) && !(PINA & 0X04) && (PINA & 0X08) && (PINA & 0X10))		)
	{
		//	00010
		//	00011
		directionCapteur = TOURNER_DROITE;
		
		// voir le commentaire à la ligne 378
		if (etapeObstacleLignePleine != LIGNE_PLEINE_STABILISATION)	
			timer.resetMinuterie();
	}
	else if (!(PINA & 0X01) && !(PINA & 0X02) && !(PINA & 0X04) && !(PINA & 0X08) && (PINA & 0X10))
	{
		//	00001
		directionCapteur = TOURNER_DROITE_FORT;
		
		// voir le commentaire à la ligne 378
		if (etapeObstacleLignePleine != LIGNE_PLEINE_STABILISATION)	
			timer.resetMinuterie();
	}	
	else
	{
		directionCapteur = CENTRE;
		
		// voir le commentaire à la ligne 378
		if (etapeObstacleLignePleine != LIGNE_PLEINE_STABILISATION)
			timer.resetMinuterie();
	}
	
	// Logistique pour le début de l'obstacle ligne pleine. Le robot a au préalable dépassé la ligne, et
	// s'est avancé un peu afin d'avoir plus d'espace entre la ligne droite et la ligne courbe de l'obstacle.
	// Dans l'état LIGNE_PLEINE_RECHERCHE_LIGNE_1, le robot est en rotation vers la gauche, et pour savoir quand
	// s'arrêté, il doit d'abord voir du blanc sur les 4 capteurs de gauche. Lorsque cela est arrivé, il arrête sa
	// rotation dès qu'il détecte la ligne courbe avec un de ses capteurs de gauche.
	if (etatRobot == ETAT_OBSTACLE_PLEIN && etapeObstacleLignePleine == LIGNE_PLEINE_RECHERCHE_LIGNE_1)
	{
		if (!(PINA & 0X02) && !(PINA & 0X04) && !(PINA & 0X08) && !(PINA & 0X10))
		{
			detectionAucun = true;
		}
		
		if (((PINA & 0X01) || (PINA & 0X02)) && detectionAucun == true)
		{
			detectionFinDetour = true;
		}			
	}	
	
	// Logistique lors de l'obstacle ligne pleine alors que le robot en est à essayer de retrouver la ligne principale
	// du circuit. En bref, le robot avance en tournant vers la gauche à partir du moment où il arrive au bout de la ligne
	// courbe. Cette partie permet de lui dire qu'il vient de détecter la ligne principale.
	if (etatRobot == ETAT_OBSTACLE_PLEIN && etapeObstacleLignePleine == LIGNE_PLEINE_RECHERCHE_LIGNE_2)
	{
		if ((PINA & 0X08) || (PINA & 0X10))
			detectionLigneFinObstacleLignePleine = true;
	}
	// Logistique de détection de la ligne dans les autres cas de rotation du robot, tel les détour à 90 degrés
	else if (((PINA & 0X01) || (PINA & 0X02) || (PINA & 0X04) || (PINA & 0X08) || (PINA & 0X10)) && detectionAucun == true)
	{
		detectionFinDetour = true;		
	}
}

/****************************************************************************
* Fonction:		codeSuiveurLigne
* Description:  Permet d'ajuster la puissance et la direction des roues selon
*				les données des capteurs suiveur de ligne
* Paramètres:	aucun
* Retour:		aucun
* Remarque:		est appelé dans alors que le robot est sur la ligne droite
*				principale du circuit, ou lorsqu'il suit la ligne courbe
*				Important à savoir : les 2 booléens de la fontion roue()
*				indique, en ordre, si les roues gauche et droite doivent avancer
*				ou reculer.
****************************************************************************/
void codeSuiveurLigne()
{	
	switch(directionCapteur)
	{				
		case TOURNER_GAUCHE :
			// à l'approche des détours à 90 degrés, le robot se tient plus à gauche, pour éviter
			// que son capteur centrale ne capte pas la ligne au mauvais moment
			if (etatRobot == ETAT_LIGNE_DROITE && (compteurObstacle == 4 ||compteurObstacle == 5))
				roue(162 + deltaV, 125 + deltaV,true,true);
			else
				roue(160 + deltaV, 127 + deltaV,true,true); 
			break;
		case TOURNER_DROITE :
			// un peu différent lorsqu'on suit la ligne courbe
			if (etatRobot == ETAT_OBSTACLE_PLEIN && etapeObstacleLignePleine == LIGNE_PLEINE_SUIVEUR_LIGNE)
				roue(127 + deltaV, 190 + deltaV,true,true);
			else
				roue(127 + deltaV, 160 + deltaV,true,true); 
			break;
		case TOURNER_GAUCHE_FORT :
			roue(180 + deltaV, 100 + deltaV,true,true); 
			break;
		case TOURNER_DROITE_FORT :
			// un peu différent lorsqu'on suit la ligne courbe
			if (etatRobot == ETAT_OBSTACLE_PLEIN && etapeObstacleLignePleine == LIGNE_PLEINE_SUIVEUR_LIGNE)
				roue(100 + deltaV, 255,true,true);
			else
				roue(100 + deltaV, 180 + deltaV,true,true);
			break;
		case AUCUN :
			//on continue l'action précédente 
			break;
		case CENTRE :	
		default:
			if (etatRobot == ETAT_LIGNE_DROITE && (compteurObstacle == 4 ||compteurObstacle == 5))
				roue(129 + deltaV, 125 + deltaV,true,true);
			else
				roue(127 + deltaV, 127 + deltaV,true,true);
			break;
	}
}

/****************************************************************************
* Fonction:		codeDetour
* Description:  Logistique lors des différents détours où le robot tourne afin
*				de retrouver la ligne (exemple, détours à 90 degrés)
* Paramètres:	bool gauche (IN) : true si le détour se fait vers la gauche
*				bool detourFinal (IN) : utilisé dans certains détour pour
*										tourner moins vite
* Retour:		aucun
* Remarque:
****************************************************************************/
void codeDetour(bool gauche, bool detourFinal)
{
	//utilisé dans certains détour pour tourner moins vite
	int8_t deltaVDetour = 0;	
	if (detourFinal)
		deltaVDetour = 20;
	
	switch(directionCapteur)
	{
		// à partir du moment que le robot ne voit que du blanc, on avance pendant
		// 200 ms, puis on démarre la rotation
		case(AUCUN):
			_delay_ms(200);
			
			if (gauche)
				roue(255,150 + deltaVDetour,true,true);
			else
				roue(150,255,true,true);
			
			detectionAucun = true;
			break;
		default:
			// si la rotation a été démarré et qu'on a détecté la ligne
			if (detectionAucun && detectionFinDetour)
			{
				// on envoie une courte impulstion afin d'aider le robot à
				// arrêter sa rotation
				if (gauche)
					roue(127,127,true,false);
				else
					roue(127,127,false,true);				
				
				_delay_ms(100);
				
				if (etatRobot == ETAT_DETOUR_90)	
				{			
					etatRobot = ETAT_LIGNE_DROITE;
					compteurObstacle++;
				}
				else if (etatRobot == ETAT_OBSTACLE_VIDE)
				{
					if (etapeObstacleLigneVide == LIGNE_VIDE_DETOUR_2)
						etapeObstacleLigneVide = LIGNE_VIDE_SUIVEUR_LIGNE;
					else if (etapeObstacleLigneVide == LIGNE_VIDE_DETOUR_FINAL)
					{
						etatRobot = ETAT_LIGNE_DROITE;
						compteurObstacle++;						
					}
				}
				else if (etatRobot == ETAT_OBSTACLE_PLEIN && etapeObstacleLignePleine == LIGNE_PLEINE_DETOUR_FINAL)
				{
					etapeObstacleLignePleine = LIGNE_PLEINE_STABILISATION;
					timer.partirMinuterie();
				}
					
				detectionFinDetour = false;
				detectionAucun = false;
			}
			// tant et aussi longtemps que les capteurs détecte quelque chose, on continue d'avancer
			// afin d'en arriver au point où ne voit plus rien
			else
				roue(127 + deltaV, 127 + deltaV,true,true);
			break;
	}
}

/****************************************************************************
* Fonction:		codeObstaclePlein
* Description:  Évolution des étapes du contournement de l'obstacle "ligne pleine"
* Paramètres:	aucun
* Retour:		aucun
* Remarque:
****************************************************************************/
void codeObstaclePlein()
{	
	switch (etapeObstacleLignePleine)
	{	
		// le robot dépasse la ligne transversale en tournant un peu vers la
		// gauche afin d'être sûr de ne pas être trop à droite de la ligne
		// principale du circuit
		case LIGNE_PLEINE_AVANCER :
		{
			roue(140,110,true,true);
			_delay_ms(800);
			detectionAucun = false;
			detectionFinDetour = false;			
			etapeObstacleLignePleine = LIGNE_PLEINE_RECHERCHE_LIGNE_1;						
			break;
		}
		
		case LIGNE_PLEINE_RECHERCHE_LIGNE_1 :
		{
			// le robot fait une rotation vers la gauche
			if (detectionFinDetour == false)
			{
				roue(255,150,true,true);
			}
			// lorsque la ligne a été trouvé
			else
			{
				// petite impulsion pour aider à stopper la rotation
				roue(127,127,true,false);				
				_delay_ms(100);	
				
				detectionFinDetour = false;
				detectionAucun = false;
				etapeObstacleLignePleine = LIGNE_PLEINE_SUIVEUR_LIGNE;
			}
			
			break;
		}
		
		// le robot suit la ligne courbe
		case LIGNE_PLEINE_SUIVEUR_LIGNE :
		{
			codeSuiveurLigne();
			break;
		}
		
		// le robot est arrivé à la fin de la ligne courbe
		case LIGNE_PLEINE_RECHERCHE_LIGNE_2 :
		{
			// il avance en tournant vers la droite
			if (detectionLigneFinObstacleLignePleine == false)
				roue(160,200,true,true);
			// jusqu'a ce qu'il trouve la ligne
			else
			{
				// il continue à avancer un peu pour dépasser la ligne
				_delay_ms(200);
				etapeObstacleLignePleine = LIGNE_PLEINE_DETOUR_FINAL;
			}
			
			break;	
		}
		
		// le robot entre en rotation pour se recenter sur la ligne principale
		case LIGNE_PLEINE_DETOUR_FINAL :
		{
			codeDetour(true, true);
			break;		
		}
		
		// mode de stabilisation, surtout utile pour l'obstacle avant le détour à 90 degrés.
		// On entre alors en mode suiveur de ligne pendant 700 ms, sauf que dans ce mode,
		// on ne tient pas compte des cas où le capteur voit 11100, 11110 ou 11111
		// Ceci évite au robot de penser qu'il est devant un détour à 90 degrés
		case LIGNE_PLEINE_STABILISATION :
		{
			if (timer.getCentiemesSecondes() < 70)
			{
				codeSuiveurLigne();				
			}
			else
			{
				timer.resetMinuterie();
				etatRobot = ETAT_LIGNE_DROITE;
				etapeObstacleLignePleine = LIGNE_PLEINE_AVANCER;
				compteurObstacle++;
			}
		}
		
		default :
			break;
	}
}

/****************************************************************************
* Fonction:		codeObstacleVide
* Description:  Évolution des étapes du contournement de l'obstacle "ligne vide"
* Paramètres:	aucun
* Retour:		aucun
* Remarque:
****************************************************************************/
void codeObstacleVide()
{
	switch (etapeObstacleLigneVide)
	{	
		// on arrive ici si les capteurs n'ont vu que du blanc pendant 300 ms
		case LIGNE_VIDE_DETOUR_1 :
		{
			// le robot s'arrête brusquement afin de ne pas foncer dans le poteau
			roue(127,127,false,false);
			_delay_ms(300);
			eteindreMoteur();
			_delay_ms(500);
			
			// impulsion afin d'aider le départ de sa rotation : permet d'obtenir
			// une rotation plus précise et évite les cas où la friction avec le sol
			// force le robot à ne pas tourner assez
			roue(50,50,false,true);
			_delay_ms(100);
			
			// rotation vers la gauche afin de s'enligner vers la ligne parallèle
			roue(127,127,false,true);
			_delay_ms(500);
			eteindreMoteur();
			_delay_ms(500);
			
			etapeObstacleLigneVide = LIGNE_VIDE_RECHERCHE_LIGNE_1;
			break;
		}
		
		// après son détour, le robot essaie de trouver la ligne parallèle
		case LIGNE_VIDE_RECHERCHE_LIGNE_1 :
		{
			// il avance tant qu'il ne l'a pas trouvé
			if (directionCapteur == AUCUN)
				roue(127,127,true,true);
			// dès qu'il a trouvé la ligne, il la dépasse un peu
			else
			{
				_delay_ms(200);
				etapeObstacleLigneVide = LIGNE_VIDE_DETOUR_2;
			}
			
			break;
		}		
		
		// le robot entre en rotation pour se recenter sur la ligne parallèle
		case LIGNE_VIDE_DETOUR_2 :
		{
			codeDetour(false, false);			
			break;
		}
		
		// il suit la ligne parallèle
		case LIGNE_VIDE_SUIVEUR_LIGNE :
		{
			codeSuiveurLigne();
			break;
		}
		
		// le robot est arrivé à la fin de la ligne parallèle
		case LIGNE_VIDE_RECHERCHE_LIGNE_2 :
		{
			// il avance en tournant vers la droite
			if (directionCapteur == AUCUN)
				roue(160,255,true,true);
			// jusqu'a ce qu'il trouve la ligne
			else
			{
				// petite impulsion pour aider à stopper la rotation
				_delay_ms(200);
				roue(127,127,false,true);
				
				_delay_ms(77);
				etapeObstacleLigneVide = LIGNE_VIDE_DETOUR_FINAL;
			}
			
			break;
		}
		
		// le robot entre en rotation pour se recenter sur la ligne principale
		case LIGNE_VIDE_DETOUR_FINAL :
		{
			codeDetour(true, true);
			break;
		}
		
		default :
			break;		
	}
}

void initialisation() 
{
	// cli est une routine qui bloque toutes les interruptions.
	// Il serait bien mauvais d'etre interrompu alors que
	// le microcontroleur n'est pas pret...
	cli ();

	// configurer et choisir les ports pour les entrees
	// et les sorties. DDRx... Initialisez bien vos variables
	
	//Pour la DEL
	DDRC |= _BV(6) | _BV(7);
	
	boutonPoussoir = 0;
	deltaV = 20;
	detectionAucun = false;
	detectionFinDetour = false;
	
	// 1 : ligne de ETAT_DEPART	
	etatRobot = ETAT_DEPART;
	compteurObstacle = 1;

	// cette procédure ajuste le registre EIMSK
	// de ATmega324PA pour permettre les interruptions externes
	EIMSK |= _BV(INT0);

	// il faut sensibiliser les interruptions externes aux
	// changement de niveau du bouton-poussoir
	// en ajustant le registre EICRA
	EICRA |= _BV(ISC01);

	// sei permet de recevoir a nouveau des interruptions.
	sei ();
}
