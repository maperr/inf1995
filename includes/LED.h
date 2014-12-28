//Énumération pour faciliter l'affection des valeurs du port B pour
//choisir la couleur du LED.
//Il faut utiliser les broches B0 et B1 pour la LED. Lorsque B0 est à 1,
//la lumière est rouge.
//Warrning: lisez bien les commentaires car ces fonctions peuvent mettre le PORTB en mode sortie
enum couleur
{
  ETEINT, //0
  ROUGE,  //1
  VERT,    //2
  AMBRE	  //3
};
//Description:  Allumer la led d'une certaine couleur pour quelques secondes (Warrning: met le PIN 0 du PORTB en mode sortie)
void LED(unsigned int couleur, unsigned int ms);
//Description:  Allumer la led (Warrning: met le PIN 0 du PORTB en mode sortie)
void allumerLED();
//Description:  Fermer la led (Warrning: met le PIN 0 du PORTB en mode sortie)
void eteindreLED();
