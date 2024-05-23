#include "../lib/helper.h"
#include "../lib/music.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/spiAVR.h"
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

const unsigned long GCD_ARRAY[] = {BUZZER_PERIOD};
const unsigned long GCD_PERIOD = findGCD_Array(GCD_ARRAY, NUM_TASKS);

task tasks[NUM_TASKS];

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
      pbuzzer_change_freq(night_of_knights[i]);
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

  ADC_init();
  serial_init(9600);
  SPI_INIT();
  TFT_INIT();
  pbuzzer_init();

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