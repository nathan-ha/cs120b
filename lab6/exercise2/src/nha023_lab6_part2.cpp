/*        Your Name & E-mail: Nathan Ha nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Lab #5  Exercise #2

 *         Exercise Description: Implement manual fan control

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: https://www.youtube.com/watch?v=XrTGnTLraoM

 */

#include "../lib/timerISR-2.h"
#include "../lib/helper.h"
#include "../lib/periph.h"
#include "../lib/LCD.h"
#include "../lib/serialATmega-1.h"
#include <stdio.h>
#include <string.h>

// TODO: declare variables for cross-task communication
char auto_mode = 0;
char buzzer_on = 0;
int H_fan_pwm = 5;
int L_fan_pwm = 5;

/* You have 5 tasks to implement for this lab */
#define NUM_TASKS 4

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
const unsigned long BUZZER_PERIOD = 100;
const unsigned long FAN_PERIOD = 1;
const unsigned long GCD_PERIOD = 1;

task tasks[NUM_TASKS]; // declared task array with 5 tasks

void display_mode_and_duty_cycle()
{
    const int duty_cycle = 100 * H_fan_pwm / (H_fan_pwm + L_fan_pwm);
    lcd_clear();
    lcd_goto_xy(0, 0);
    lcd_goto_xy(1, 0);
    char msg[20] = "FAN: ";
    char buffer[4] = "   ";
    snprintf(buffer, 4, "%d", duty_cycle);
    strcat(msg, buffer);
    strcat(msg, "%");

    lcd_write_str(msg);
    lcd_goto_xy(0, 0);
    if (!auto_mode)
        lcd_write_str("Mode: MANUAL");
    else
        lcd_write_str("Mode: AUTO");
}

// TODO: Define, for each task:
//  (1) enums and
//  (2) tick functions
enum mode_state
{
    MODE_INIT,
    MANUAL,
    AUTO
};

int tick_mode(int state)
{

    switch (state)
    {
    case MODE_INIT:
        state = MANUAL;
        display_mode_and_duty_cycle();
        break;
    case MANUAL:
        if (auto_mode)
        {
            state = AUTO;
            lcd_clear();
            lcd_goto_xy(0, 0);
            lcd_write_str("Mode: AUTO");
        }
        break;
    case AUTO:
        if (!auto_mode)
        {
            state = MANUAL;
            display_mode_and_duty_cycle();
        }
        break;
    default:
        break;
    }

    switch (state)
    {
    case MANUAL:
        break;
    case AUTO:
        break;
    default:
        break;
    }
    return state;
}

enum joystick_state
{
    JOYSTICK_INIT,
    NEUTRAL,
    UP,
    DOWN,
    PRESS
};

int tick_joystick(int state)
{
    const int stick_x = ADC_read(1);
    const char press = ADC_read(2) < 500;
    const char up = stick_x > 900;
    const char down = stick_x < 200;
    const char neutral = !up && !down && !press;

    switch (state)
    {
    case JOYSTICK_INIT:
        state = NEUTRAL;
        break;
    case NEUTRAL:
        if (up)
        {
            state = UP;
            if (!auto_mode && L_fan_pwm != 0)
            {
                H_fan_pwm++;
                L_fan_pwm--;
                if (!auto_mode)
                    display_mode_and_duty_cycle();
            }
        }
        else if (down)
        {
            state = DOWN;
            if (!auto_mode && H_fan_pwm != 0)
            {
                H_fan_pwm--;
                L_fan_pwm++;
                if (!auto_mode)
                    display_mode_and_duty_cycle();
            }
        }
        else if (press)
        {
            state = PRESS;
            auto_mode = !auto_mode;
        }
        break;
    case UP:
        if (neutral)
        {
            state = NEUTRAL;
        }
        break;
    case DOWN:
        if (neutral)
        {
            state = NEUTRAL;
        }
        break;
    case PRESS:
        if (neutral)
        {
            state = NEUTRAL;
        }
        break;
    default:
        break;
    }

    switch (state)
    {
    case NEUTRAL:
        break;
    case UP:
        buzzer_on = 1;
        break;
    case DOWN:
        buzzer_on = 1;
        break;
    case PRESS:
        buzzer_on = 1;
        break;
    default:
        break;
    }

    return state;
}

enum buzzer_state
{
    BUZZER_INIT,
    OFF,
    ON
};

int tick_buzzer(int state)
{
    static int ticks = 0;
    switch (state)
    {
    case BUZZER_INIT:
        state = OFF;
        break;
    case OFF:
        if (buzzer_on)
        {
            ticks = 0;
            state = ON;
        }
        break;
    case ON:
        if (ticks < 1)
        {
            ticks++;
        }
        else
        {
            // who toucha ma sphaghett(i code)
            // if joystick is being held, don't switch off yet
            const int stick_x = ADC_read(1);
            const char press = ADC_read(2) < 500;
            const char up = stick_x > 900;
            const char down = stick_x < 200;
            const char neutral = !up && !down && !press;

            if (!neutral)
            {
                buzzer_on = 1;
                state = ON;
            }
            else
            {
                buzzer_on = 0;
                state = OFF;
            }
        }
        break;
    default:
        break;
    }

    switch (state)
    {
    case OFF:
        PORTB = SetBit(PORTB, 0, 0);
        break;
    case ON:
        PORTB = SetBit(PORTB, 0, 1);
        break;
    default:
        break;
    }
    return state;
}

void power_fan()
{
    PORTB = SetBit(PORTB, 1, 1); // counter clockwise rotation
    // PORTB = SetBit(PORTB, 2, 1); // clockwise rotation
}

void depower_fan()
{
    PORTB = SetBit(PORTB, 1, 0); // counter clockwise rotation
    // PORTB = SetBit(PORTB, 2, 0); // clockwise rotation
}

enum fan_state {INIT_FAN, FAN_LOW, FAN_HIGH};
int tick_fan(int state)
{
    static int ticks = 0;
    switch (state)
    {
        case INIT_FAN:
            state = FAN_LOW;
            break;
        case FAN_LOW:
            if (ticks >= L_fan_pwm)
            {
                if (auto_mode) depower_fan(); // remove in pt 3
                else power_fan();
                ticks = 0;
                state = FAN_HIGH;
            }
            else
            {
                ticks++;
            }
            break;
        case FAN_HIGH:
            if (ticks >= H_fan_pwm)
            {
                depower_fan();
                ticks = 0;
                state = FAN_LOW;
            }
            else
            {
                ticks++;
            }
            break;
        default:
            break;    
    }

    switch (state)
    {
        case FAN_LOW:
            break;
        case FAN_HIGH:
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
    // port d outputs
    DDRD = 0xFF;
    PORTD = 0x00;
    // port c inputs
    DDRC = 0x00;
    PORTC = 0xFF;
    // port b outputs
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

    tasks[0].period = MODE_PERIOD;
    tasks[0].state = MODE_INIT;
    tasks[0].elapsedTime = MODE_PERIOD;
    tasks[0].TickFct = &tick_mode;

    tasks[1].period = JOYSTICK_PERIOD;
    tasks[1].state = JOYSTICK_INIT;
    tasks[1].elapsedTime = JOYSTICK_PERIOD;
    tasks[1].TickFct = &tick_joystick;

    tasks[2].period = BUZZER_PERIOD;
    tasks[2].state = BUZZER_INIT;
    tasks[2].elapsedTime = BUZZER_PERIOD;
    tasks[2].TickFct = &tick_buzzer;

    tasks[3].period = FAN_PERIOD;
    tasks[3].state = INIT_FAN;
    tasks[3].elapsedTime = FAN_PERIOD;
    tasks[3].TickFct = &tick_fan;

    tasks[4].period = BUZZER_PERIOD;
    tasks[4].state = BUZZER_INIT;
    tasks[4].elapsedTime = BUZZER_PERIOD;
    tasks[4].TickFct = &tick_buzzer;

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1)
    {
    }

    return 0;
}