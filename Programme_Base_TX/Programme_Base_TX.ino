/*

Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> Rien
VCC   -> 3.3V
GND   -> GND

Capteur Temp
DATA  -> 3
VCC   -> 5V
GND   -> GND
*/


#include <SPI.h>
#include <NRFLite.h>
#include <OneWire.h>

byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
float celsius;

const static uint8_t RADIO_ID = 1;              // Numéro d'indentification de notre module émetteur
const static uint8_t DESTINATION_RADIO_ID = 0;  // Numéro d'identification du module récepteur (A qui notre émetteur transmet)
const static uint8_t PIN_RADIO_CE = 9;          //déclaration du pin CE
const static uint8_t PIN_RADIO_CSN = 10;        //déclaration du pin CSN

int Valeur;                                     //variable qui contient la valeur transmise

struct RadioPacket                              // Packet de données envoyé,
/*uint8_t=variable sur 8 bit, uint32_t=variable sur 32 bit*/
{
    uint32_t Valeur;                            //variable Valeur codée sur 32 bit
};

NRFLite _radio;                                 //Déclaration du module NRF
RadioPacket _radioData;                         //Déclaration du packet de données

OneWire  ds(3);  // on pin 3


void setup()
{
    Serial.begin(115200);                       //On ouvre la laison série avec le PC à 115200 baud
    
    pinMode(12,OUTPUT);
       
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))//si l'initialisation du module n'est pas finie
    {
        Serial.println("L'arduino ne peut pas communiquer avec le module ");
        while (1); // Attend
    }
    Serial.println("Setup fini");
    
}

void loop()
{
   if (!ds.search(addr)) 
  {
    //Serial.println("No more addresses.");
    ds.reset_search();
    delay(250);
    return;
  }
  
  //Serial.print("ROM =");
  /*
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
  */

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      //Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return;
  } 
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) 
  {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) 
    {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } 
  else 
  {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  Serial.print(" Temperature = ");
  Serial.print(celsius);
  Serial.println(" °C");

  delay(200); 
  //duree de la trame de 22.00ms
  celsius=celsius*100;
  

  /*
   * TRANSMISSION RADIO
   */
    _radioData.Valeur=celsius;
   // Serial.println(celsius);
    //Serial.println(_radioData.Valeur);//Affiche la valeur envoyée
    _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData),NRFLite::NO_ACK); //Si le module récepteur confirme qu'il a bien recu le message
    delay(3000);//pause de 3 secondes
}
