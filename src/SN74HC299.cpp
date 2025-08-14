#include "SN74HC299.h"
#include "Arduino.h"



SN74HC299::SN74HC299(uint8_t numPins, uint8_t pinClock, uint8_t pinOutputEnable, uint8_t pinS0, uint8_t pinS1, uint8_t pinData) {
  _numPins = numPins;
  pCP = pinClock;
  pOE = pinOutputEnable;
  pS0 = pinS0;
  pS1 = pinS1;
  pD0 = pinData;
}
void SN74HC299::begin() {
  pinMode(pCP, OUTPUT);
  pinMode(pOE, OUTPUT);
  pinMode(pS0, OUTPUT);
  pinMode(pS1, OUTPUT);
  pinMode(pD0, OUTPUT);
  pinDataReg = 0;
  output_Disable();
  dw(pCP, L); //set clock to LOW
  write(0); //reset all pins
}

///@brief reads digital state of the specified pin
///@param pin : pin number
///@param readPhysical : if true, the digital state present on the physical pin is reported, and if false, virtual value is reported which was set with digitalWrite.
uint8_t SN74HC299::pinRead(uint8_t pin, bool readPhysical) {
  if (pin > _numPins) {
    return;
  }
  if (readPhysical) {
    uint64_t val = read();
    return bitRead(val, pin);
  } else {
    return bitRead(pinDataReg, pin);
  }
}
///@brief similar to digitalWrite
///@param pin : pin number.
///@param value : pin state to set.
void SN74HC299::pinWrite(uint8_t pin, uint8_t value) {
  if (pin > _numPins) {
    return;
  }

  if (value)
    pinDataReg |= (uint64_t)1 << pin;  //bitwrite works for values under 32 bits
  else
    pinDataReg &= ~((uint64_t)1 << pin);  //bitwrite works for values under 32 bits
  //value &= ~((uint64_t)1 << bitPosition);


  write(pinDataReg);
}
void SN74HC299::mode_ShiftRight() {
  if (shiftMode != 2) {
    shiftMode = 2;
    pinMode(pD0, OUTPUT);
  }
  dw(pS1, L);
  dw(pS0, H);

  //dus(1);
}
void SN74HC299::mode_ShiftLeft() {
  if (shiftMode != 0) {
    shiftMode = 0;
    pinMode(pD0, INPUT);
  }
  dw(pS1, H);
  dw(pS0, L);
  shiftMode = 0;
  //dus(1);
}
void SN74HC299::mode_Hold() {
  dw(pS1, L);
  dw(pS0, L);
  shiftMode = 1;
  //dus(1);
}
void SN74HC299::mode_Load() {
  dw(pS1, H);
  dw(pS0, H);
  shiftMode = 4;
  //dus(1);
}
void SN74HC299::output_Enable() {
  dw(pOE, L);
  //dus(1);
}
void SN74HC299::output_Disable() {
  dw(pOE, H);
  //dus(1);
}
void SN74HC299::clock(byte usDelay) {
  // raise clock
  dw(pCP, H);
  dus(usDelay);
  //lower clock
  dw(pCP, L);
  dus(usDelay);
}

///@brief write to the pins at once
///@param data : data to be written in bit level.
void SN74HC299::write(uint64_t data) {
  //set chip mode to Shift Right
  mode_ShiftRight();

  //disable output
  output_Disable();

  for (int idx = _numPins - 1; idx >= 0; idx--) {
    // set data line to bit(n) value
    dw(pD0, bitRead(data, idx));

    clock(0);
  }
  //enable output
  output_Enable();
}

///@brief reads all of the pins
///@return pin states
uint64_t SN74HC299::read() {

  output_Disable();
  //load mode
  mode_Load();
  clock(1);
  //shift
  mode_ShiftLeft();
  byte retval = 0;
  //shift
  for (int i = 0; i < _numPins; i++) {
    //after the first clock after mode_Load(), first bit is already present at pQ7
    bitWrite(retval, i, digitalRead(pD0));
    clock(1);
  }
  return retval;
}

