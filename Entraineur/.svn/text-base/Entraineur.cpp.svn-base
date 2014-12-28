/****************************************************************************
* Fichier: Entraineur.cpp
* Auteur: Maxime Perusse, Anas Balboul, Nam Nguyen & Kim Piché
* Date: Avril 2014
* Description: Programme main de l'entraineur
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
#include "../includes/son.h"
#include "../includes/capteur_distance.h"
#include "../includes/lcm_so1602dtr_m_fw.h"
#include "../includes/customprocs.h"
#include "../includes/can.h"

#define DEMO_DDR	DDRC // Data Direction Register' AVR occupé par l'aff.
#define DEMO_PORT	PORTC // Port AVR occupé par l'afficheur

using namespace std;

// Enum du capteur
enum capteurIR
{
	Gauche,
	GaucheCentre,
	Centre,
	Droite,
	DroiteCentre,
	Tout,
	Rien
};

capteurIR directionCapteur; // Création d'un objet capteurIR

// Enum des états du trajet
enum deplacementRobot
{
	aller,
	transfert,
	retour,
	aller2,
	stop
};

uint16_t secondes; // Secondes qui seront affichées sur le LCD à la fin
uint16_t minutes; // Minutes qui seront affichées sur le LCD à la fin
char doublepoint =':'; // Charactère à afficher sur le LCD à la fin

timer_materiel chrono; // Création d'un objet timer_matériel, le chronomètre

/****************************************************************************
* Fonction:	ISR(TIMER2_COMPA_vect)
* Description:  Interruption pour le chronomètre
* Paramètres:	TIMER2_COMPA_vect : l'interruption pour timer2
* Remarque:     Routine d'interruption
****************************************************************************/
ISR(TIMER2_COMPA_vect)
{	
	// Augmenter les secondes à chaque cent centième de seconde
	if (chrono.getCentiemesSecondes() == 99) 
	{
		chrono.incSecondes();
	}
	else
		chrono.incCentiemesSecondes();	
	
	TCNT2 = 0;
}

volatile uint16_t compteurSon; // Création du compteur pour le son

/****************************************************************************
* Fonction:	ISR(TIMER0_COMPB_vect)
* Description:  Interruption pour le son
* Paramètres:	TIMER0_COMPB_vect : l'interruption pour timer0b
* Remarque:     Routine d'interruption
****************************************************************************/
ISR(TIMER0_COMPB_vect)
{	
	// on utilise la note 80 : 830Hz
	// on est en mode toggle
	// donc 830 fois 2 cycles pour 1 seconde
	if (compteurSon >= 830 * 2)
	{
		sonOff();
	}
	else
		compteurSon++;
}

/****************************************************************************
* Fonction:	high_level() 
* Description:  Genere un signal cadencé qui oscille a 38kHz
****************************************************************************/
void high_level() 
{
	TCNT0 = 0; // le timer

	OCR0A = 104; // le registre de comparaison avec le timer 0

	// mode CTC du timer 0 avec horloge divisee par 1
	// interruption après la duree specifiee
	TCCR0A |= _BV(COM0A0) | _BV(WGM01); 

	TCCR0B |= _BV(CS00);

	TIMSK0 |= _BV(OCIE0A);
}

/****************************************************************************
* Fonction:	low_level() 
* Description:  Arrete le timer et mets les registres a leur valeur par defaut
*				C'est comme envoyer un signal a frequence nulle
****************************************************************************/
void low_level()
{
	TIMSK0 = 0;	// desactiver les interruptions
	TCCR0A = 0; // registres avec valeurs par defaut
	TCCR0B = 0;
	PORTB &= ~_BV(4); // met le pin de la sortie de OC0B a 0
}


/****************************************************************************
* Fonction:	send_one() 
* Description:  Envoi un 1 selon le protocole RC5
****************************************************************************/
void send_one() 
{
	low_level();  
	_delay_us(842);
	high_level();
	_delay_us(842);
}

/****************************************************************************
* Fonction:	send_zero() 
* Description: Envoi un 0 selon le protocole RC5
****************************************************************************/
void send_zero()
{
	high_level();
	_delay_us(842);
	low_level();
	_delay_us(842);
}

/****************************************************************************
* Fonction:	gauche(const deplacementRobot &etatRobot, bool &toutOKcompteur, bool detection)
* Description:  fonction apellee lorsque le capteur infrarouge n'a que 
*					son capteur de gauche actif (la ligne se trouve a la gauche du robot)
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				bool detection : presence ou non de l'athlete a ses cotes
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void gauche(const deplacementRobot &etatRobot, bool &toutOKcompteur, bool detection)
{
	int8_t deltaV = 0; // variable qui modifie la vitesse du robot
	
	if (detection == false) // si l'athlete est derriere
		deltaV = 60; // entraineur ralentit, pour se laisser rejoindre
	else  // si l'athlete est devant
		deltaV = -7; // entraineur accelere, pour le depasser
	
	// Mode aller 
	if((etatRobot==aller) || (etatRobot==aller2))
		roue(100 + deltaV, 150 + deltaV, true, true); // tourne legerement a droite
		
	// Mode retour
	else if(etatRobot==retour)
		roue(150 + deltaV, 100 + deltaV, true, true); // tourne legerement a gauche
		
	// Compteur	
	toutOKcompteur=true; // une incrementation de compteur est possible
}

/****************************************************************************
* Fonction:	droite(const deplacementRobot &etatRobot, bool &toutOKcompteur, bool detection)
* Description:  fonction apellee lorsque le capteur infrarouge n'a que son 
*					capteur de droite actif (la ligne se trouve a la droite du robot)
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				bool detection : presence ou non de l'athlete a ses cotes
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void droite(const deplacementRobot &etatRobot, bool &toutOKcompteur, bool detection)
{
	int8_t deltaV = 0; // variable qui modifie la vitesse du robot
	
	if (detection == false) // si l'athlete est derriere
		deltaV = 60; // entraineur ralentit, pour se laisser rejoindre
	else  // si l'athlete est devant
		deltaV = -7; // entraineur accelere, pour le depasser
	
	//Mode aller 
	if(etatRobot==aller || etatRobot==aller2)
		roue(100 + deltaV, 150 + deltaV, true, true); // tourne legerement a droite
		
	//Mode retour	
	else if(etatRobot==retour)
		roue(150 + deltaV, 100 + deltaV,true,true); // tourne legerement a gauche
		
	// Compteur
	toutOKcompteur=true; // une incrementation de compteur est possible
}

/****************************************************************************
* Fonction:	rien(const deplacementRobot &etatRobot, bool &toutOKcompteur, bool detection)
* Description:  fonction apellee lorsque le capteur infrarouge n'a aucun 
*					capteur actif (le robot ne trouve plus la ligne)
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				bool detection : presence ou non de l'athlete a ses cotes
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void rien(const deplacementRobot &etatRobot, bool &toutOKcompteur, bool detection)
{
	int8_t deltaV = 0; 	// variable qui modifie la vitesse du robot
	
	if (detection == false) // si l'athlete est derriere
		deltaV = 60; // entraineur ralentit, pour se laisser rejoindre
	else  // si l'athlete est devant
		deltaV = -7; // entraineur accelere, pour le depasser
	
	// Mode aller
	if(etatRobot==aller || etatRobot==aller2) 
		roue(130 + deltaV,100 + deltaV,true,true); // tourne legerement a droite

	// Mode retour
	else if(etatRobot==retour)
		roue(100 + deltaV, 130 + deltaV,true,true); // tourne legerement a gauche
		
	// Compteur
	toutOKcompteur=true; // une incrementation de compteur est possible
}

/****************************************************************************
* Fonction:	centre(const deplacementRobot &etatRobot, bool &toutOKcompteur, bool detection)
* Description:  fonction apellee lorsque le capteur infrarouge n'a que son 
*					capteur du centre actif (le robot se trouve sur la ligne)
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				bool detection : presence ou non de l'athlete a ses cotes
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void centre(const deplacementRobot &etatRobot, bool &toutOKcompteur, bool detection)
{
	int8_t deltaV = 0; // variable qui modifie la vitesse du robot
	
	if (detection == false) // si l'athlete est derriere
		deltaV = 60; // entraineur ralentit, pour se laisser rejoindre
	else  // si l'athlete est devant
		deltaV = -7; // entraineur accelere, pour le depasser
	
	// Mode aller
	if(etatRobot==aller || etatRobot==aller2)
		roue(90 + deltaV, 150 + deltaV, true, true); // tourne rapidement a gauche
	
	// Mode retour
	else if(etatRobot==retour)
		roue(150 + deltaV, 90 + deltaV,true,true); // tourne rapidement a droite
		
	// Compteur
	toutOKcompteur=true; // une incrementation de compteur est possible
}

/****************************************************************************
* Fonction:	gaucheCentre(const deplacementRobot &etatRobot, bool &toutOKcompteur, uint8_t &compteur)
* Description:  fonction apellee lorsque le capteur infrarouge a le 
*					capteur du centre et celui de gauche actif (le robot se troube legerement a droite de la ligne)
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				uint8_t &compteur : compteur permettant de connaitre la position du robot sur la ligne
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void gaucheCentre(const deplacementRobot &etatRobot, bool &toutOKcompteur, uint8_t &compteur)
{
	// Mode aller
	if(etatRobot==aller || etatRobot==aller2)
		roue(110,160,true,true); // tourne legerement a gauche
		
	// Mode retour	
	else if(etatRobot==retour)
		roue(120,110,true,true); // tourne legerement a droite
}

/****************************************************************************
* Fonction:	droiteCentre(const deplacementRobot &etatRobot, bool &toutOKcompteur, uint8_t &compteur)
* Description:  fonction apellee lorsque le capteur infrarouge a le 
*					capteur du centre et celui de droite actif (le robot se trouve legerement a droite de la ligne)
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				uint8_t &compteur : compteur permettant de connaitre la position du robot sur la ligne
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void droiteCentre(const deplacementRobot &etatRobot, bool &toutOKcompteur, uint8_t &compteur)
{
	// Mode aller
	if(etatRobot==aller || etatRobot==aller2)
		roue(110,120,true,true); // tourne legerement a droite
		
	// Mode retour	
	else if(etatRobot==retour)
		roue(160,110,true,true); // tourne legerement a gauche
}

/****************************************************************************
* Fonction:	tout(const deplacementRobot &etatRobot, bool &toutOKcompteur, uint8_t &compteur)
* Description:  fonction apellee lorsque le capteur infrarouge a les 3 
*					capteurs actif (le robot se sur une ligne d'incrementation)
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				uint8_t &compteur : compteur permettant de connaitre la position du robot sur la ligne
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void tout(const deplacementRobot &etatRobot, bool &toutOKcompteur, uint8_t &compteur)
{
	// Mode aller et retour
	roue(120,120,true,true); // avance tout droit
	
	// Compteur
	if(toutOKcompteur==true) // on s'assure que le capteur infrarouge du tick precedent n'etait pas un tout,
	{
		toutOKcompteur=false; // evite l'incrementation infinie
		compteur++;
	}	 
}

/****************************************************************************
* Fonction:	tourner180(const deplacementRobot &etatRobot, bool &toutOKcompteur, uint8_t &compteur)
* Description:  fonction apellee lorsque le capteur infrarouge a les 3 
*					capteurs actif (le robot se sur une ligne d'incrementation)
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				uint8_t &compteur : compteur permettant de connaitre la position du robot sur la ligne
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void tourner180(deplacementRobot &etatRobot, bool &toutOKcompteur, uint8_t &compteur)
{
	toutOKcompteur = true; // incrementation possible
	
	roue(120,120,true,true);
	_delay_ms(1100); // depasser la ligne darrivee pour tourner ensuite
	
	//Mode aller
	if(etatRobot == aller){
		roue(150,5,true,false); // un petit pousse pour roue droite
		_delay_ms(500);
		while((PINA & 0x40))
		{
			roue(160,160,true,false); 
		}
		roue(150,150,false,true); //repositionnement
		_delay_ms(200);		
		roue (50,50,true,true);
		_delay_ms(100);
		while(PINA & 0x04)
			roue (150,150,true,true);
		_delay_ms(400);	
			
		eteindreMoteur();
		
		etatRobot = transfert; // le robot est pret pour le transfert de donnees a l'athlete
	}
	
	//Mode retour
	else if(etatRobot==retour){
		roue(5,150,false,true); // un petit pousse pour roue gauche
		_delay_ms(500);
		while((PINA & 0x04))
		{
			roue(160,160,false,true);
		}
		roue(150,150,true,false); //repositionnement
		_delay_ms(200);		
		roue (50,50,true,true);
		_delay_ms(100);		
		etatRobot = aller2; // le robot est dans son dernier sprint!
	}// fin else if		
		
	
	compteur=0; // on recommence le trajet sur la ligne, on reinitialise donc le compteur de position
}

/****************************************************************************
* Fonction:	deplacementDepart(const deplacementRobot &etatRobot)
* Description:  fonction apellee au debut du trajet. Vu que le robot doit etre centré au départ,
*					cette fonction s'assure qu'il se positionne a doire de la ligne
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void deplacementDepart(const deplacementRobot &etatRobot)
{
	while(!(PINA & 0x04) && !(PINA & 0x10) && !(PINA & 0x40)) // tout
		roue (100,100,true,true); // avance
	
	_delay_ms(50);
	
	while(!(PINA & 0x01) && !(PINA & 0x10) && !(PINA & 0x40)) // tout
		roue (100,100,true,true); // avance
	
	if(etatRobot==aller || etatRobot==aller2){
		while(PINA & 0x04) // si non gauche
			roue(130,255,true,true); // tourner a droite

		while(!(PINA & 0x04)) // si gauche
			roue(130,130,true,true); // avancer droit
		}
	else if(etatRobot==retour){
		while(PINA & 0x40) // si non droite
			roue(255,140,true,true); // tourner a gauche

		while(!(PINA & 0x40)) // si droite
			roue(130,130,true,true); // avancer droit
	}
}

/****************************************************************************
* Fonction:	testCapteur(bool &toutOKcompteur, uint8_t &compteur,deplacementRobot &etatRobot, uint8_t bits[], bool detection)
* Description:  fonction qui regarde l'etat du capteur infrarouge
* Paramètres:	deplacementRobot &etatRobot : l'etat present du robot
*				bool &toutOKcompteur : variable qui s'assure que l'incrementation du compteur de ligne se fait bien
*				uint8_t &compteur : compteur permettant de connaitre la position du robot sur la ligne
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void testCapteur(bool &toutOKcompteur, uint8_t &compteur,deplacementRobot &etatRobot, uint8_t bits[], bool detection)
{	
	if(compteur!=6)
	{
			if(!(PINA & 0x04)&& (PINA & 0x10) && (PINA & 0x40))
			{
				gauche(etatRobot, toutOKcompteur, detection);
			}
				
			else if((PINA & 0x04)&& !(PINA & 0x10) && (PINA & 0x40))
			{
				centre(etatRobot, toutOKcompteur, detection);
			}
			else if((PINA & 0x04)&& (PINA & 0x10) && !(PINA & 0x40))
			{
				droite(etatRobot, toutOKcompteur, detection);
			}
			else if(!(PINA & 0x04)&& !(PINA & 0x10) && (PINA & 0x40))
			{
				gaucheCentre(etatRobot, toutOKcompteur, compteur);
			}
			else if((PINA & 0x04)&& !(PINA & 0x10) && !(PINA & 0x40))
			{
				droiteCentre(etatRobot, toutOKcompteur, compteur);
			}
			else if(!(PINA & 0x04)&& !(PINA & 0x10) && !(PINA & 0x40))
			{
				tout(etatRobot, toutOKcompteur, compteur);
			}
			else if((PINA & 0x04)&& (PINA & 0x10) && (PINA & 0x40))
			{
				rien(etatRobot, toutOKcompteur, detection);
			}
	}
	else if(etatRobot == aller || etatRobot == retour)
		tourner180(etatRobot,toutOKcompteur, compteur);
	else if (etatRobot == aller2)
		etatRobot = stop;
}

/****************************************************************************
* Fonction:	detectionPoteau(uint8_t &compteur, char &test,	char &test2, uint8_t obstacles[])
* Description:  fonction qui determine la position des obstacles
* Paramètres:	char &test : character a afficher sur le LCD pour s'assurer que le robot a bien capté le premier obstacle
*				char &test2 : character a afficher sur le LCD pour s'assurer que le robot a bien capté le deuxieme obstacle
*				uint8_t obstacles[] :
*				uint8_t &compteur : compteur permettant de connaitre la position du robot sur la ligne
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
void detectionPoteau(uint8_t &compteur, char &test,	char &test2, uint8_t obstacles[])
{
	capteur_distance_GP2D12 gp2d12;
	if(compteur==4 && obstacles[1]==0) { // position du deuxieme couple d'obstacle (a la 4e incrementation de ligne pleine)
		obstacle poteau12 = gp2d12.detectionObstable(); // on detecte l'obstacle
	if (poteau12 == PRES) { // par defaut, l'obstacle est loin, on change la valeur seulement s'il est pres
		obstacles[1]=1; // le poteau est pres
		}
	}
	if(compteur==2 && obstacles[0]==0) { // position du premier couple d'obstacle (a la 2e incrementation de ligne pleine)
		obstacle poteau34 = gp2d12.detectionObstable(); // on detecte l'obstacle
	if (poteau34 == PRES) { // par defaut, l'obstacle est loin, on change la valeur seulement s'il est pres
		obstacles[0]=1; // le poteau est pres
		}
	}
	
	//test
	if (obstacles[0]) 
		test='P'; // P pour pres
	else 
		test='L'; // L pour loin
				
	if (obstacles[1]) 
		test2='P';
	else
		test2='L';
}

/****************************************************************************
* Fonction:	detectionAthlete(bool pres)
* Description:  fonction qui determine si l'athlete est a cote de l'entraineur et 
*					qui determine si l'entraineur doit utiliser son capteur 
*					de distance de gauche ou de droite pour suivre l'athlete
* Paramètres:	bool pres : vrai -> l'athlete est a gauche, faux -> l'athlete est a droite
* Retour:       bool : vrai -> l'athlete est au meme niveau que l'entraineut, faux -> l'entraineur a de l'avance
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
bool detectionAthlete(bool pres)
{
	if (pres)
	{
		capteur_distance_GP2D120 gp2d120;
		return gp2d120.detectionRobotPres();
	}
	else
	{
		capteur_distance_GP2D12 gp2d12;
		return gp2d12.detectionRobotLoin();
	}
}

/****************************************************************************
* Fonction:	main()
* Description:  fonction principale de l'entraineur
* Remarque:     Notre robot ne suit pas la ligne avec le capteur du milieu, 
*					il est constamment a droite de la ligne et la "cogne" continuellement
****************************************************************************/
int main()
{
	DDRA |= _BV(3) | _BV(5);
	DDRB |= _BV(1) | _BV(3) | _BV(4) | _BV(5);
	DDRC = 0xff; // broche C en entier en mode sortie pour l'écran
	//DDRD est géré par la fonction roues
	
	char test;	// tests d'obstacles
	char test2;
	 
	LCM disp(&DEMO_DDR, &DEMO_PORT); //objet disp pour affichage LCD
 
	capteur_distance_GP2D120 gp2d120;
	capteur_distance_GP2D12 gp2d12;
	bool detection=true; // bool pour choisir le capteur
	bool toutOKcompteur=true; // bool pour activer compteur
	
	
	uint8_t compteur=0; // le compteur representant la position du robot sur son trajet
	uint8_t obstacles[2]={0,0}; // true pour P2-P1 (premiers poteaux pour athlete, deuxieme pour entraineur)
								   // et P4-P3 (deuxieme pour athlete, premier pour entraineur)
								   // true pcq plus difficile de capter les poteaux loins (true pour loin, false pour pres)
								   
uint8_t bits[14] = {1, 1, 				  // bits de depart
					   0, 				  // bit de toggle
					   0, 1, 0, 0, 1, 	  // bits d'adresses
					   1, 0, 0, 0, 0, 1}; // bits de donnees
					
	roue(255,255,true,true); // moteur eteint au depart
	_delay_ms(1000);
	uint8_t compteurPresent=0;
	
	deplacementRobot etatRobot = aller; // etat initial est l'aller
	deplacementDepart(etatRobot); // positionnment a droite de la ligne
	
	for(;;)
	{		
		switch(etatRobot) // choisir la detection (capteur gauche si robot pres, capteur droit si robot loin ou stop)
		{
			case(retour):
							
				detection = detectionAthlete(true);				
				
				testCapteur(toutOKcompteur, compteur,etatRobot, bits, detection);
				
				if(compteur != compteurPresent)
					disp<<compteur; // affichage pour s'assurer du bon fonctionnement
				compteurPresent=compteur;
			break;
			
			case(aller2):
				detection=detectionAthlete(false);
				
				testCapteur(toutOKcompteur, compteur,etatRobot, bits, detection);
				
				if(compteur != compteurPresent)
					disp<<compteur;  // affichage pour s'assurer du bon fonctionnement
				compteurPresent=compteur;
				break;
			
			case (transfert):
				disp << test << " " << test2; // affichage de la position des obstacles avant la transmission
				_delay_ms(1500);
				disp.clear();
				disp<<"Transmission";
				
				bits[12] = obstacles[1]; // le 12e bit transmit est la position du premier couple d'obstacle
				bits[13] = obstacles[0]; // le 13e bit transmit est la position du deuxieme couple d'obstacle
				PORTA|= _BV(5);
				
				//transmission				
				for (int i = 0; i < 20; i++) // on envoie 20 fois le message pour s'assurer que l'athlete a bien recu le message
					for(int j = 0; j < 14;j++)
					{
						if (bits[j] == 0)
							send_zero();
						else
							send_one();
					}		
			
				low_level();
					
				//déclencher le chronometre apres la transmission
				sei(); // on recommence a capter les interruptions
				compteurSon = 0;
				sonOn(80); // joue une note aigue pendant une seconde
				
				chrono.partirMinuterie(); // demarre le chronometre pour le parcours de l'athlete qui commence		
				toutOKcompteur=true;				
				compteur=1;
				disp.clear(); // ferme l'affichage
				etatRobot = retour; // position du robot est retour
				break;
				
			case(stop):
				eteindreMoteur(); 
				do{}
				while(detectionAthlete(false)==false); // tant que l'athlete n'est pas arrivé, on n'arrete pas le chronometre
				
				sei(); // on recommence a capter les interruptions
				compteurSon = 0;
				sonOn(80); // joue une note aigue pendant une seconde

				PORTA&= ~(_BV(5)); // fermer la led

				// affichage des minutes et des secondes sur le LCD
				chrono.resetMinuterie(); // termine le chronometre
				secondes = chrono.getSecondes();
				minutes=(secondes-(secondes%60))/60;
				secondes=secondes-(minutes*60);
				while(1) 
				{
					disp.clear();
					if(secondes<=9) // affichage sous forme MM:SS
						disp <<"0"<< minutes << doublepoint <<"0"<< secondes; 
					else
						disp <<"0"<< minutes << doublepoint << secondes;
					_delay_ms(500);
				}			
				break;
				
			case(aller):
				// Detection des poteaux
				detectionPoteau(compteur, test,	test2, obstacles);
								
				// Deplacement du robot 
				testCapteur(toutOKcompteur, compteur,etatRobot, bits, true);
				
				if(compteur != compteurPresent)
					disp<<compteur; // affichage du compteur pour s'assurer du bon fonctionnement
				compteurPresent=compteur;
				break;
				
			default:
				
			break;
		}
	
	}
	return 0;		
} 
