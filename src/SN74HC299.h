#include <stdint.h>
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
  void begin(uint64_t initialData);
  uint64_t read(bool updateReg = false);
  void write(uint64_t data, bool updateReg =  false);
  void pinWrite(uint8_t pin, uint8_t value, bool writePhysical = true);
  uint8_t pinRead(uint8_t pin, bool readPhysical = true);
  uint64_t getPinDataReg();
  void setPinDataReg(uint64_t data);
  void commit();
  void setByte(uint8_t buf, uint8_t index, uint8_t length = 8);
  uint8_t getByte(uint8_t index, uint8_t length = 8);
  uint8_t getByteAnonymous(uint8_t index, uint8_t length = 8);
  void setPortion (uint8_t buf[], uint8_t index, uint8_t length);
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

