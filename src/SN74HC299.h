/*
  74HC299 SHIFT REGISTER LIBRARY FOR ARDUINO
  This library uses the common SN74HC299 Shift Register IC to for I/O expansion up to 64 lines (8 cascaded HC299 ICs)
  Before using, on HC299  you need to pull PIN2 (~OE1) and PIN9 (~MR) HIGH using a 1Kohm resistor
  and conncet PIN8 (Q0) to PIN11 (D0/Dsr) then to the desired Arduino pin (here we've used Pin6)
  so:
    [HC299]
    1 (S0)            ->  ARDUINO 4
    2 (~OE1)          ->  HIGH
    3 (~OE2)          ->  ARDUINO 3
    4 (I/O6)          ->  I/O
    5 (I/O4)          ->  I/O
    6 (I/O2)          ->  I/O
    7 (I/O0)          ->  I/O
    8 (Q0)            ->  ARDUINO 6
    9 (~MR)           ->  HIGH
    10 (GND)          ->  GROUND
    11 (D0)           ->  ARDUINO 6
    12 (CP)           ->  ARDUINO 2
    13 (I/O1)         ->  I/O
    14 (I/O3)         ->  I/O
    15 (I/O5)         ->  I/O
    16 (I/O7)         ->  I/O
    17 (Q7)           ->  NC
    18 (D7)           ->  NC
    19 (S1)           ->  ARDUINO 5
    20 (+5v)          ->  VCC (5v)

*/


#include "Arduino.h"
#ifndef SN74HC299_h
#define SN74HC299_h

#define H 0x1
#define L 0x0

#define dw(p, x) digitalWrite(p, x)
#define dus(x) delayMicroseconds(x)


class SN74HC299 {
public:
  SN74HC299(uint8_t numPins, uint8_t pinClock, uint8_t pinOutputEnable, uint8_t pinS0, uint8_t pinS1, uint8_t pinData);
  void begin();
  uint64_t read();
  void write(uint64_t data);
  void pinWrite(uint8_t pin, uint8_t value);
  uint8_t pinRead(uint8_t pin, bool readPhysical);
private:
  void mode_ShiftLeft();
  void mode_ShiftRight();
  void mode_Hold();
  void mode_Load();
  void output_Enable();
  void output_Disable();
  void clock(uint8_t);


  uint8_t _numPins, pCP, pOE, pS0, pS1, pD0;
  /// 0=Left 1=hold 2=right 3=load
  uint8_t shiftMode = 0;
  uint64_t pinDataReg;
};
#endif