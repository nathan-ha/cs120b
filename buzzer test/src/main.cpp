/*        Your Name & E-mail: Nathan Ha nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Lab #7  Exercise #3

 *         Exercise Description: Implement servo motor functionality for car

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: https://www.youtube.com/watch?v=BbJ7CYbZU5E

 */

#include "../lib/helper.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/timerISR.h"

#define NUM_TASKS 1  // TODO: Change to the number of tasks being used

// Task struct for concurrent synchSMs implmentations
typedef struct _task {
  signed char state;          // Task's current state
  unsigned long period;       // Task period
  unsigned long elapsedTime;  // Time elapsed since last task tick
  int (*TickFct)(int);        // Task tick function
} task;

// TODO: Define Periods for each task
//  e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned long BUZZER_PERIOD = 400;
const unsigned long GCD_PERIOD = BUZZER_PERIOD;

task tasks[NUM_TASKS];  // declared task array with 5 tasks

/*
Notes(L) =
  1: B5 - 976
  2: E5 - 650
  3: B4 - 488
  4: G4 - 390
  5: E4 - 325
  6: D4 - 278
  7: B3 - 244
  8: A3 - 216


*/
#include "../lib/piano_notes.h"

void change_freq(int f) {
  ICR1 = 16000000 / (8 * f);  // 20ms pwm period
  OCR1A = ICR1 * 0.5;
}

enum state_b { INIT, PLAY };
int tick_b(int state) {
  static int i = 0;
  switch (state) {
    case INIT:
      state = PLAY;
      break;
    case PLAY:
      break;
    default:
      break;
  }

  switch (state) {
    case PLAY:
      change_freq(night_of_knights[i]);
      i = (i + 1) % night_of_knight_length;
      break;
    default:
      break;
  }
  return state;
}

void TimerISR() {
  // TCCR0B = (TCCR0B & 0xF8) | 0x04;
  for (unsigned int i = 0; i < NUM_TASKS; i++) {          // Iterate through each task in the task array
    if (tasks[i].elapsedTime == tasks[i].period) {        // Check if the task is ready to tick
      tasks[i].state = tasks[i].TickFct(tasks[i].state);  // Tick and set the next state for this task
      tasks[i].elapsedTime = 0;                           // Reset the elapsed time for the next tick
    }
    tasks[i].elapsedTime += GCD_PERIOD;  // Increment the elapsed time by GCD_PERIOD
  }
}

int main(void) {
  // TODO: initialize all your inputs and ouputs
  // ports d and b outputs
  DDRB = 0xFF;
  PORTB = 0x00;
  DDRD = 0xFF;
  PORTD = 0x00;

  // port c inputs
  DDRC = 0x00;
  PORTC = 0xFF;

  ADC_init();  // initializes ADC
  serial_init(9600);

  // TODO: Initialize the buzzer timer/pwm(timer0)
  // OCR1A = 128;                            // sets duty cycle to 50% since TOP is always 256
  // TCCR1A |= (1 << COM0A1);                // use Channel A
  // TCCR1A |= (1 << WGM01) | (1 << WGM00);  // set fast PWM Mode

  // TCCR1B = (TCCR0B & 0xF8) | 0x02;  // set prescaler to 8
  // // TCCR1B = (TCCR1B & 0xF8) | 0x03;        // set prescaler to 64
  // // TCCR0B = (TCCR0B & 0xF8) | 0x04;        // set prescaler to 256
  // // TCCR0B = (TCCR0B & 0xF8) | 0x05;        // set prescaler to 1024

  //////////////
  TCCR1A |= (1 << WGM11) | (1 << COM1A1);               // COM1A1 sets it to channel A
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);  // CS11 sets the prescaler to be 8
  // WGM11, WGM12, WGM13 set timer to fast pwm mode

  // TODO: Initialize tasks here
  //  e.g.
  //  tasks[0].period = ;
  //  tasks[0].state = ;
  //  tasks[0].elapsedTime = ;
  //  tasks[0].TickFct = ;

  tasks[0].period = BUZZER_PERIOD;
  tasks[0].state = INIT;
  tasks[0].elapsedTime = BUZZER_PERIOD;
  tasks[0].TickFct = &tick_b;

  TimerSet(GCD_PERIOD);
  TimerOn();

  while (1) {
  }

  return 0;
}