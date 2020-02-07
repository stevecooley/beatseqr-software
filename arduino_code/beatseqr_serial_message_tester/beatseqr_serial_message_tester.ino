// Arduino Serial Tester
// rld, cycling'74, 3.2008

long randomvalue = 0; // random value
long countervalue = 0; // counter value
int serialvalue; // value for serial input
int started = 0; // flag for whether we've received serial yet

void setup() 
{ 
  Serial.begin(57600); // open the arduino serial port
} 


void loop() 
{ 

  Serial.println("ZPL,1;");     // play, value

  Serial.println("ZTA,0;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTA,1;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTA,2;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTA,3;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTA,4;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTA,3;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTA,2;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTA,1;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTA,0;");    // tempo adjust, value
  delay(100);
  Serial.println("ZTM,130;");   // tempo, value
  delay(100);
  Serial.println("ZOC,2;");     // octact adjust, value
  delay(100);
  Serial.println("ZSW,33;");    // swing, value
  delay(100);

  delay(100);
  Serial.println("ZPS,1;");     // pattern select, pattern number
  delay(100);
  Serial.println("ZPC,1,2;");   // pattern copy, from, to
  delay(100);
  Serial.println("ZPL,1;");     // play, value
  delay(100);
  Serial.println("ZMU,1,0;");   // mute, voice, value
  delay(100);
  Serial.println("ZSO,1,0;");   // mute, voice, value
  delay(100);

  delay(100);
  Serial.println("ZVL,1,0;");   // velocity, voice, value
  delay(100);
  Serial.println("ZRV,1,0;");   // param record velocity, voice, value
  delay(100);
  Serial.println("ZCC,1,0;");   // midi CC, voice, value
  delay(100);
  Serial.println("ZRC,1,0;");   // param record midi CC, voice, value
  delay(100);
  Serial.println("ZNN,1,0;");   // midi note number, voice, value
  delay(100);
  Serial.println("ZRN,1,0;");   // param record midi note number, voice, value
  delay(100);

  delay(100);
  Serial.println("ZMC,1,0;");   // midi channel, voice, value
  delay(100);

  delay(100);
  Serial.println("ZSD,1,7,01,1;");
  delay(100);
  Serial.println("ZSD,1,7,01,0;");
  delay(100);
  Serial.println("ZSD,2,1,15,0;");
  delay(100);
  Serial.println("ZSD,2,1,15,1;");


  delay(5000); // pause

  Serial.println("ZPL,0;");     // play, value

  delay(5000); // pause


} 





