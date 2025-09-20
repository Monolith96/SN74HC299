
This basic library expands Arduino I/O lines up to 64 pins using 74HC299 IC (8x cascaded)
and allows for both read and write operations taking only 5 pins on the MCU.</br?
also offering expanded bit/byte manipulation with less effot.

### 4x Cascaded HC299s:</br>
![copy_7B411B78-0BCC-41CA-9921-27DE1CC53947(1)](https://github.com/user-attachments/assets/a520a8b1-6a09-43f3-8d23-76bba41a02a1)

### EEPROM Programmer example output
<img width="1551" height="915" alt="74299_EEPROMPROG_DMP" src="https://github.com/user-attachments/assets/5b06b8b3-acc7-4df6-99d8-b4b799aa3b68" />


# USAGE:
please read the [schematics](#cascading-schematic) first *carefully*.

> [!IMPORTANT]
> for any setup, whether there is one register or eight registers cascaded, the *Q<sub>0</sub>* of the first HC299 must be connected directly to *DSR* pin on the same chip.

> [!IMPORTANT]
> for more advanced projects which need output state retention during each pinWrite cycle (there is a short all-out state on extended pins while writing on chip due to lack of internal latch in the HC299) one can add a D-Type Flip-Flop for pins that require it and attach *CP* (Clock Pulse) of the Flip-Flop to *#OE<sub>1</sub>* (Output Enable) of the HC299.


> [!NOTE]
> for basic pin state manipulation, you can use `pinWrite()` and `pinRead()` functions with default arguments.


| Command | Description | Returns |
| --- | --- | --- |
| `SN74HC299(uint8_t numPins, uint8_t pinClock, uint8_t pinOutputEnable, uint8_t pinS0, uint8_t pinS1, uint8_t pinData)` | Class Cunstructor</br>`numPins` : number of pins.</br>`pinClock` : *Clock* pin on HC299.</br>`pinOutputEnable` : *#OE<sub>1</sub>* pin on HC299.</br>`pinS0` : *S0* pin on HC299.</br>`pinS1` : *S1* pin on HC299</br>`pinData` : *DSR* and *Q<sub>0</sub>* pin on HC299.</br> **Important!: check the schematics down this page.**  | - |
| `void begin()` | initializes the library with all pins set to `0` | - |
| `void begin(uint64_t initialData)` | initializes the library with all pins set according to bits from `initialData` argument | - |
| `uint64_t read(bool updateReg = false)` | reads digtal state of all pins at once. </br>`updateReg` : whether to update virtual register data with the new data read from physical pins. | pin states |
| `void write(uint64_t data, bool updateReg =  false)` | writes digital state of all pins at once. </br>`data` : data to be written in bit level. </br>`updateReg` : whether to update virtual register data with the given data.| - |
| `void pinWrite(uint8_t pin, uint8_t value, bool writePhysical = true)`| similar to digitalWrite().</br>`pin` : pin number.</br>`value` : pin state to set.</br>`writePhysical` : whether to write the data physical pin or just write to the register. | - |
| `uint8_t pinRead(uint8_t pin, bool readPhysical = true)` | reads digital state of the specified pin.</br>`pin` : pin number</br>`readPhysical` : if *true*, the digital state present on the physical pin is reported, and if *false*, the value from the virtual register is reported. | pin state (`0` or `1`) |
| `uint64_t getPinDataReg()`| returns virtual register data at once. | virtual register data |
| `void setPinDataReg(uint64_t data)` | sets virtual register data an once.</br>`data` : data to be applied. | - | 
| `void commit()` | commits the virtual register data to the physical pins. | - |
| `void setByte(uint8_t buf, uint8_t index, uint8_t length = 8)` | takes a byte and applies it to the virtual register on the specified position.</br>`buf` : the data byte buffer.</br>`index` : starting index.</br>`length` : number of bits effective. | - |
| `uint8_t getByte(uint8_t index, uint8_t length = 8)` | returns a byte of data from virtual register.</br>`index` : starting index.</br>`length` : number of bits effective. | a byte of data with `length` valid bits |
| `uint8_t getByteAnonymous(uint8_t index, uint8_t length = 8)` | returns a byte of data from physical pins starting from specified pin position without affecting virtual register. useful when working with Memory chips.</br>`index` : starting index.</br>`length` : number of bits effective. | a byte of data with `length` valid bits |
| `void setPortion (uint8_t buf[], uint8_t index, uint8_t length)` | takes an array of bits and applies them to the virtual register.</br>`buf[]` : the data buffer.</br>`index` : starting index.</br>`length` : length of the buffer. | - |


# Cascading schematic:
<img width="2073" height="835" alt="299-CASCADED" src="https://github.com/user-attachments/assets/d4b1b450-ec58-4095-bb4e-a098df80d5da" />

## EEPROM-Programmer example schematic:
<img width="1031" height="869" alt="74299_EEPROMPROG_SCH" src="https://github.com/user-attachments/assets/d03b5088-0baa-4910-ab7d-a51023b668d5" />


### Controlling a 1602 LCD and reading 2 inputs using one HC299 and one Flip-Flop taking 5 pins on Arduino.
![image](https://github.com/user-attachments/assets/4b5c4c4d-476f-4f72-95d2-790c58572885)

here's a demo video on
[Youtube](https://www.youtube.com/watch?v=-GAqLGjc2NI)
