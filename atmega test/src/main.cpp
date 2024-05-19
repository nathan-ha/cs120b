#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../lib/serialATmega.h"

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b)
{
  return (b ? (x | (0x01 << k)) : (x & ~(0x01 << k)));
  //   Set bit to 1           Set bit to 0
}

unsigned char GetBit(unsigned char x, unsigned char k)
{
  return ((x & (0x01 << k)) != 0);
}

void ADC_init()
{
  ADMUX = (1 << REFS0);
  ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  // ADEN: setting this bit enables analog-to-digital conversion.
  // ADSC: setting this bit starts the first conversion.
  // ADATE: setting this bit enables auto-triggering. Since we are
  //        in Free Running Mode, a new conversion will trigger whenever
  //        the previous conversion completes.
}

unsigned int ADC_read(unsigned char chnl)
{
  uint8_t low, high;

  ADMUX = (ADMUX & 0xF8) | (chnl & 7);
  ADCSRA |= 1 << ADSC;
  while ((ADCSRA >> ADSC) & 0x01)
  {
  }

  low = ADCL;
  high = ADCH;

  return ((high << 8) | low);
}

int main()
{
  serial_init(9600);
  ADC_init();
  DDRD = 0x00; // port D inputs
  PORTD = 0xFF;

  DDD1 = 0b1;


  // TODO: Initialize the buzzer timer/pwm(timer0)
  OCR0A = 128;                            // sets duty cycle to 50% since TOP is always 256
  TCCR0A |= (1 << COM0A1);                // use Channel A
  TCCR0A |= (1 << WGM01) | (1 << WGM00);  // set fast PWM Mode
  // TCCR0B = (TCCR0B & 0xF8) | 0x02;        // set prescaler to 8
  TCCR0B = (TCCR0B & 0xF8) | 0x03;        // set prescaler to 64
  // TCCR0B = (TCCR0B & 0xF8) | 0x04;        // set prescaler to 256
  // TCCR0B = (TCCR0B & 0xF8) | 0x05;        // set prescaler to 1024

  while (1)
  {
  }
}