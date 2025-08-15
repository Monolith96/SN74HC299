This basic library expands Arduino I/O lines up to 64 pins using 74HC299 IC (8x cascaded)
and allows for both read and write operations.

for more advanced projects which need output state retention during each pinWrite cycle (there is a short all-out state on extended pins whilw writing on chip due to lack of internal latch in the HC299) one can add a D-Type Flip-Flop for pins that require it and attach CP (Clock Pulse) of the Flip-Flop to OE (Output Enable) of the HC299.

![image](https://github.com/user-attachments/assets/4b5c4c4d-476f-4f72-95d2-790c58572885)
