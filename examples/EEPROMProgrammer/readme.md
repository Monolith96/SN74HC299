Serial Output screenshot:
<img width="1551" height="915" alt="74299_EEPROMPROG_DMP" src="https://github.com/user-attachments/assets/7c28943c-276b-4c4a-91bf-4a64ec1b6875" />

> [!NOTE]
> if you do not want to use flip-flops and bus transceivers, EEPROM control lines should be driven directly via the MCU. this way the "all-out" state during register write cycle will not affect the EEPROM since #WE #OE #CE pins are not in the expansion pins.


Here's the schematics for this example:
<img width="1031" height="869" alt="74299_EEPROMPROG_SCH" src="https://github.com/user-attachments/assets/696059a0-77c8-4d41-8175-730de68ac3fa" />
