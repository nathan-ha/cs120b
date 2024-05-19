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

#define NUM_TASKS 7  // TODO: Change to the number of tasks being used

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
const unsigned long VERTICAL_JOYSTICK_PERIOD = 200;
const unsigned long STEPPER_PERIOD = 2;
const unsigned long BUZZER_PERIOD = 1000;
const unsigned long SERVO_PERIOD = 2;
const unsigned long GCD_PERIOD = 2;

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

// shared variables
int H_stepper = 0;
int L_stepper = 0;
char forward_direction = 0;

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

void change_pwm(void) {
  if (forward_direction) {
    H_stepper = map_value(512, 1024, 0, 15, ADC_read(0));
    L_stepper = 15 - H_stepper;
    return;
  }
  L_stepper = map_value(0, 512, 0, 15, ADC_read(0));
  H_stepper = 15 - L_stepper;
}

enum vertical_joystick_state { VJ_INIT, NEUTRAL, UP, DOWN };
int tick_vjoystick(int state) {
  const int stick_x = ADC_read(0);
  const char up = stick_x > 600;
  const char down = stick_x < 400;
  const char neutral = !up && !down;

  switch (state) {
    case VJ_INIT:
      state = NEUTRAL;
      break;
    case NEUTRAL:
      if (up) {
        state = UP;
      } else if (down) {
        state = DOWN;
      }
      break;
    case UP:
      if (neutral) {
        state = NEUTRAL;
      }
      break;
    case DOWN:
      if (neutral) {
        state = NEUTRAL;
      }
      break;
    default:
      break;
  }

  switch (state) {
    case NEUTRAL:
      L_stepper = 15;
      H_stepper = 0;
      break;
    case UP:
      forward_direction = 1;
      change_pwm();
      break;
    case DOWN:
      forward_direction = 0;
      change_pwm();
      break;
    default:
      break;
  }

  return state;
}

enum stepper_state { STEPPER_INIT, HIGH_STEP, LOW_STEP };
int tick_stepper(int state) {
  static int ticks = 0;

  switch (state) {
    case STEPPER_INIT:
      state = LOW_STEP;
      break;
    case LOW_STEP:
      if (ticks >= L_stepper) {
        state = HIGH_STEP;
        ticks = 0;
      }
      break;
    case HIGH_STEP:
      if (ticks >= H_stepper) {
        state = LOW_STEP;
        ticks = 0;
      }
    default:
      break;
  }

  switch (state) {
    case LOW_STEP:
      ticks++;
      break;
    case HIGH_STEP:
      if (H_stepper == 0) break;
      static int phase_index = 0;
      if (!forward_direction) {
        PORTB = (PORTB & 0x03) | stages[phase_index] << 2;  //& first to reset pins 2-5 but not 0-1 then | with phase
                                                            // shifted left 2 to assign the right value to pins 2-5
        phase_index++;
        if (phase_index > 7) {  // if all phases are completed, restart
          phase_index = 0;
        }
      } else {
        PORTB = (PORTB & 0x03) | stages[phase_index] << 2;
        phase_index--;
        if (phase_index < 0) {
          phase_index = 8;
        }
      }
      ticks++;
      break;
  }
  return state;
}

enum backup_buzzer_state { BB_INIT, BB_OFF, PAUSE, ON };
int tick_back_buzzer(int state) {
  switch (state) {
    case BB_INIT:
      state = BB_OFF;
      break;
    case BB_OFF:
      if (!forward_direction && H_stepper != 0) {
        state = ON;
      }
      break;
    case ON:
      state = PAUSE;
      TCCR0B = 0;  // buzzer off
      break;
    case PAUSE:
      state = BB_OFF;
      break;
    default:
      break;
  }
  switch (state) {
    case BB_OFF:
      break;
    case ON:
      TCCR0B = (TCCR0B & 0xF8) | 0x03;  // buzzer on
      break;
    case PAUSE:
      break;
  }
  return state;
}

enum servo_state { INIT_SERVO, ROTATE };
int tick_servo(int state) {
  switch (state) {
    case INIT_SERVO:
      state = ROTATE;
      break;
    case ROTATE:
      break;
    default:
      break;
  }
  switch (state) {
    case ROTATE:
      OCR1A = map_value(0, 1024, 999, 4999, ADC_read(1));
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

  tasks[3].period = VERTICAL_JOYSTICK_PERIOD;
  tasks[3].state = VJ_INIT;
  tasks[3].elapsedTime = VERTICAL_JOYSTICK_PERIOD;
  tasks[3].TickFct = &tick_vjoystick;

  tasks[4].period = STEPPER_PERIOD;
  tasks[4].state = STEPPER_INIT;
  tasks[4].elapsedTime = STEPPER_PERIOD;
  tasks[4].TickFct = &tick_stepper;

  tasks[5].period = BUZZER_PERIOD;
  tasks[5].state = BB_INIT;
  tasks[5].elapsedTime = BUZZER_PERIOD;
  tasks[5].TickFct = &tick_back_buzzer;

  tasks[6].period = SERVO_PERIOD;
  tasks[6].state = INIT_SERVO;
  tasks[6].elapsedTime = SERVO_PERIOD;
  tasks[6].TickFct = &tick_servo;

  TimerSet(GCD_PERIOD);
  TimerOn();

  while (1) {
  }

  return 0;
}