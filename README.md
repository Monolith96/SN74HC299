This basic library expands Arduino I/O lines up to 64 pins using 74HC299 IC (8x cascaded)
and allows for both read and write operations.

4 Cascaded HC299s:
![copy_7B411B78-0BCC-41CA-9921-27DE1CC53947(1)](https://github.com/user-attachments/assets/a520a8b1-6a09-43f3-8d23-76bba41a02a1)

Cascading schematic:
<img width="2073" height="835" alt="299-CASCADED" src="https://github.com/user-attachments/assets/d4b1b450-ec58-4095-bb4e-a098df80d5da" />

for more advanced projects which need output state retention during each pinWrite cycle (there is a short all-out state on extended pins whilw writing on chip due to lack of internal latch in the HC299) one can add a D-Type Flip-Flop for pins that require it and attach CP (Clock Pulse) of the Flip-Flop to OE (Output Enable) of the HC299.

![image](https://github.com/user-attachments/assets/4b5c4c4d-476f-4f72-95d2-790c58572885)

here's a demo video on
[Youtube](https://www.youtube.com/watch?v=-GAqLGjc2NI)
