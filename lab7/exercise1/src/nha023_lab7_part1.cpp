/*        Your Name & E-mail: Nathan Ha nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Lab #7  Exercise #1

 *         Exercise Description: Implement turn signals and honking

 *        

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: <URL>

 */

#include "../lib/helper.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/timerISR.h"

#define NUM_TASKS 3  // TODO: Change to the number of tasks being used

// Task struct for concurrent synchSMs implmentations
typedef struct _task {
  signed char state;          // Task's current state
  unsigned long period;       // Task period
  unsigned long elapsedTime;  // Time elapsed since last task tick
  int (*TickFct)(int);        // Task tick function
} task;

// TODO: Define Periods for each task
//  e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned long SIGNAL_PERIOD = 500;
const unsigned long HONK_PERIOD = 200;
const unsigned long GCD_PERIOD = findGCD(SIGNAL_PERIOD, HONK_PERIOD);

task tasks[NUM_TASKS];  // declared task array with 5 tasks

void TimerISR() {
  for (unsigned int i = 0; i < NUM_TASKS; i++) {          // Iterate through each task in the task array
    if (tasks[i].elapsedTime == tasks[i].period) {        // Check if the task is ready to tick
      tasks[i].state = tasks[i].TickFct(tasks[i].state);  // Tick and set the next state for this task
      tasks[i].elapsedTime = 0;                           // Reset the elapsed time for the next tick
    }
    tasks[i].elapsedTime += GCD_PERIOD;  // Increment the elapsed time by GCD_PERIOD
  }
}

int stages[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001};  // Stepper motor phases

// TODO: Create your tick functions for each task
enum signal_state { SIGNAL_INIT, OFF, L1, L2, L3 };
int tick_right_signal(int state) {
  const char is_pressed = ADC_read(4) > 500;
  switch (state) {
    case SIGNAL_INIT:
      state = OFF;
      break;
    case OFF:
      if (is_pressed) {
        state = L1;
        PORTD = SetBit(PORTD, 4, 1);  // set first led
      }
      break;
    case L1:
      if (is_pressed) {
        state = L2;
        PORTD = SetBit(PORTD, 4, 0);  // off first led
        PORTD = SetBit(PORTD, 3, 1);  // set 2nd led
      } else {
        state = OFF;
      }
      break;
    case L2:
      if (is_pressed) {
        state = L3;
        PORTD = SetBit(PORTD, 3, 0);  // off 2nd led
        PORTD = SetBit(PORTD, 2, 1);  // set 3rd led
      } else {
        state = OFF;
      }
      break;
    case L3:
      state = OFF;
    default:
      break;
  }
  switch (state) {
    case OFF:
      PORTD = SetBit(PORTD, 4, 0);  // off first led
      PORTD = SetBit(PORTD, 3, 0);  // off 2nd led
      PORTD = SetBit(PORTD, 2, 0);  // off 3rd led
      break;
    case L1:
      break;
    case L2:
      break;
    case L3:
      break;
    default:
      break;
  }
  return state;
}

int tick_left_signal(int state) {
  const char is_pressed = ADC_read(3) > 500;
  switch (state) {
    case SIGNAL_INIT:
      state = OFF;
      break;
    case OFF:
      if (is_pressed) {
        state = L1;
        PORTB = SetBit(PORTB, 0, 1);  // set first led
      }
      break;
    case L1:
      if (is_pressed) {
        state = L2;
        PORTB = SetBit(PORTB, 0, 0);  // off first led
        PORTD = SetBit(PORTD, 7, 1);  // set 2nd led
      } else {
        state = OFF;
      }
      break;
    case L2:
      if (is_pressed) {
        state = L3;
        PORTD = SetBit(PORTD, 7, 0);  // off 2nd led
        PORTD = SetBit(PORTD, 5, 1);  // set 3rd led
      } else {
        state = OFF;
      }
      break;
    case L3:
      state = OFF;
    default:
      break;
  }
  switch (state) {
    case OFF:
      PORTD = SetBit(PORTD, 7, 0);  // off first led
      PORTB = SetBit(PORTB, 0, 0);  // off 2nd led
      PORTD = SetBit(PORTD, 5, 0);  // off 3rd led
      break;
    case L1:
      break;
    case L2:
      break;
    case L3:
      break;
    default:
      break;
  }
  return state;
}

enum honk_state { HONK_INIT, HONK_OFF, HONK_ON };

int tick_honk(int state) {
  const char is_pressed = ADC_read(2) < 500;
  switch (state) {
    case HONK_INIT:
      state = HONK_OFF;
      break;
    case HONK_OFF:
      if (is_pressed) {
        state = HONK_ON;
        TCCR0B = (TCCR0B & 0xF8) | 0x04;  // set prescaler to 128
      }
      break;
    case HONK_ON:
      if (!is_pressed) {
        state = HONK_OFF;
        TCCR0B = 0x00;  // turn off buzzer
      }
      break;
    default:
      break;
  }
  switch (state) {
    case HONK_OFF:
      break;
    case HONK_ON:
      break;
    default:
      break;
  }
  return state;
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
  OCR0A = 128;                            // sets duty cycle to 50% since TOP is always 256
  TCCR0A |= (1 << COM0A1);                // use Channel A
  TCCR0A |= (1 << WGM01) | (1 << WGM00);  // set fast PWM Mode
  // TCCR0B = (TCCR0B & 0xF8) | 0x02;        // set prescaler to 8
  // TCCR0B = (TCCR0B & 0xF8) | 0x03;        // set prescaler to 64
  // TCCR0B = (TCCR0B & 0xF8) | 0x04;        // set prescaler to 256
  // TCCR0B = (TCCR0B & 0xF8) | 0x05;        // set prescaler to 1024

  // TODO: Initialize the servo timer/pwm(timer1)
  TCCR1A |= (1 << WGM11) | (1 << COM1A1);               // COM1A1 sets it to channel A
  TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);  // CS11 sets the prescaler to be 8
  // WGM11, WGM12, WGM13 set timer to fast pwm mode
  ICR1 = 39999;  // 20ms pwm period
  OCR1A = 0 /* set your value here */;

  // TODO: Initialize tasks here
  //  e.g.
  //  tasks[0].period = ;
  //  tasks[0].state = ;
  //  tasks[0].elapsedTime = ;
  //  tasks[0].TickFct = ;

  tasks[0].period = SIGNAL_PERIOD;
  tasks[0].state = SIGNAL_INIT;
  tasks[0].elapsedTime = SIGNAL_PERIOD;
  tasks[0].TickFct = &tick_left_signal;

  tasks[1].period = SIGNAL_PERIOD;
  tasks[1].state = SIGNAL_INIT;
  tasks[1].elapsedTime = SIGNAL_PERIOD;
  tasks[1].TickFct = &tick_right_signal;

  tasks[2].period = HONK_PERIOD;
  tasks[2].state = HONK_INIT;
  tasks[2].elapsedTime = HONK_PERIOD;
  tasks[2].TickFct = &tick_honk;

  TimerSet(GCD_PERIOD);
  TimerOn();

  while (1) {
  }

  return 0;
}