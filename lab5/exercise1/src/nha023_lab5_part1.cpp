/*        Your Name & E-mail: Nathan Ha nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Lab #5  Exercise #1

 *         Exercise Description: Read distance from sonar sensor and display on 4 digit 7 seg display

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: https://www.youtube.com/watch?v=JcXAr4BipfQ

 */

#include "../lib/timerISR-2.h"
#include "../lib/helper.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"

// TODO: declare variables for cross-task communication
int distance = 0; // cm

#define NUM_TASKS 2

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
const unsigned long SONAR_PERIOD = 1000; // ms
const unsigned long OUTPUT_PERIOD = 1;
const unsigned long GCD_PERIOD = findGCD(SONAR_PERIOD, OUTPUT_PERIOD);

task tasks[NUM_TASKS]; // declared task array with 5 tasks

// TODO: Define, for each task:
//  (1) enums and
//  (2) tick functions
enum sonar_state
{
    SONAR_INIT,
    IDLE,
    READ
} sonar_state;
int t = 0;
int TickFct_sonar(int state)
{
    switch (state)
    {
    case SONAR_INIT:
        state = IDLE;
        break;
    case IDLE:
        state = READ;
        break;
    case READ:
        state = IDLE;
        break;
    default:
        break;
    }

    switch (state)
    {
    case SONAR_INIT:
        break;
    case IDLE:
        break;
    case READ:
        distance = (int)read_sonar();
        break;
    default:
        break;
    }

    return state;
}
enum output_state
{
    OUTPUT_INIT,
    FIRST_DIGIT,
    SECOND_DIGIT,
    THIRD_DIGIT,
    FOURTH_DIGIT
} output_state;
int TickFct_output(int state)
{
    // transitions
    switch (state)
    {
    case OUTPUT_INIT:
        state = FIRST_DIGIT;
        break;
    case FIRST_DIGIT:
        state = SECOND_DIGIT;
        break;
    case SECOND_DIGIT:
        state = THIRD_DIGIT;
        break;
    case THIRD_DIGIT:
        state = FOURTH_DIGIT;
        break;
    case FOURTH_DIGIT:
        state = FIRST_DIGIT;
        break;
    default:
        break;
    }
    // actions
    switch (state)
    {
    case OUTPUT_INIT:
        break;
    case FIRST_DIGIT:
        // PORTB = 0xFF;
        PORTB = SetBit(PORTB, 5, 1);
        outNum(distance % 10);
        PORTB = SetBit(PORTB, 2, 0); // enable D1
        break;
    case SECOND_DIGIT:
        // PORTB = 0xFF;
        PORTB = SetBit(PORTB, 2, 1);
        outNum((distance / 10) % 10);
        PORTB = SetBit(PORTB, 3, 0); // enable D2
        break;
    case THIRD_DIGIT:
        // PORTB = 0xFF;
        PORTB = SetBit(PORTB, 3, 1);
        outNum((distance / 100) % 10);
        PORTB = SetBit(PORTB, 4, 0); // enable D3
        break;
    case FOURTH_DIGIT:
        // PORTB = 0xFF;
        PORTB = SetBit(PORTB, 4, 1);
        outNum((distance / 1000) % 10);
        PORTB = SetBit(PORTB, 5, 0); // enable D4
        break;
    default:
        break;
    }
    return state;
}



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
    // set all digital pins to outputs
    DDRB = 0xFF;
    PORTB = 0x00;

    DDRD = 0xFF;
    PORTD = 0x00;

    DDRB = SetBit(DDRB, 0, 0); // d8 input
    PORTB = SetBit(PORTB, 0, 1);

    DDRC = 0xFF; // port c output
    PORTC = 0x00;

    ADC_init();   // initializes ADC
    init_sonar(); // initializes sonar

    // TODO: Initialize tasks here
    //  e.g. tasks[0].period = TASK1_PERIOD
    //  tasks[0].state = ...
    //  tasks[0].timeElapsed = ...
    //  tasks[0].TickFct = &task1_tick_function;
    tasks[0].state = OUTPUT_INIT;
    tasks[0].period = OUTPUT_PERIOD;
    tasks[0].elapsedTime = 0;
    tasks[0].TickFct = &TickFct_output;

    tasks[1].state = SONAR_INIT;
    tasks[1].period = SONAR_PERIOD;
    tasks[1].elapsedTime = 0;
    tasks[1].TickFct = &TickFct_sonar;


    TimerSet(GCD_PERIOD);
    TimerOn();
    serial_init(9600);
    while (1)
    {
        while (!TimerFlag)
            ;
        TimerFlag = 0;
    }

    return 0;
}