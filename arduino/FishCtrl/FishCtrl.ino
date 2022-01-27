//BMP180
#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;
#define ALTITUDE 0 


//KS0429
#define TdsSensorPin A1
#define VREF 5.0 // analog reference voltage(Volt) of the ADC
#define SCOUNT 30 // sum of sample point

int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;

//DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2 
OneWire oneWire(ONE_WIRE_BUS); // Imposta la connessione OneWire
DallasTemperature ds18b20(&oneWire); // Dichiarazione dell'oggetto sensore


//FLOWMETER
double flow1; 
double flow1_total; 
unsigned long currentTime;
unsigned long lastTime;
unsigned long pulse_freq1;
unsigned long pulse_freq1_total;
const byte interruptPin1 = 22;

double flow2; 
double flow2_total; 
unsigned long pulse_freq2;
unsigned long pulse_freq2_total;
const byte interruptPin2 = 23;



void setup()
{
  Serial.begin(9600);
  delay(3000);
  Serial.println("START:OK");
  delay(3000);
  pinMode(TdsSensorPin, INPUT);
  if (pressure.begin())
    Serial.println("OK:BMP180 and KS0429 init success");
  else
  {
    Serial.println("ERR:BMP180 init fail\n\n");
    Serial.println("ERR:Check connection");
    while (1)
    {
      delay(1000);
      Serial.println("ERR:InfiniteLoop.Reset");
      if (pressure.begin()){
        Serial.println("OK:BMP180 init success");
        break;
      }
    }
  }
  ds18b20.begin();

  pinMode(interruptPin1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin1), pulse1, RISING); 
  currentTime = millis();
  lastTime = currentTime;

  pinMode(interruptPin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), pulse2, RISING);

  delay(1000);
  pulse_freq1 = 0;
  pulse_freq2 = 0;
  pulse_freq1_total = 0;
  pulse_freq2_total = 0;
}



float conduc=0.0f, dstemp=0.0f, temper=0.0f, mbar=0.0f;

void loop()
{
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();

    if(incomingByte=='R'){
      pulse_freq1 = 0;
      pulse_freq2 = 0;
      pulse_freq1_total = 0;
      pulse_freq2_total = 0;  
    }
  }
  
  currentTime = millis();
  // Every second, calculate and print L/Min
  if(currentTime >= (lastTime + 1000))
  {
     lastTime = currentTime; 
     flow1 = (pulse_freq1 / 7.5); 
     flow1_total = (pulse_freq1_total * .00225); 
     pulse_freq1 = 0;

     flow2 = (pulse_freq2 / 7.5); 
     flow2_total = (pulse_freq2_total * .00225); 
     pulse_freq2 = 0;
  }
  
  
  //KS0429
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin); //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U)
  {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVolatge = averageVoltage / compensationCoefficient; //temperature compensation
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; //convert voltage value to tds value
    //Serial.print("voltage:");
    //Serial.print(averageVoltage,2);
    //Serial.print("V ");
    //Serial.print("us:");
    //Serial.println(tdsValue, 0);
    conduc = tdsValue;
  }


  //BMP180
  static unsigned long printBMP180 = millis();
  if (millis() - printBMP180 > 5000U)
  {
    printBMP180 = millis();
    char status;
    double T, P, p0, a;
    status = pressure.startTemperature();
    if (status != 0)
    {
      delay(status);
      status = pressure.getTemperature(T);
      if (status != 0)
      {
        //Serial.print("degC:");
        //Serial.println(T, 2);
        temper = T;
        delay(100);
        status = pressure.startPressure(3);
        if (status != 0)
        {
          delay(status);
          status = pressure.getPressure(P, T);
          if (status != 0)
          {
            mbar = P * 0.000986923 * 1013.25;
          }
          else Serial.println("ERR:pressure measurement\n");
        }
        else Serial.println("ERR:starting pressure measurement\n");
      }
      else Serial.println("ERR:retrieving temperature measurement\n");
    }
    else Serial.println("ERR:starting temperature measurement\n");
  }


  static unsigned long printDS18B20 = millis();
  if (millis() - printDS18B20 > 5000U)
  {
    printDS18B20 = millis();
    ds18b20.requestTemperatures();
    float celsius = ds18b20.getTempCByIndex(0);
    dstemp = celsius;
  }

  static unsigned long printerSerial = millis();
  if (millis() - printerSerial > 5000U)
  {
    printerSerial = millis();
    Serial.print("LOG:");
    Serial.print(conduc,2);
    Serial.print("#");
    Serial.print(dstemp,2);
    Serial.print("#");
    Serial.print(temper,2);
    Serial.print("#");
    Serial.print(mbar,0);
    Serial.print("#");
    Serial.print(flow1, 2); 
    Serial.print("#");
    Serial.print(flow1_total, 2); 
    Serial.print("#");
    Serial.print(flow2, 2); 
    Serial.print("#");
    Serial.print(flow2_total, 2); 
    
    Serial.println();
    
    pulse_freq1 = 0; 
    pulse_freq2 = 0; 
  }
}



//FUNCTIONS
//KS0429
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}


void pulse1() 
{
  pulse_freq1++;
  pulse_freq1_total++;
}

void pulse2() 
{
  pulse_freq2++;
  pulse_freq2_total++;
}
