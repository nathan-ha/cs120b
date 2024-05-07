
#include "../lib/timerISR-2.h"
#include "../lib/helper.h"
#include "../lib/periph.h"
#include "../lib/LCD.h"
#include "../lib/serialATmega-1.h"

// TODO: declare variables for cross-task communication

/* You have 5 tasks to implement for this lab */
#define NUM_TASKS 3

// Task struct for concurrent synchSMs implmentations
typedef struct _task
{
    signed char state;         // Task's current state
    unsigned long period;      // Task period
    unsigned long elapsedTime; // Time elapsed since last task tick
    int (*TickFct)(int);       // Task tick function
} task;

// TODO: Define Periods for each task
//  e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned long MODE_PERIOD = 200;
const unsigned long JOYSTICK_PERIOD = 100;
const unsigned long BUZZER_PERIOD = 10;
const unsigned long GCD_PERIOD = findGCD(BUZZER_PERIOD, findGCD(MODE_PERIOD, JOYSTICK_PERIOD));

task tasks[NUM_TASKS]; // declared task array with 5 tasks

// TODO: Define, for each task:
//  (1) enums and
//  (2) tick functions
enum mode_state
{
    MODE_INIT,
    MANUAL,
    AUTO
};
enum joystick_state
{
    JOYSTICK_INIT,
    NEUTRAL,
    UP,
    DOWN,
    PRESS
};
enum buzzer_state
{
    BUZZER_INIT,
    OFF,
    ON
};

void TimerISR()
{
    for (unsigned int i = 0; i < NUM_TASKS; i++)
    { // Iterate through each task in the task array
        if (tasks[i].elapsedTime == tasks[i].period)
        {                                                      // Check if the task is ready to tick
            tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
            tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
        }
        tasks[i].elapsedTime += GCD_PERIOD; // Increment the elapsed time by GCD_PERIOD
    }
}

int main(void)
{
    // TODO: initialize all your inputs and ouputs
    // port d and b outputs
    DDRD = 0xFF;
    PORTD = 0x00;
    DDRB = 0xFF;
    PORTB = 0x00;

    ADC_init();   // initializes ADC
    init_sonar(); // initializes sonar
    lcd_init();

    // TODO: Initialize tasks here
    //  e.g. tasks[0].period = TASK1_PERIOD
    //  tasks[0].state = ...
    //  tasks[0].timeElapsed = ...
    //  tasks[0].TickFct = &task1_tick_function;

    lcd_goto_xy(0, 0);
    lcd_write_str("hello");

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1)
    {
    }

    return 0;
}