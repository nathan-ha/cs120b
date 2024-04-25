/*        Your Name & E-mail: Nathan Ha nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Lab #4  Exercise #2

 *         Exercise Description: Moving joystick in specific pattern activates motor. Pressing while unlocked lets you change password.

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: <URL>

 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../lib/timerISR.h"
#include "../lib/serialATmega.h"

enum states
{
  INIT,
  NEUTRAL,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  PRESS,
  SUCCESS
} state;

unsigned char SetBit(unsigned char number, unsigned char kth_bit, unsigned char bit)
{
  return (bit ? (number | (0x01 << kth_bit)) : (number & ~(0x01 << kth_bit)));
  //   Set bit to 1           Set bit to 0
}

unsigned char GetBit(unsigned char number, unsigned char kth_bit)
{
  return ((number & (0x01 << kth_bit)) != 0);
}

void ADC_init()
{
  ADMUX = (1 << REFS0);
  ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  // ADEN: setting this bit enables analog-to-digital conversion.
  // ADSC: setting this bit starts the first conversion.
  // ADATE: setting this bit enables auto-triggering. Since we are
  //        in Free Running Mode, a new conversion will trigger whenever
  //        the previous conversion completes.
}

unsigned int ADC_read(unsigned char chnl)
{
  uint8_t low, high;

  ADMUX = (ADMUX & 0xF8) | (chnl & 7);
  ADCSRA |= 1 << ADSC;
  while ((ADCSRA >> ADSC) & 0x01)
  {
  }

  low = ADCL;
  high = ADCH;

  return ((high << 8) | low);
}

int nums[16] = {0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011, 0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011, 0b1110111, 0b0011111, 0b1001110, 0b0111101, 0b1001111, 0b1000111};
// a  b  c  d  e  f  g
void outNum(int num)
{
  PORTD = nums[num] << 1;                     // assigns bits 1-7 of nums(a-f) to pins 2-7 of port d
  PORTB = SetBit(PORTB, 0, nums[num] & 0x01); // assigns bit 0 of nums(g) to pin 0 of port b
}

// directions[] and outDir() will be neeeded for ex 2 and 3
//  a  b  c  d  e  f  g
void outDir(enum states dir)
{
  switch (dir)
  {
  case UP:
    PORTD = 0b0111110 << 1;
    PORTB = SetBit(PORTB, 0, 0b0111110 & 0x01);
    break;
  case DOWN:
    PORTD = 0b0111101 << 1;
    PORTB = SetBit(PORTB, 0, 0b0111101 & 0x01);
    break;
  case LEFT:
    PORTD = 0b0001110 << 1;
    PORTB = SetBit(PORTB, 0, 0b0001110 & 0x01);
    break;
  case RIGHT:
    PORTD = 0b0000101 << 1;
    PORTB = SetBit(PORTB, 0, 0b0000101 & 0x01);
    break;
  default:
    break;
  }
}

int phases[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001}; // 8 phases of the stepper motor step

enum states password[4] = {UP, DOWN, LEFT, RIGHT};
enum states moves[4] = {NEUTRAL, NEUTRAL, NEUTRAL, NEUTRAL};

char turn_num = 0;

bool password_is_correct(void)
{
  for (int i = 0; i < 4; i++)
  {
    if (password[i] != moves[i])
      return false;
  }
  return true;
}

void resetMoves(void)
{
  for (int i = 0; i < 4; i++)
    moves[i] = NEUTRAL;
}

char motor_is_done = 0;
char is_locked = 1;
char phase_index = 0;
int ticks = 0;
int is_changing_password = 0;
int temp_password_index = 0;

void Tick()
{
  const int stick_x = ADC_read(0);
  const int stick_y = ADC_read(1);

  const char stick_up = stick_x > 900;
  const char stick_down = stick_x < 200;
  const char stick_right = stick_y > 900;
  const char stick_left = stick_y < 200;
  const char stick_press = ADC_read(2) < 500;
  const char stick_neutral = !stick_up && !stick_down && !stick_left && !stick_right;

  // State Transistions
  // TODO: complete transitions
  switch (state)
  {
  case INIT:
    state = NEUTRAL;
    break;
  case NEUTRAL:
    if (temp_password_index >= 4)
    {
      temp_password_index = 0;
      is_changing_password = 0;
      turn_num = 0;
      PORTC = SetBit(PORTC, 3, 0); // turn off led
      resetMoves();
    }
    if (turn_num >= 4 && password_is_correct() && !is_changing_password)
    {
      state = SUCCESS;
    }
    else if (stick_up)
    {
      state = UP;
      if (is_changing_password)
        password[temp_password_index++] = UP;
      moves[turn_num] = UP;
    }
    else if (stick_down)
    {
      state = DOWN;
      if (is_changing_password)
        password[temp_password_index++] = DOWN;
      moves[turn_num] = DOWN;
    }
    else if (stick_left)
    {
      state = LEFT;
      if (is_changing_password)
        password[temp_password_index++] = LEFT;
      moves[turn_num] = LEFT;
    }
    else if (stick_right)
    {
      state = RIGHT;
      if (is_changing_password)
        password[temp_password_index++] = RIGHT;
      moves[turn_num] = RIGHT;
    }
    else if (stick_press)
    {
      state = PRESS;
    }
    break;
  case UP:
    if (stick_neutral)
    {
      state = NEUTRAL;
      turn_num++;
    }
    else
    {
      state = UP;
    }
    break;
  case DOWN:
    if (stick_neutral)
    {
      state = NEUTRAL;
      turn_num++;
    }
    else
    {
      state = DOWN;
    }
    break;
  case LEFT:
    if (stick_neutral)
    {
      state = NEUTRAL;
      turn_num++;
    }
    else
    {
      state = LEFT;
    }
    break;
  case RIGHT:
    if (stick_neutral)
    {
      state = NEUTRAL;
      turn_num++;
    }
    else
    {
      state = RIGHT;
    }
    break;

  case SUCCESS:
    if (motor_is_done)
    {
      state = NEUTRAL;
      turn_num = 0;
      motor_is_done = 0;
      resetMoves();
    }
    else
    {
      state = SUCCESS;
    }
    break;
  case PRESS:
    if (stick_neutral)
    {
      state = NEUTRAL;
    }
    else
    {
      state = PRESS;
    }
    break;
  default:
    state = INIT;
    break;
  }

  // State Actions
  switch (state)
  {
  case INIT:
    break;
  case NEUTRAL:
    if (turn_num >= 4 && !password_is_correct())
      turn_num = 0;
    if (is_locked)
      PORTC = SetBit(PORTC, 5, 1); // sets decimal point on lock
    outNum(turn_num);
    break;
  case UP:
    outDir(UP);
    break;
  case DOWN:
    outDir(DOWN);
    break;
  case LEFT:
    outDir(LEFT);
    break;
  case RIGHT:
    outDir(RIGHT);
    break;
  case SUCCESS:
    outNum(4);
    // spin motor
    if (!motor_is_done)
    {
      if (is_locked)
      {
        PORTB = (PORTB & 0x03) | phases[phase_index] << 2; //& first to reset pins 2-5 but not 0-1 then | with phase shifted left 2 to assign the right value to pins 2-5
        phase_index++;
        if (phase_index > 7)
        { // if all phases are completed, restart
          phase_index = 0;
        }
      }
      else
      {
        PORTB = (PORTB & 0x03) | phases[phase_index] << 2;
        phase_index--;
        if (phase_index < 0)
        {
          phase_index = 8;
        }
      }
    }
    ticks++;
    if (ticks >= 1000)
    {
      motor_is_done = 1;
      ticks = 0;
      is_locked = !is_locked;
      if (!is_locked)
        PORTC = SetBit(PORTC, 5, 0); // drops decimal point on unlock
    }
    break;
  case PRESS:
    if (!is_locked)
    {
      PORTC = SetBit(PORTC, 3, 1); // toggle led
      is_changing_password = 1;
      temp_password_index = 0;
      turn_num = 0;
    }
    break;
  default:
    break;
  }
}

int main(void)
{
  DDRB = 0xFF;
  PORTB = 0x00; // init port B outputs
  DDRD = 0xFF;
  PORTD = 0x00; // init port D outputs

  ADC_init(); // initializes the analog to digital converter
  serial_init(9600);

  state = INIT;
  DDRC = 0x00;
  PORTC = 0xFF;                // port c input
  DDRC = SetBit(DDRC, 5, 1);   // a5 output
  PORTC = SetBit(PORTC, 5, 0); // a5 output
  PORTC = SetBit(PORTC, 3, 0); // led off
  TimerSet(1);                 // period of 1 ms. good period for the stepper mottor
  TimerOn();

  while (1)
  {
    Tick(); // Execute one synchSM tick
    while (!TimerFlag)
    {
    } // Wait for SM period
    TimerFlag = 0; // Lower flag
  }

  return 0;
}