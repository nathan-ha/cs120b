/*        Your Name & E-mail: Nathan Ha nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Lab #5  Exercise #3

 *         Exercise Description: Read distance from sonar sensor and display on 4 digit 7 seg display. Change LED color based on distance. Joystick can be used to change distance thresholds.

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: https://www.youtube.com/watch?v=08BgsMcOLk4

 */

#include "../lib/timerISR-2.h"
#include "../lib/helper.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"

// TODO: declare variables for cross-task communication
int distance = 0; // cm

#define NUM_TASKS 5

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
const unsigned long OUTPUT_PERIOD = 1;   // ms
const unsigned long GREEN_PERIOD = 1;
const unsigned long RED_PERIOD = 1;
const unsigned long JOY_PERIOD = 200;
const unsigned long GCD_PERIOD = findGCD(JOY_PERIOD, findGCD(findGCD(SONAR_PERIOD, OUTPUT_PERIOD), findGCD(GREEN_PERIOD, RED_PERIOD)));

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

char display_inches = 0;

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
    int temp_distance = distance;
    if (display_inches)
        temp_distance = distance / 2.54; // convert to inches

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
        outNum(temp_distance % 10);
        PORTB = SetBit(PORTB, 2, 0); // enable D1
        break;
    case SECOND_DIGIT:
        // PORTB = 0xFF;
        PORTB = SetBit(PORTB, 2, 1);
        outNum((temp_distance / 10) % 10);
        PORTB = SetBit(PORTB, 3, 0); // enable D2
        break;
    case THIRD_DIGIT:
        // PORTB = 0xFF;
        PORTB = SetBit(PORTB, 3, 1);
        outNum((temp_distance / 100) % 10);
        PORTB = SetBit(PORTB, 4, 0); // enable D3
        break;
    case FOURTH_DIGIT:
        // PORTB = 0xFF;
        PORTB = SetBit(PORTB, 4, 1);
        outNum((temp_distance / 1000) % 10);
        PORTB = SetBit(PORTB, 5, 0); // enable D4
        break;
    default:
        break;
    }
    return state;
}

int close_thresh = 10;
int moderate_thresh = 20;

enum green_pwm_state
{
    GREEN_INIT,
    GREEN_LOW,
    GREEN_HIGH
};
int green_pwn_L = 0;
int green_pwn_H = 0;
int green_ticks = 0;
void update_pwm_green()
{
    if (distance <= close_thresh)
    {
        green_pwn_H = 0;
        green_pwn_L = 10;
    }
    else if (distance < moderate_thresh)
    {
        green_pwn_H = 3;
        green_pwn_L = 7;
    }
    else
    {
        green_pwn_H = 10;
        green_pwn_L = 0;
    }
}
int TickFct_green(int state)
{
    switch (state)
    {
    case GREEN_INIT:
        state = GREEN_LOW;
        break;
    case GREEN_LOW:
        if (green_ticks > green_pwn_L)
        {
            green_ticks = 0;
            state = GREEN_HIGH;
        }
        else
        {
            state = GREEN_LOW;
            set_rgb_led(0, 0, 0);
        }
        break;
    case GREEN_HIGH:
        if (green_ticks > green_pwn_H)
        {
            green_ticks = 0;
            state = GREEN_LOW;
        }
        else
        {
            state = GREEN_HIGH;
            set_rgb_led(0, 1, 0);
        }
        break;
    default:
        break;
    }

    switch (state)
    {
    case GREEN_LOW:
        green_ticks++;
        break;
    case GREEN_HIGH:

        green_ticks++;
        break;
    default:
        break;
    }
    update_pwm_green();
    return state;
}

enum red_pwm_state
{
    RED_INIT,
    RED_LOW,
    RED_HIGH
};
int red_pwn_L = 0;
int red_pwn_H = 0;
int red_ticks = 0;
void update_pwm_red()
{
    if (distance < close_thresh)
    {
        red_pwn_H = 10;
        red_pwn_L = 0;
    }
    else if (distance < moderate_thresh)
    {
        red_pwn_H = 9;
        red_pwn_L = 1;
    }
    else
    {
        red_pwn_H = 0;
        red_pwn_L = 10;
    }
}
int TickFct_red(int state)
{
    switch (state)
    {
    case RED_INIT:
        state = RED_LOW;
        break;
    case RED_LOW:
        if (red_ticks > red_pwn_L)
        {
            state = RED_HIGH;
            red_ticks = 0;
        }
        else
        {
            state = RED_LOW;
            set_rgb_led(0, 0, 0);
        }
        break;
    case RED_HIGH:
        if (red_ticks > red_pwn_H)
        {
            state = RED_LOW;
            red_ticks = 0;
        }
        else
        {
            state = RED_HIGH;
            set_rgb_led(1, 0, 0);
        }
        break;
    default:
        break;
    }

    switch (state)
    {
    case RED_LOW:
        red_ticks++;
        break;
    case RED_HIGH:
        red_ticks++;
        break;
    default:
        break;
    }
    update_pwm_red();
    return state;
}

enum joystick_state
{
    JOY_INIT,
    JOY_NEUTRAL,
    JOY_UP,
    JOY_DOWN,
    JOY_PRESS,
};
int TickFct_joystick(int state)
{
    const int stick_x = ADC_read(0);
    const char stick_press = ADC_read(1) < 500;
    const char stick_up = stick_x > 900;
    const char stick_down = stick_x < 200;
    const char stick_neutral = !stick_up && !stick_down && !stick_press;

    switch (state)
    {
    case JOY_INIT:
        state = JOY_NEUTRAL;
        break;
    case JOY_NEUTRAL:
        if (stick_up && !stick_down)
        {
            if (moderate_thresh == 10)
            { // close (5, 10) --> moderate
                close_thresh = 10;
                moderate_thresh = 20;
            }
            else if (moderate_thresh == 20) // moderate (10, 20) --> far
            {
                close_thresh = 15;
                moderate_thresh = 30;
            }
            state = JOY_UP;
        }
        else if (stick_down && !stick_up)
        {
            if (moderate_thresh == 30)
            { // far (15, 30) --> moderate
                close_thresh = 10;
                moderate_thresh = 20;
            }
            else if (moderate_thresh == 20) // moderate (10, 20) --> close
            {
                close_thresh = 5;
                moderate_thresh = 10;
            }
            state = JOY_DOWN;
        }
        else if (stick_press && !stick_neutral)
        {
            state = JOY_PRESS;
            display_inches = !display_inches;
        }
        break;
    case JOY_UP:
        if (stick_neutral)
        {
            state = JOY_NEUTRAL;
        }
        break;
    case JOY_DOWN:
        if (stick_neutral)
        {
            state = JOY_NEUTRAL;
        }
    case JOY_PRESS:
        if (stick_neutral)
        {
            state = JOY_NEUTRAL;
        }
        break;
    default:
        break;
    }

    switch (state)
    {
    case JOY_NEUTRAL:
        break;
    case JOY_UP:
        break;
    case JOY_DOWN:
        break;
    case JOY_PRESS:
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

    // set joystick inputs
    DDRC = SetBit(DDRC, 0, 0);
    DDRC = SetBit(DDRC, 1, 0);
    PORTC = SetBit(PORTC, 0, 1);
    PORTC = SetBit(PORTC, 1, 1);

    ADC_init();   // initializes ADC
    init_sonar(); // initializes sonar

    // TODO: Initialize tasks here
    //  e.g. tasks[0].period = TASK1_PERIOD
    //  tasks[0].state = ...
    //  tasks[0].timeElapsed = ...
    //  tasks[0].TickFct = &task1_tick_function;
    tasks[0].state = OUTPUT_INIT;
    tasks[0].period = OUTPUT_PERIOD;
    tasks[0].elapsedTime = OUTPUT_PERIOD;
    tasks[0].TickFct = &TickFct_output;

    tasks[1].state = SONAR_INIT;
    tasks[1].period = SONAR_PERIOD;
    tasks[1].elapsedTime = SONAR_PERIOD;
    tasks[1].TickFct = &TickFct_sonar;

    tasks[2].state = GREEN_INIT;
    tasks[2].period = GREEN_PERIOD;
    tasks[2].elapsedTime = GREEN_PERIOD;
    tasks[2].TickFct = &TickFct_green;

    tasks[3].state = RED_INIT;
    tasks[3].period = RED_PERIOD;
    tasks[3].elapsedTime = RED_PERIOD;
    tasks[3].TickFct = &TickFct_red;

    tasks[4].state = JOY_INIT;
    tasks[4].period = JOY_PERIOD;
    tasks[4].elapsedTime = JOY_PERIOD;
    tasks[4].TickFct = &TickFct_joystick;

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