double flow; //Liters of passing water volume
double flow_total; //Liters of passing water volume
unsigned long currentTime;
unsigned long lastTime;
unsigned long pulse_freq;
unsigned long pulse_freq_total;
const byte interruptPin = 22;

void setup()
{
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), pulse, RISING); 
  currentTime = millis();
  lastTime = currentTime;
}

void loop ()
{
  currentTime = millis();
  // Every second, calculate and print L/Min
  if(currentTime >= (lastTime + 1000))
  {
     lastTime = currentTime; 
     pulse_freq_total += pulse_freq;
     // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
     flow = (pulse_freq / 7.5); 
     flow_total = (pulse_freq_total / 7.5); 
     pulse_freq = 0; // Reset Counter
     Serial.print(flow, DEC); 
     Serial.print(" L/Min # ");
     Serial.print(flow_total, DEC); 
     Serial.println(" L");
  }
}

void pulse () // Interrupt function
{
  pulse_freq++;
}
