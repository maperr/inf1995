inf1995
=======
Le code de l'athlete se trouve dans le dossier Athlete
Le code de l'entraineur se trouve dans le dossier Entraineur
Les librairies se trouvent dans le dossier lib_dir
Les fichiers.h se trouvent dans le dossier includes

Arrangement des PORTS (voir le .ods pour une versio plus claire):

Entraineur:

PORT A
GP2D12			1	2	GP2D120
Suiveur Left		3	4	DEL positive
Suiveur Middle		5	6	DEL negative
suiveur Right		7	8	
GP2D120 ground		GND	VCC	GP2D120 vcc

PORT B
			1	2	Transmission Ground
			3	4	Transmission
Son			5	6	Son Ground
			7	8	
Suiveur Ground		GND	VCC	Suiveur vcc

PORT C
LCD			1	2	LCD
LCD			3	4	LCD
LCD			5	6	LCD
LCD			7	8	LCD
LCD			GND	VCC	LCD
			
		
PORT D	
			1	2	moteur droit direction
			3	4	
Moteur gauche pwm	5	6	moteur droit pwm
moteur gauche direction	7	8	Chronomètre
GP2D12 ground		GND	VCC	GP2D12 vcc






Athlete:

PORT A
suiveur (extreme gauche)	1	2	suiveur (gauche)
suiveur (centre)		3	4	suiveur (droite)
suiveur (extreme droite)	5	6	
Magnet data			7	8	
suiveur ground			GND	VCC	suiveur VCC
			
PORT B					
Transmission reception		1	2	
				3	4	
				5	6	
				7	8	
Transmission Ground		GND	VCC	Transmission VCC
			
PORT C					
				1	2	
				3	4	
				5	6	
DEL positive			7	8	DEL negative
Magnet ground			GND	VCC	Magnet VCC
			
PORC D			
				1	2	moteur droit direction
int0 bouton poussoir		3	4	
Moteur gauche pwm		5	6	moteur droit pwm
moteur gauche direction		7	8	timer pour ligne vide
				GND	VCC	





TIMERS:
PORT A
	1	2	
INT3	3	4	
	5	6	
	7	8	
	GND	VCC	
			
PORT B					
	1	2	
	3	4	OC0A
OC0B	5	6	
	7	8	
	GND	VCC	
			
PORT C					
	1	2	
	3	4	
	5	6	
	7	8	
	GND	VCC	
			
PORT D					
	1	2	
INT0	3	4	INT1
OC1B	5	6	OC1A
	7	8	OC2A
	GND	VCC	


