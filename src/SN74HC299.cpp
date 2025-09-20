#include "pins_arduino.h"
#include <stdint.h>
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
  pinDataReg = 0;  //reset all pins
  output_Disable();
  dw(pCP, L);  //set clock to LOW
  commit();
}
void SN74HC299::begin(uint64_t initialData) {
  pinMode(pCP, OUTPUT);
  pinMode(pOE, OUTPUT);
  pinMode(pS0, OUTPUT);
  pinMode(pS1, OUTPUT);
  pinMode(pD0, OUTPUT);
  pinDataReg = initialData;  //set all pins according to the given data
  output_Disable();
  dw(pCP, L);  //set clock to LOW
  commit();
}
///@brief reads digital state of the specified pin
///@param pin : pin number
///@param readPhysical : if true, the digital state present on the physical pin is reported, and if false, the value from the virtual register is reported.
uint8_t SN74HC299::pinRead(uint8_t pin, bool readPhysical) {
  if (pin > _numPins) {
    return 127;
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
///@param writePhysical : whether to write the data physical pin or just write to the register.
void SN74HC299::pinWrite(uint8_t pin, uint8_t value, bool writePhysical) {
  if (pin >= _numPins) {
    return;
  }

  if (value == 1) {
    pinDataReg |= (uint64_t)1 << pin;  //bitwrite works for values under 32 bits
  } else {
    pinDataReg &= ~((uint64_t)1 << pin);
  }
  if (writePhysical) write(pinDataReg);
}
///@brief takes an array of bits and applies them to the virtual register.
///@param buf[] : the data buffer.
///@param index : starting index.
///@param length : length of the buffer.
void SN74HC299::setPortion(uint8_t buf[], uint8_t index, uint8_t length) {
  if ((index + length) >= _numPins) {
    return;
  }
  for (int i = 0; i < length; i++) {
    pinWrite(index + i, buf[i], false);
  }
}
///@brief takes a byte and applies it to the virtual register on the specified position.
///@param buf : the data byte buffer.
///@param index : starting index.
///@param length : number of bits effective.
void SN74HC299::setByte(uint8_t buf, uint8_t index, uint8_t length) {
  if ((index + length) > _numPins || length > 8) {
    return;
  }

  for (int i = 0; i < length; i++) {
    pinWrite(index + i, bitRead(buf, i), false);
  }
}

///@brief returns a byte of data from virtual register.
///@param index : starting index.
///@param length : number of bits effective.
uint8_t SN74HC299::getByte(uint8_t index, uint8_t length) {
  if ((index + length) > _numPins || length > 8) {
    return;
  }
  uint8_t retVal = 0;
  for (int i = 0; i < length; i++) {

    retVal |= pinRead(index + i, false) << i;
  }
  return retVal;
}

///@brief returns a byte of data from physical pins starting from specified pin position without affecting virtual register.
/// useful when working with Memory chips.
///@param index : starting index.
///@param length : number of bits effective.
uint8_t SN74HC299::getByteAnonymous(uint8_t index, uint8_t length) {
  if ((index + length) > _numPins || length > 8) {
    return;
  }
  uint8_t retVal = 0;
  uint64_t buf = read();
  retVal = (buf >> index);

  return retVal;
}

///@brief returns virtual register data at once.
uint64_t SN74HC299::getPinDataReg() {
  return pinDataReg;
}
///@brief sets virtual register data an once.
///@param data : data to be applied.
void SN74HC299::setPinDataReg(uint64_t data) {
  pinDataReg = data;
}
///@brief commits the virtual register data to the physical pins.
void SN74HC299::commit() {
  write(pinDataReg, true);
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

  //dus(usDelay);
  //lower clock
  dw(pCP, L);
  //dus(usDelay);
}

///@brief write to the pins at once
///@param data : data to be written in bit level.
///@param updateReg : whether to update virtual register data with the given data.
void SN74HC299::write(uint64_t data, bool updateReg) {
  if (updateReg) pinDataReg = data;
  //set chip mode to Shift Right
  mode_ShiftRight();

  //disable output
  output_Disable();

  //shift data out
  for (int word = ((_numPins / 8) - 1); word >= 0; word--) {
    shiftOut(pD0, pCP, MSBFIRST, data >> (word * 8));
  }

  //the other way around
  //for (int idx = _numPins - 1; idx >= 0; idx--) {
  //  // set data line to bit(n) value
  //  dw(pD0, bitRead(data, idx));
  //
  //  clock(0);
  //}

  //enable output
  output_Enable();
  output_Disable();
  output_Enable();
}

///@brief reads digtal state of all pins at once.
///@param updateReg : whether to update virtual register data with the data read from pins.
///@return pin states
uint64_t SN74HC299::read(bool updateReg) {

  output_Disable();
  //load mode
  mode_Load();
  clock(0);
  //shift
  mode_ShiftLeft();
  uint64_t retval = 0;
  //shift data in

  for (int i = 0; i < _numPins; i++) {
    //after the first clock after mode_Load(), first bit is already present at pQ7
    if (digitalRead(pD0))
      retval |= (uint64_t)1 << i;
    else
      retval &= ~((uint64_t)1 << i);
    clock(0);
  }
  if (updateReg) {
    pinDataReg = retval;
  } else {
    return retval;
  }
  return retval;
}
