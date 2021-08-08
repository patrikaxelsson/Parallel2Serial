# Parallel2Serial

Converts a centronics style parallel port to a serial port, output only.
Created for getting an alternative debug output on an Amiga, when the serial
port is occupied, but should work in with any computer with a centronics
style parallel port.

## Hardware

Uses the same connection scheme/pinout as a [plipbox](https://github.com/cnvogelg/plipbox/blob/master/doc/src/hardware.md)
based on an Arduino Mini or Nano, so a plipbox can be flashed with this
firmware:

| Par num | Par name | Arduino | AVR | Description                                              |
| ------- | -------- | ------- | --- | -------------------------------------------------------- |
| 1       | /STROBE  | D3      | PD3 | Low indicates that data is available from the Amiga.     |
| 2       | D0       | A0      | PC0 |                                                          | 
| 3       | D1       | A1      | PC1 |                                                          | 
| 4       | D2       | A2      | PC2 |                                                          | 
| 5       | D3       | A3      | PC3 |                                                          | 
| 6       | D4       | A4      | PC4 |                                                          | 
| 7       | D5       | A5      | PC5 |                                                          | 
| 8       | D6       | D6      | PD6 |                                                          | 
| 9       | D7       | D7      | PD7 |                                                          | 
| 10      | /ACK     | D8      | PB0 | Low indicates data has been read by the AVR.             |
| 11      | BUSY     | D4      | PD4 | High indicates AVR is not ready to read more data.       |
| 12      | POUT     | D5      | PD5 | Not used.                                                |
| 13      | SEL      | D3      | PB1 | High indicates to the Amiga that something is connected. |
| 17-22   | GND      | GND     | GND |                                                          |

## Software

The serial port is set to 57600bps by the firmware, not configurable
currently, to change the speed a recompile is needed. Reasons for 57600 is
simply that the plipbox uses the same speed, the AVR can produce this
baudrate with quite small error and it is reasonably fast for what this
project currently is intended for - debug output.

