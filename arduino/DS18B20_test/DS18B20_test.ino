// librerie per il funzionamento del sensore
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2 // Pin Arduino a cui colleghiamo il pin DQ del sensore
const int pinLed = LED_BUILTIN; // Utilizzo del LED su scheda 

const int soglia = 25; // Accende il LED su scheda se si superano i 25°C

OneWire oneWire(ONE_WIRE_BUS); // Imposta la connessione OneWire

DallasTemperature sensore(&oneWire); // Dichiarazione dell'oggetto sensore

void setup(void)
{
  Serial.begin(9600);       // Inizializzazione della serial monitor
  sensore.begin();          // Inizializzazione del sensore

  // Stampa del messaggio di avvio
  Serial.println("Temperatura rilevata dal sensore DS18B20");
  Serial.println("----------------------------------------");
  delay(1000);
}

void loop()
{
  sensore.requestTemperatures(); // richiesta lettura temperatura
  
  float celsius = sensore.getTempCByIndex(0);
   
  Serial.print("C:");
  Serial.println(celsius);
  
  delay(1000);
}
