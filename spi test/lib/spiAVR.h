#ifndef SPIAVR_H
#define SPIAVR_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "helper.h"

// B5 should always be SCK(spi clock) and B3 should always be MOSI. If you are using an
// SPI peripheral that sends data back to the arduino, you will need to use B4 as the MISO pin.
// The SS pin can be any digital pin on the arduino. Right before sending an 8 bit value with
// the SPI_SEND() funtion, you will need to set your SS pin to low. If you have multiple SPI
// devices, they will share the SCK, MOSI and MISO pins but should have different SS pins.
// To send a value to a specific device, set it's SS pin to low and all other SS pins to high.

// Outputs, pin definitions
#define PIN_SCK PORTB5   // SHOULD ALWAYS BE B5 ON THE ARDUINO // sck
#define PIN_MOSI PORTB3  // SHOULD ALWAYS BE B3 ON THE ARDUINO  // sda
#define PIN_SS PORTB2    // cs
#define PIN_A0 PORTB1    // A0 or DC (command/data mode)
#define PIN_RST PORTB0   // reset

// If SS is on a different port, make sure to change the init to take that into account.
void SPI_INIT() {
  DDRB |= (1 << PIN_MOSI) | (1 << PIN_SCK) | (1 << PIN_SS) | (1 << PIN_A0) | (1 << PIN_RST);
  SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);  // initialize SPI coomunication
}

void SPI_SEND(char data) {
  SPDR = data;                    // set data that you want to transmit
  while (!(SPSR & (1 << SPIF)));  // wait until done transmitting
}

// ****************************************************************************************************
// ****************************************************************************************************
// ****************************************************************************************************
// ********************************* TFT DISPLAY COMMANDS *********************************************
// ****************************************************************************************************
// ****************************************************************************************************
// ****************************************************************************************************

// Function to send command to display
void TFT_SEND_COMMAND(uint8_t command) {
  PORTB &= ~(1 << PIN_A0);  // set A0 (DC) low for command mode
  PORTB &= ~(1 << PIN_SS);  // select the tft display
  SPI_SEND(command);        // send command
  PORTB |= (1 << PIN_SS);   // deselect the tft display
}

// Function to send data to display
void TFT_SEND_DATA(uint8_t data) {
  PORTB |= (1 << PIN_A0);   // set A0 (DC) high for data mode
  PORTB &= ~(1 << PIN_SS);  // select the tft display
  SPI_SEND(data);           // send data
  PORTB |= (1 << PIN_SS);   // deselect the tft display
}

// clears the display memory faster than writing one pixel at a time
// same as the init function
// note: could also use this logic to quickly make a rectangle
void TFT_FLUSH() {
  TFT_SEND_COMMAND(0x2A);  // Set column address
  TFT_SEND_DATA(0x00);     // start column high byte
  TFT_SEND_DATA(0x00);     // start column low byte
  TFT_SEND_DATA(0x00);     // end column high byte
  TFT_SEND_DATA(127);      // end column low byte

  TFT_SEND_COMMAND(0x2B);  // Set row address
  TFT_SEND_DATA(0x00);     // start row high byte
  TFT_SEND_DATA(0x00);     // start row low byte
  TFT_SEND_DATA(0x00);     // end row high byte
  TFT_SEND_DATA(127);      // end row low byte

  TFT_SEND_COMMAND(0x2C);  // Memory write
  for (short i = 0; i < 128 * 200; i++) {
    TFT_SEND_DATA(0x00);  // Write black color data to clear the screen
  }
}

// Function to initialize ST7735
void TFT_INIT() {
  // Reset the display
  // honestly not sure if we need to reset the display but it can't hurt
  PORTB &= ~(1 << PIN_RST);  // Reset low
  _delay_ms(10);             // Delay for reset
  PORTB |= (1 << PIN_RST);   // Reset high
  _delay_ms(10);             // Delay for reset

  // Initialize display
  TFT_SEND_COMMAND(0x01);  // Software reset
  _delay_ms(10);

  TFT_SEND_COMMAND(0x11);  // Sleep out // don't know if this device ever actually goes to sleep
  _delay_ms(10);

  TFT_SEND_COMMAND(0x3A);  // pixel format
  TFT_SEND_DATA(0x03);     // 12-bit colors

  TFT_FLUSH();  // erase leftover memory

  // Set default rotation
  TFT_SEND_COMMAND(0x36);    // Memory Access Control
  TFT_SEND_DATA(0b0101010);  // MY = 0, MX (mirror x) = 1, MV = 1, ML = 0, RGB (instead of bgr) = 1, MH = 0

  // Set display on
  TFT_SEND_COMMAND(0x29);  // Display on
}

// draws pixels at the x y coords
void TFT_DRAW_PIXEL(uint8_t x, uint8_t y, uint16_t color) {
  TFT_SEND_COMMAND(0x2A);  // Set column address
  TFT_SEND_DATA(0x00);     // start column high byte
  TFT_SEND_DATA(x);        // start column low byte
  TFT_SEND_DATA(0x00);     // end column high byte
  TFT_SEND_DATA(x);        // end column low byte

  TFT_SEND_COMMAND(0x2B);  // Set row address
  TFT_SEND_DATA(0x00);     // start row high byte
  TFT_SEND_DATA(y);        // start row low byte
  TFT_SEND_DATA(0x00);     // end row high byte
  TFT_SEND_DATA(y);        // end row low byte

  // Set pixel color
  TFT_SEND_COMMAND(0x2C);     // Memory write
  TFT_SEND_DATA(color >> 4);  // honestly don't know why i have to shift it like this
  TFT_SEND_DATA(color << 4);  // datasheet doesn't mention it, but it's the only way it works
}

#endif /* SPIAVR_H */