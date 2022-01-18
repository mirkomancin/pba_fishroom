#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;
#define ALTITUDE 0 
void setup() {
  Serial.begin(9600);
  delay(1000);
  int i=0;
  for(i=0; i<3; i++){
    Serial.println(i);
    delay(1000);
  }
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail\n\n");
    Serial.println("Check connection");
    while (1);
  }
}
void loop() {
  char status;
  double T, P, p0, a;
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      Serial.print("temperature: ");
      Serial.print(T, 2);
      Serial.println(" deg C ");
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          Serial.print("absolute pressure: ");
          Serial.print(P, 2);
          Serial.print(" hpa = ");
          Serial.print(P * 100, 2);
          Serial.print(" pa = ");
          Serial.print(P * 0.000986923, 2);
          Serial.print(" atm = ");
          Serial.print(P * 0.750063755, 2);
          Serial.print(" mmHg = ");
          Serial.print(P * 0.750061683, 2);
          Serial.print(" torr = ");
          Serial.print(P * 0.014503774, 2);
          Serial.println(" psi");
          p0 = pressure.sealevel(P, ALTITUDE); // we're at 943.7 meters
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0, 2);
          Serial.println(" hpa ");;
          a = pressure.altitude(P, p0);
          Serial.print("your altitude: ");
          Serial.print(a, 0);
          Serial.println(" meters ");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
  Serial.println("==========================================================================");
  delay(5000);
}
