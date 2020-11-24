/*

Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> Rien
VCC   -> En dessous de 3.6 volts
GND   -> GND


Potentiomètre Arduino
Pin de droite -> 5V
Pin du millieu -> A0
Pin de gauche -> GND
*/


#include <SPI.h>
#include <NRFLite.h>

const static uint8_t RADIO_ID = 1;             // Numéro d'indentification de notre module émetteur
const static uint8_t DESTINATION_RADIO_ID = 0; // Numéro d'identification du module récepteur (A qui notre émetteur transmet)
const static uint8_t PIN_RADIO_CE = 9;//déclaration du pin CE
const static uint8_t PIN_RADIO_CSN = 10;//déclaration du pin CSN

int Valeur;//variable qui contient la valeur transmise

struct RadioPacket // Packet de données envoyé,
//uint8_t=variable sur 8 bit, uint32_t=variable sur 32 bit
{
    uint32_t Valeur;//variable Valeur codée sur 32 bit
};

NRFLite _radio; //Déclaration du module NRF
RadioPacket _radioData;//Déclaration du packet de données

void setup()
{
    Serial.begin(115200);//On ouvre la laison série avec le PC à 115200 baud

       
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))//si l'initialisation du module n'est pas finie
    {
        Serial.println("L'arduino ne peut pas communiquer avec le module ");
        while (1); // Attend
    }
    Serial.println("Setup fini");
    
}

void loop()
{
     _radioData.Valeur=analogRead(A0);//lecture de la valeur du potentiometre
     //donne une valeur entre 0 et 1023
     _radioData.Valeur = map(_radioData.Valeur, 0, 1023, 0, 180);//produit en croix pour avoir une valeur d'angle pour le servo entre 0° et 180°
    
    Serial.println(_radioData.Valeur);//Affiche la valeur envoyée
    
  if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) //Si le module récepteur confirme qu'il a bien recu le message
    {
        Serial.println("...Bien arrivé");
    }
    
    else
    {
        Serial.println("...erreur");
    }

    delay(10);//pause de 10 milli secondes
    
    /*
    Par défaut la commande _radio.send transmet les données puis attend pour un accusé de réception
    
    Il est possible de changer cette condition avec ces commandes :
    
    _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)//pas d'accusé de réception
    _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // par défaut , accusé de réception
    */
}
