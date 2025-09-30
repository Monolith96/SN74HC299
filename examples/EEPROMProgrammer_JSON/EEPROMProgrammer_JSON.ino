// Json string commands and responses: 
// READ: {
//      "cmd":"read",
//      "address":<starting-address>,
//      "length":<number-of-bytes-to-read>
//      }
//
//   RETURNS (SUCCESSFUL) {
//      "status":"ok",
//      "address":<starting-address>,
//      "length":<number-of-bytes-read>,
//      "data":[<byte-0>,<byte-1>,<byte-2>, ...]
//      }
//   RETURNS (FAILED) {
//      "status":"err",
//      "msg":"<reason>"
//      }
//
//
// WRITE: {
//      "cmd":"write",
//      "address":<starting-address>,
//      "length":<number-of-bytes-to-write>,
//      "data":[<byte-0>,<byte-1>,<byte-2>, ...]
//      }
//   RETURNS (SUCCESSFUL) {
//      "status":"ok",
//      "address":<starting-address>
//      }
//   RETURNS (FAILED) {
//      "status":"err",
//      "msg":"<reason>"
//      }

#include "SN74HC299.h"
#include <ArduinoJson.h>
#define _CS 13
#define _OE 14
#define _WE 15
#define NUMPINS 24
#define TWC 500
#define TRC 500
#define EEPROMMAX 8192
SN74HC299 SR(NUMPINS, 2, 3, 4, 5, 6);



const size_t JSON_BUFFER_SIZE = 64;  
void setup() {
  Serial.begin(921600);
  SR.begin((uint64_t)0x7 << _CS);  //Start with _CS _OE _WE set HIGH
}


void loop() {
  static String jsonString = "";
  while (Serial.available()) {
    char c = Serial.read();
    jsonString += c;
    if (c == '\n') {
      jsonString.trim();
      if (jsonString.length() > 0) {
        parseAndExecuteCommand(jsonString);
      }
      jsonString = "";
    }
  }
}

void parseAndExecuteCommand(const String& jsonStr) {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, jsonStr);

  if (error) {
    Serial.println("{\"status\":\"err\",\"msg\":\"parsefail\"}");
    return;
  }

  const char* cmd = doc["cmd"];
  if (!cmd) {
    Serial.println("{\"status\":\"err\",\"msg\":\"nocmd\"}");
    return;
  }

  if (strcmp(cmd, "write") == 0) {
    int address = doc["address"];
    JsonArray data = doc["data"].as<JsonArray>();
    if (doc["length"].as<int>() != data.size()) 
    {
      Serial.println("{\"status\":\"err\",\"msg\":\"datacountmismatch\"}");
      return;
    }
    if (address >= 0 && address < EEPROMMAX) {
      for (int i = 0; i < data.size(); i++) {
        if ((address + data.size()) < EEPROMMAX) {
          writeAt(address + i, data[i].as<uint8_t>());
        } else {
          Serial.println("{\"status\":\"err\",\"msg\":\"outofrange\"}");
        }
      }
      Serial.print("{\"status\":\"ok\",\"address\":\""); Serial.print(address); Serial.println("\"}");
    } else {
      Serial.println("{\"status\":\"err\",\"msg\":\"invaliddataoraddress\"}");
    }
  } else if (strcmp(cmd, "read") == 0) {
    int address = doc["address"];
    uint8_t length = doc["length"].as<uint8_t>();

    if (address >= 0 && address + length <= EEPROMMAX) {
      Serial.print("{\"status\":\"ok\",");Serial.print("\"address\":"); Serial.print(address); Serial.print(",\"length\":");Serial.print(length);Serial.print(",\"data\":[");
      for (int i = 0; i < length; i++) {
          Serial.print(readAt(address + i));
          if (!(i+1 == length)) Serial.print(',');
      }
      Serial.println("]}");
    } else {
      Serial.println("{\"status\":\"err\",\"msg\":\"invaliddataoraddress\"}");
    }
  } else {
    Serial.println("{\"status\":\"err\",\"msg\":\"unknowncmd\"}");
  }
}

/*void serialPinToggle() {
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
}*/
/*void dump() {
  Serial.println("Dumping EEPROM data...");
  char readbyte;
  for (int i = 0; i < 50; i += 16) {
    Serial.print("[");
    Serial.print(i, HEX);
    Serial.print("] \t");
    for (int j = 0; j < 16; j++) {
      Serial.print(readAt(i + j), HEX);
      Serial.print(" ");
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
}*/
/*void format() {
  Serial.print("Formatting chip...");
  int perc = 0;
  for (int i = 0; i < 2048; i++) {
    perc = i * (100.0f / 2047.0f);
    if ((perc % 8) == 0) {
      Serial.print(perc);
      Serial.println("%");
    }
    writeAt(i, 0xFF);
    //delay(5);
  }
  Serial.println("Format complete.");
}*/
// bit order
// 0  1  2  3  4  5  6  7  8  9  10  11  12    13  14  15    16 17 18 19 20 21 22 23
// A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 A10 A11 A12   /CS /OE /WE   D0 D1 D2 D3 D4 D5 D6 D7
uint8_t writeAt(int address, uint8_t data) {
  uint64_t buf = 0;

  SR.setByte(address, 0);
  SR.setByte(address >> 8, 8, 5);


  SR.setByte(data, 16);
  //SR.commit();
  SR.pinWrite(_CS, LOW);
  SR.pinWrite(_WE, LOW);
  delayMicroseconds(TWC);
  SR.pinWrite(_WE, HIGH, false);
  SR.pinWrite(_CS, HIGH);

  SR.setByte(0, 16);
  //SR.commit();
  return 0;
}
// bit order
// 0  1  2  3  4  5  6  7  8  9  10  11  12    13  14  15    16 17 18 19 20 21 22 23
// A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 A10 A11 A12   /CS /OE /WE   D0 D1 D2 D3 D4 D5 D6 D7
uint8_t readAt(uint16_t address) {
  uint64_t buf = 0;
  SR.setByte(0x00, 16);

  SR.setByte(address, 0);
  SR.setByte(address >> 8, 8, 5);

  SR.pinWrite(_CS, LOW, false);
  SR.pinWrite(_OE, LOW);

  delayMicroseconds(TRC);


  uint8_t data = SR.getByteAnonymous(16);
  SR.pinWrite(_OE, HIGH, false);
  SR.pinWrite(_CS, HIGH);
  return data;
}
/*void printbytes(uint64_t b) {
  for (int i = 0; i < NUMPINS; i++) {
    Serial.print(bitRead(b, (NUMPINS - 1) - i) == 1 ? 1 : 0);
  }
  Serial.println();
}*/
