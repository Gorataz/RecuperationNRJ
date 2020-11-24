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

Servo  Arduino
GND -> GND (noir ou marron)
5V -> 5V (rouge)
Signal -> 6 (orange ou blanc)

*/
#include<Servo.h>
#include <SPI.h>
#include <NRFLite.h>

Servo myServo;//déclaration du servomoteur


const static uint8_t RADIO_ID = 0;       // Numéro d'indentification de notre module récepteur
const static uint8_t PIN_RADIO_CE = 9; //déclaration du pin CE
const static uint8_t PIN_RADIO_CSN =10;//déclaration du pin CSN

struct RadioPacket // Packet de données envoyé,
//uint8_t = variable sur 8 bit, uint16_t=variable sur 16 bit, uint32_t=variable sur 32 bit
{
    uint32_t Valeur;//variable Valeur codée sur 32 bit
};

NRFLite _radio; //Déclaration du module NRF
RadioPacket _radioData;//Déclaration du packet de données

void setup()
{
    Serial.begin(115200);//On ouvre la laison série avec le PC à 115200 baud

    myServo.attach(6);//Déclaration du pin du servomoteur

    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))//si l'initialisation du module n'est pas finie
    {
        Serial.println("L'arduino ne peut pas communiquer avec le module ");
        while (1); // Attend
    }
    Serial.println("Setup fini");
    
    /*
    Par défaut la vitesse est de 2mb/s sur la channel 100/125
    L'émetteur et le recepteur doivent etre sur la même channel et avoir la même vitesse pour bien communiquer
    
    Il est possible de changer ces paramtres avec les commandes:    
    
    _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)//250kB/s channel 0
    _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)//1 MB/s channel 75
    _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // par défaut
    */
}

void loop()
{
    while (_radio.hasData())//Tant que le module NRF a des données
    {
        _radio.readData(&_radioData); // lecture des nouvelles données 

        myServo.write(_radioData.Valeur);//Le servo prend la valeur recue
        Serial.println(_radioData.Valeur);//afficher sur lepc la valeur recue
        
    }
}
