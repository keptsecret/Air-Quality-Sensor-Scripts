/* Grove - Dust Sensor Demo v1.0
Interface to Shinyei Model PPD42NS Particle Sensor
Program by Christopher Nafis
Written April 2012
http://www.seeedstudio.com/depot/grove-dust-sensor-p-1050.html
http://www.sca-shinyei.com/pdf/PPD42NS.pdf
JST Pin 1 (Black Wire) => Arduino GND
JST Pin 3 (Red wire) => Arduino 5VDC
JST Pin 4 (Yellow wire) => Arduino Digital Pin 8
*/

int pin = 5;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sample 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

void setup() {
  Serial.begin(115200);
  pinMode(5,INPUT);
  starttime = millis();//get the current time;
}

void loop() {
  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) > sampletime_ms) { //if the sample time == 30s
    ratio = lowpulseoccupancy/(sampletime_ms*10.0); // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
    Serial.println(ratio);
    Serial.print("concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    lowpulseoccupancy = 0;
    starttime = millis();
  }
}
