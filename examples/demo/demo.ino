//SN74HC299 LIBRARY DEMO SKETCH 

#include "SN74HC299.h"

SN74HC299 SR(8,2,3,4,5,6);
void setup() {
  SR.begin();
  Serial.begin(9600);
  initanim();
}

void loop() {
  //uncomment one of these for demo
  serialPinToggle(); //toggling pins (virtually) by sending pin number.
  //serialReadWrite(); //reading bus data by sending -1, and writing a number by sending a positive value.
}

void serialPinToggle(){
    if (Serial.available() > 0) {
      
    int val = Serial.parseInt();

    Serial.print("toggling pin "); Serial.println(val);
    Serial.print("current virtual value: "); Serial.println(SR.pinRead(val, false));
    Serial.print("current actual value: "); Serial.println(SR.pinRead(val, true  ));
    
    if (SR.pinRead(val, false) == 1) {
      SR.pinWrite(val, 0);
    } else {
      SR.pinWrite(val, 1);
    }
  }
}
void serialReadWrite() {
  if (Serial.available() > 0) {
    int val = Serial.parseInt();
    Serial.println(val);
    byte bval = (byte)val;
    if (val == -1) {
      //read
      byte readval = SR.read();
      Serial.print("Value read was: -");

      for (byte idx = 0; idx < 8; idx++) {
        Serial.print(bitRead(readval, idx));
      }
      Serial.print("-  ");
      Serial.println(readval);
      return;
    }
    Serial.print("writing -");
    for (byte idx = 0; idx < 8; idx++) {
      Serial.print(bitRead(bval, idx));
    }
    Serial.println("-");
  }
}

//============================animations and tests==========================

void testread(){
  Serial.println("\nperforming read 01001010 [82] error test ...");
  byte retval=0; int total=300, errors=0;
  for (int i=total; i>0; i--) {
    if(SR.read() != 82) {
      errors++;
    }
  }
  Serial.print("tested "); Serial.print(total); Serial.print(" times with "); Serial.print(errors); Serial.println(" errors.");

}

void initanim() {
  for (byte times = 1; times < 3; times++) {
    for (byte idx = 0; idx < 8; idx++) {
      SR.write(1 << idx);
      delay(20);
    }
    for (byte idx = 0; idx < 8; idx++) {
      SR.write(128 >> idx);
      delay(20);
    }
  }
  byte tblink = 50, tstall = 250;
  for (byte idx = 1; idx < 4; idx++) {

    SR.write(240);
    delay(tblink);

    SR.write(0);
    delay(tblink);

    SR.write(240);
    delay(tblink);

    SR.write(0);
    delay(tblink);

    SR.write(240);
    delay(tblink);

    SR.write(0);
    delay(tstall);

    SR.write(15);
    delay(tblink);

    SR.write(0);
    delay(tblink);

    SR.write(15);
    delay(tblink);

    SR.write(0);
    delay(tblink);

    SR.write(15);
    delay(tblink);

    SR.write(0);
    delay(tstall);
  }

  SR.write(255);
  for (byte idx = 1; idx < 4; idx++) {
    for (int fade = 255; fade > 0; fade--) {
      analogWrite(3, fade);
      delay(5);
    }
    for (int fade = 0; fade < 255; fade++) {
      analogWrite(3, fade);
      delay(5);
    }
    delay(300);
  }
  SR.write(0);
}
void pwm() {
  for (int i = -32000; i < 3200; i++) {
    SR.write(255);
    dus(3);
    SR.write(0);
    dus(9);
  }
}
void test() {
  Serial.println("starting timing test");
  unsigned long start = millis();
  for (int i = 0; i < 256; i++) {
    SR.write((byte)i);
  }
  Serial.print("256 write cycles took ");
  Serial.print(micros() - start);
  Serial.println("ms");

  start = micros();
  for (int i = 0; i < 1000; i++) {
    SR.write((byte)i);
    //delay(10);
  }
  Serial.print("1000 write cycles took ");
  Serial.print(micros() - start);
  Serial.println("ms");
}

