/*        Your Name & E-mail: Nathan Ha    nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Project (First Week)

 *         Exercise Description: Bullet hell style game

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: <URL>

 */

#include <avr/pgmspace.h>

#include "../lib/LCD.h"
#include "../lib/helper.h"
#include "../lib/irAVR.h"
#include "../lib/music.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/spiAVR.h"
#include "../lib/state_machines.h"
#include "../lib/timerISR.h"

#define NUM_TASKS 0

#define PIN_IR PD3

// Task struct for concurrent synchSMs implmentations
typedef struct _task {
  signed char state;          // Task's current state
  unsigned long period;       // Task period
  unsigned long elapsedTime;  // Time elapsed since last task tick
  int (*TickFct)(int);        // Task tick function
} task;

// TODO: Define Periods for each task
//  e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned int PERIODS[] = {BUZZER_PERIOD, DISPLAY_PERIOD, GAME_PERIOD, LCD_PERIOD, TIMER_PERIOD, IR_PERIOD};
const unsigned int GCD_PERIOD = 100;//findGCD_Array(PERIODS, NUM_TASKS);

task tasks[NUM_TASKS];

void TimerISR() {
  // for (unsigned int i = 0; i < NUM_TASKS; i++) {          // Iterate through each task in the task array
  //   if (tasks[i].elapsedTime >= tasks[i].period) {        // Check if the task is ready to tick
  //     tasks[i].state = tasks[i].TickFct(tasks[i].state);  // Tick and set the next state for this task
  //     tasks[i].elapsedTime = 0;                           // Reset the elapsed time for the next tick
  //   }
  //   tasks[i].elapsedTime += GCD_PERIOD;  // Increment the elapsed time by GCD_PERIOD
  // }
    int msg = 0;
    // for (char i = 0; i < 32; i++) {
    //   msg |= (GetBit(PORTD, PIN_IR) << i);
    // }
    // msg = ADC_read(3);
    // serial_println(msg);
    // _delay_ms(100);
}

int main(void) {
  // ports d and b outputs
  DDRB = 0xFF;
  PORTB = 0x00;
  DDRD = 0xFF;
  PORTD = 0x00;

  // pin 3 (ir) input
  DDRD &= ~(1 << PIN_IR);
  DDRD |= (1 << PIN_IR);

  // port c inputs
  DDRC = 0x00;
  PORTC = 0xFF;

  // other inits
  ADC_init();
  lcd_init();
  serial_init(9600);
  SPI_INIT();
  TFT_INIT();
  pbuzzer_init();
  IRinit(&DDRD, &PIND, PIN_IR);

  serial_println("hi");

  // tasks[0].period = BUZZER_PERIOD;
  // tasks[0].state = PBUZZER_INIT;
  // tasks[0].elapsedTime = BUZZER_PERIOD;
  // tasks[0].TickFct = &tick_passive_buzzer;

  // tasks[4].period = DISPLAY_PERIOD;
  // tasks[4].state = DISPLAY_INIT;
  // tasks[4].elapsedTime = DISPLAY_PERIOD;
  // tasks[4].TickFct = &tick_display;

  // tasks[3].period = GAME_PERIOD;
  // tasks[3].state = GAME_INIT;
  // tasks[3].elapsedTime = GAME_PERIOD;
  // tasks[3].TickFct = &tick_game;

  // tasks[2].period = LCD_PERIOD;
  // tasks[2].state = LCD_INIT;
  // tasks[2].elapsedTime = LCD_PERIOD;
  // tasks[2].TickFct = &tick_lcd;

  // tasks[1].period = TIMER_PERIOD;
  // tasks[1].state = TIMER_INIT;
  // tasks[1].elapsedTime = TIMER_PERIOD;
  // tasks[1].TickFct = &tick_timer;

  // tasks[0].period = IR_PERIOD;
  // tasks[0].state = IR_INIT;
  // tasks[0].elapsedTime = IR_PERIOD;
  // tasks[0].TickFct = &tick_ir;

  TimerSet(GCD_PERIOD);
  TimerOn();

  volatile int c[30][30];
  while (1) {

    for (int i = 0; i < 30; i++) {
      for (int j = 0; j < 30; j++) {
        c[j][i]++;
      }
    }
    _delay_ms(10);
    serial_println("test");

  }

  return 0;
}