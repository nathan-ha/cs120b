/*        Your Name & E-mail: Nathan Ha    nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Project (First Week)

 *         Exercise Description: Bullet hell style game

 *        

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: <URL>

 */


#include "../lib/helper.h"
#include "../lib/music.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/spiAVR.h"
#include "../lib/state_machines.h"
#include "../lib/timerISR.h"

#define NUM_TASKS 3

// Task struct for concurrent synchSMs implmentations
typedef struct _task {
  signed char state;          // Task's current state
  unsigned long period;       // Task period
  unsigned long elapsedTime;  // Time elapsed since last task tick
  int (*TickFct)(int);        // Task tick function
} task;

// TODO: Define Periods for each task
//  e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned long BUZZER_PERIOD = 300;
const unsigned long DISPLAY_PERIOD = 20;
const unsigned long GAME_PERIOD = 200;

const unsigned long PERIODS[] = {BUZZER_PERIOD, DISPLAY_PERIOD, GAME_PERIOD};
const unsigned long GCD_PERIOD = findGCD_Array(PERIODS, NUM_TASKS);

task tasks[NUM_TASKS];

void TimerISR() {
  for (unsigned int i = 0; i < NUM_TASKS; i++) {          // Iterate through each task in the task array
    if (tasks[i].elapsedTime >= tasks[i].period) {        // Check if the task is ready to tick
      tasks[i].state = tasks[i].TickFct(tasks[i].state);  // Tick and set the next state for this task
      tasks[i].elapsedTime = 0;                           // Reset the elapsed time for the next tick
    }
    tasks[i].elapsedTime += GCD_PERIOD;  // Increment the elapsed time by GCD_PERIOD
  }
}

int main(void) {
  // ports d and b outputs
  DDRB = 0xFF;
  PORTB = 0x00;
  DDRD = 0xFF;
  PORTD = 0x00;
  // port c inputs
  DDRC = 0x00;
  PORTC = 0xFF;
  // other inits
  ADC_init();
  serial_init(9600);
  SPI_INIT();
  TFT_INIT();
  pbuzzer_init();
  game_init();

  tasks[0].period = BUZZER_PERIOD;
  tasks[0].state = PBUZZER_INIT;
  tasks[0].elapsedTime = BUZZER_PERIOD;
  tasks[0].TickFct = &tick_passive_buzzer;

  tasks[1].period = DISPLAY_PERIOD;
  tasks[1].state = DISPLAY_INIT;
  tasks[1].elapsedTime = DISPLAY_PERIOD;
  tasks[1].TickFct = &tick_display;

  tasks[2].period = GAME_PERIOD;
  tasks[2].state = GAME_INIT;
  tasks[2].elapsedTime = GAME_PERIOD;
  tasks[2].TickFct = &tick_game;

  TimerSet(GCD_PERIOD);
  TimerOn();

  while (1) {
  }

  return 0;
}