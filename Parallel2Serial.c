/*
Converts a centronics style parallel port to a serial port, for output only.
Created for use as an alternative debug output on an Amiga when the serial
port is occupied, but should work in with any computer with a centronics
style parallel port.

Uses the same connection scheme/pinout as a plipbox based on an Arduino Mini
or Nano, so a plipbox can be flashed with this firmware:
https://github.com/cnvogelg/plipbox/blob/master/doc/src/hardware.md

Par num  Par name  Arduino  AVR  Description
1        /STROBE   D3       PD3  Low indicates that data is available from the Amiga.
2        D0        A0       PC0
3        D1        A1       PC1
4        D2        A2       PC2
5        D3        A3       PC3
6        D4        A4       PC4
7        D5        A5       PC5
8        D6        D6       PD6
9        D7        D7       PD7
10       /ACK      D8       PB0  Low indicates data has been read by the AVR.
11       BUSY      D4       PD4  High indicates AVR is not ready to read data.
12       POUT      D5       PD5  Not used.
13       SEL       D3       PB1  High indicates to the Amiga that something is connected.
17-22    GND       GND      GND
*/

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

void uart_init(uint32_t baudrate) {
	const uint32_t divisorFP1024 = (((uint32_t) (1024 / 8) * F_CPU) / baudrate);
	// Round to closest divisor
	const uint16_t divisor = (divisorFP1024 + 512) / 1024;
	const uint16_t ubrr = divisor - 1; 
  
	UBRR0H = ubrr >> 8;
	UBRR0L = ubrr & 0xff;

	UCSR0A |= (1 << U2X0); // 2X so F_CPU is divided with 8 instead of 16 above

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable both RX and TX
}

void uart_putbyte(uint8_t byte) {
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = byte;
}

#define PAR_STROBE_BIT 3
#define PAR_SEL_BIT    1
#define PAR_BUSY_BIT   4
#define PAR_ACK_BIT    0    

void parport_init(void) {
	// Set SEL to low to indicate we are not connected - not initialized yet!
	DDRB |= (1 << PAR_SEL_BIT);
	PORTB &= ~(1 << PAR_SEL_BIT);
	// Set BUSY to output high if someone ignores SEL, as we are BUSY - not initialized yet!
	DDRD |= (1 << PAR_BUSY_BIT);
	PORTD &= ~(1 << PAR_BUSY_BIT);

	// Enable pullup of /STROBE to not have this code go bonkers if not connected to an Amiga.
	PORTD |= (1 << PAR_STROBE_BIT);

	// Set /ACK to output high to not start with an unintentional ack. 
	DDRB |= (1 << PAR_ACK_BIT);
	PORTB |= (1 << PAR_ACK_BIT);

	// Set SEL to high to indicate we are connected
	PORTB |= (1 << PAR_SEL_BIT);
}

void parport_loop() {
	PORTD &= ~(1 << PAR_BUSY_BIT); // Not BUSY while waiting for start of /STROBE
	loop_until_bit_is_clear(PIND, PAR_STROBE_BIT); // Wait for start of /STROBE
	PORTD |= (1 << PAR_BUSY_BIT); // BUSY directly after start of /STROBE
	
	// Read and forward parport data byte to serial port
	const uint8_t byte = (PINC & 0x3f) | (PIND & 0xc0);
	uart_putbyte(byte);
	
	loop_until_bit_is_set(PIND, PAR_STROBE_BIT); // Wait for end of /STROBE

	// Do an /ACK pulse
	PORTB &= ~(1 << PAR_ACK_BIT);
	_delay_us(5);
	PORTB |= (1 << PAR_ACK_BIT);
}

int main(void) {
	//TODO: Make baudrate configurable via serial and/or parallel port
  	uart_init(57600);
	parport_init();

	while (true) {
		parport_loop();
	}

	return 0;
}
