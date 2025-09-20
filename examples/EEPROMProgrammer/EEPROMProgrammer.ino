//this sketch is meant for demonstrating the new capabilities of the library with an EEPROM Programmer project.
//you need three HC299s, two HC574s, and one HC(T)245 for this to work properly. 

#include "SN74HC299.h"
#define _CS 13
#define _OE 14
#define _WE 15
#define NUMPINS 24
#define TWC 2
#define TRC 2

SN74HC299 SR(NUMPINS, 2, 3, 4, 5, 6);
void setup() {
  Serial.begin(9600);

  SR.begin( (uint64_t)0x7 << _CS ); //Start with _CS _OE _WE set HIGH

  SR.pinWrite(_WE, HIGH);
  SR.pinWrite(_OE, HIGH);
  SR.pinWrite(_CS, HIGH);

}
void serialPinToggle() {
  if (Serial.available() > 0) {

    int val = Serial.parseInt();
    if (val == -1) {
      printbytes(SR.read());
      return;
    }
    Serial.print("toggling pin ");
    Serial.println(val);
    Serial.print("current virtual value: ");
    Serial.println(SR.pinRead(val, false));
    Serial.print("current actual value: ");
    Serial.println(SR.pinRead(val, true));

    if (SR.pinRead(val, false) == 1) {
      SR.pinWrite(val, 0);
    } else {
      SR.pinWrite(val, 1);
    }
  }
}
void dump() {
  Serial.println("Dumping EEPROM data...");
  char readbyte;
  for (int i = 0; i < 50; i+=16) {
    Serial.print("[");Serial.print(i, HEX); Serial.print("] \t");
    for (int j = 0; j < 16; j++) {
      Serial.print(readAt(i + j), HEX); Serial.print(" ");
    }
    Serial.print("\t");
    for (int j = 0; j < 16; j++) {
      readbyte = (char)readAt(i + j);
      Serial.print('.');
      if (isAlphaNumeric(readbyte)) {
        Serial.print((char)readAt(i + j)); 
      } else {
        Serial.print(' ');
      }
    }
    Serial.println();
  }
}
void format() {
  Serial.print("Formatting chip...");
  int perc=0;
  for ( int i = 0; i < 2048; i++) {
    perc = i * (100.0f / 2047.0f);
    if ((perc % 8) == 0) {
      Serial.print(perc);Serial.println("%");
    } 
    writeAt(i, 0xFF);
    //delay(5);
  }
  Serial.println("Format complete.");
}
void loop() {
  if (Serial.available()) {
    String p = Serial.readString();
    if (p.startsWith("w")) {
      uint16_t addr = p.substring(1, p.indexOf(' ')).toInt();
      uint8_t data = p.substring(p.indexOf(' ') + 1).toInt();
      Serial.print("writing ");
      Serial.print(data);
      Serial.print(" at ");
      Serial.println(addr);
      writeAt(addr, data);
    } else if (p.startsWith("r")) {
      uint16_t addr = p.substring(1).toInt();
      uint8_t data = readAt(addr);
      Serial.print("retreived ");
      Serial.print(data);
      Serial.print(" from ");
      Serial.println(addr);
    } else if (p.startsWith("F")) {
      Serial.print("Confirm to Format chip? y-n ");
      bool noreply = true;
      while (noreply) {
        if (Serial.available()) {
          
          String c = Serial.readString();
          c.toUpperCase();
          if (c.startsWith("Y")) {
            //do format
            noreply = false;
            format();

          } else if (c.startsWith("N")) {
            //abort
            Serial.println("Format aborted.");
          } else {
            //unknown reply
            Serial.println("Invalid option. aborted.");
            noreply = false;
          }
        } else {
          noreply = true;
        }
        

      }
    } else if (p.startsWith("D")) {
      //dump data
      dump();
    }
  }
  delay(10);
}
// bit order
// 0  1  2  3  4  5  6  7  8  9  10  11  12    13  14  15    16 17 18 19 20 21 22 23
// A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 A10 A11 A12   /CS /OE /WE   D0 D1 D2 D3 D4 D5 D6 D7  
uint8_t writeAt(int address, uint8_t data) {
  uint64_t buf=0;

  SR.setByte(address, 0);
  SR.setByte(address >> 8, 8, 5);


  SR.setByte(data, 16);
  SR.commit();
  SR.pinWrite(_CS, LOW);
  SR.pinWrite(_WE, LOW);
  delay(TWC);
  SR.pinWrite(_WE, HIGH);
  SR.pinWrite(_CS, HIGH);

  SR.setByte(0, 16);
  SR.commit();
  return 0;

}
// bit order
// 0  1  2  3  4  5  6  7  8  9  10  11  12    13  14  15    16 17 18 19 20 21 22 23
// A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 A10 A11 A12   /CS /OE /WE   D0 D1 D2 D3 D4 D5 D6 D7  
uint8_t readAt(uint16_t address) {
  uint64_t buf=0;
  SR.setByte(0x00, 16);

  SR.setByte(address, 0);
  SR.setByte(address >> 8, 8, 5);

  SR.pinWrite(_CS, LOW);
  SR.pinWrite(_OE, LOW);

  delay(TRC);


  uint8_t data = SR.getByteAnonymous(16);
  SR.pinWrite(_OE, HIGH);
  SR.pinWrite(_CS, HIGH);
  return data;

}
void printbytes(uint64_t b) {
  for (int i = 0; i < NUMPINS; i++) {
    Serial.print(bitRead(b, (NUMPINS - 1) - i) == 1 ? 1 : 0);
  }
  Serial.println();
}
