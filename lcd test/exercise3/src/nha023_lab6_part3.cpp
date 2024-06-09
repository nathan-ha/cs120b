/*        Your Name & E-mail: Nathan Ha nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Lab #5  Exercise #3

 *         Exercise Description: Can control fan manually and autoatically

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: https://www.youtube.com/watch?v=L2hR0_vnp6I

 */

#include <stdio.h>
#include <string.h>

#include "../lib/LCD.h"
#include "../lib/helper.h"
#include "../lib/periph.h"
#include "../lib/serialATmega-1.h"
#include "../lib/timerISR-2.h"

void TimerISR() {}

int main(void) {
  // port d outputs
  DDRD = 0xFF;
  PORTD = 0x00;
  // port c inputs
  DDRC = 0x00;
  PORTC = 0xFF;
  // port b outputs
  DDRB = 0xFF;
  PORTB = 0x00;

  serial_init(9600);
  ADC_init();    // initializes ADC
  init_sonar();  // initializes sonar
  lcd_init();
  lcd_goto_xy(0, 0);
  while (1) {
    lcd_write_str("womp");
    _delay_ms(1000);
    lcd_clear();
    serial_println("here");
  }

  return 0;
}