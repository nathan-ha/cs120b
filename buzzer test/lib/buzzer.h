#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void buzzer_init() {
  OCR0A = 255;                            // sets duty cycle to 50% since TOP is always 256
  TCCR0A |= (1 << COM0A1);                // use Channel A
  TCCR0A |= (1 << WGM01) | (1 << WGM00);  // set fast PWM Mode
  TCCR0B = (TCCR0B & 0xF8) | 0x02;        // set prescaler to 8
  // TCCR0B = (TCCR0B & 0xF8) | 0x03;        // set prescaler to 64
  // TCCR0B = (TCCR0B & 0xF8) | 0x04;        // set prescaler to 256
  // TCCR0B = (TCCR0B & 0xF8) | 0x05;        // set prescaler to 1024
}