/*        Your Name & E-mail: Nathan Ha nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Lab #4  Exercise #2

 *         Exercise Description: Moving joystick in specific pattern activates motor

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

enum states {INIT, NEUTRAL, UP, DOWN, LEFT, RIGHT, PRESS, SUCCESS} state;


unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
   return (b ?  (x | (0x01 << k))  :  (x & ~(0x01 << k)) );
              //   Set bit to 1           Set bit to 0
}

unsigned char GetBit(unsigned char x, unsigned char k) {
   return ((x & (0x01 << k)) != 0);
}

void ADC_init() {
  ADMUX = (1<<REFS0);
	ADCSRA|= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}



unsigned int ADC_read(unsigned char chnl){
  uint8_t low, high;

  ADMUX  = (ADMUX & 0xF8) | (chnl & 7);
  ADCSRA |= 1 << ADSC ;
  while((ADCSRA >> ADSC)&0x01){}


	low  = ADCL;
	high = ADCH;

	return ((high << 8) | low) ;
}


int nums[16] = {0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011, 0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011, 0b1110111, 0b0011111, 0b1001110, 0b0111101, 0b1001111, 0b1000111 }; 
// a  b  c  d  e  f  g
void outNum(int num){
	PORTD = nums[num] << 1; //assigns bits 1-7 of nums(a-f) to pins 2-7 of port d
  PORTB = SetBit(PORTB, 0 ,nums[num]&0x01); // assigns bit 0 of nums(g) to pin 0 of port b
}

//directions[] and outDir() will be neeeded for ex 2 and 3
int directions[4] = { }; //TODO: copmlete the array containg the values needed for the 7 sgements for each of the 4 directions
// a  b  c  d  e  f  g
//TODO: display the direction to the 7-seg display. HINT: will be very similar to outNum()
void outDir(enum states dir){
  
}

int phases[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001}; //8 phases of the stepper motor step

enum states password[4] = {UP, DOWN, LEFT, RIGHT};
enum states moves[4] = {NEUTRAL, NEUTRAL, NEUTRAL, NEUTRAL};

char turn_num = 0;


bool password_is_correct(void) {
  for (char i = 0; i < 4; i++) {
    if (password[i] != moves[i]) return false;
  }
  return true;
}

void resetMoves(void) {
  for (int i = 0; i < 4; i++) 
    moves[i] = NEUTRAL;
}

void spin_motor(int ms) {
  int i = 0;
  for (int t = 0; t < ms; t++) {
      if((PINC >> 2) & 0x01){ //button not pressed
         PORTB = (PORTB & 0x03) | phases[i] << 2; //& first to reset pins 2-5 but not 0-1 then | with phase shifted left 2 to assign the right value to pins 2-5
         i++; //increment to next phase
         if(i > 7){ //if all phases are completed, restart
            i = 0;
         }
     }else{
         PORTB = (PORTB & 0x03) | phases[i] << 2;
         i--;
         if(i < 0){
             i = 8;
         }
    }
    while (!TimerFlag){}  // Wait for SM period
    TimerFlag = 0;
  }
}

char motor_is_done = 0;

void Tick() {
  const int stick_x = ADC_read(0);
  const int stick_y = ADC_read(1);

  const char stick_up = stick_x > 900;
  const char stick_down = stick_x < 100;
  const char stick_right = stick_y > 900;
  const char stick_left = stick_y < 100;
  const char stick_press = ADC_read(2) < 500;
  const char stick_neutral = !stick_up && !stick_down && !stick_left && !stick_right;

  // State Transistions
  //TODO: complete transitions 
  switch(state) {

    case INIT:
      state = NEUTRAL;
      break;
    case NEUTRAL:
      if (turn_num >= 4 && password_is_correct()) {
        state = SUCCESS;
      }
      else if (stick_up) {
        state = UP;
        moves[turn_num] = UP;
      }
      else if (stick_down) {
        state = DOWN;
        moves[turn_num] = DOWN;
      }
      else if (stick_left) {
        state = LEFT;
        moves[turn_num] = LEFT;
      }
      else if (stick_right) {
        state = RIGHT;
        moves[turn_num] = RIGHT;
      }
      break;
    case UP:
      if (stick_neutral) {
        state = NEUTRAL;
        turn_num++;
      }
      break;
    case DOWN:
      if (stick_neutral) {
        state = NEUTRAL;
        turn_num++;
      }
      break;
    case LEFT:
      if (stick_neutral) {
        state = NEUTRAL;
        turn_num++;
      }
      break;
    case RIGHT:
      if (stick_neutral) {
        state = NEUTRAL;
        turn_num++;
      }
      break;
    case SUCCESS:
      if (motor_is_done) {
        state = NEUTRAL;
        turn_num = 0;
        motor_is_done = 0;
        resetMoves();
      }
      else {
        state = SUCCESS;
      }
      break;
    default:
      state = INIT;
      break;

  }

  // State Actions
  //TODO: complete transitions
  switch(state) {
    case INIT:
      break;
    case NEUTRAL:
      if (turn_num >= 4 && !password_is_correct()) turn_num = 0;
      outNum(turn_num);
      break;
    case UP:
      // TODO: output U
      break;
    case DOWN:
      // TODO: output D
      break;
    case LEFT:
      // TODO: output L
      break;
    case RIGHT:
      // TODO: output R
      break;
    case SUCCESS:
      outNum(4);
      spin_motor(2000);
      motor_is_done = 1;
      break;
    default:
      break;

  }

}



int main(void)
{
  DDRB = 0xFF; PORTB = 0x00; // init port B outputs
  DDRD = 0xFF; PORTD = 0x00; // init port D outputs


  ADC_init();//initializes the analog to digital converter
  serial_init(9600);

	
  state = INIT;
  DDRC = 0x00; PORTC = 0xFF;
  TimerSet(1); //period of 1 ms. good period for the stepper mottor
  TimerOn();
    while (1)
    {
      // serial_println(state);
		  Tick();      // Execute one synchSM tick
      while (!TimerFlag){}  // Wait for SM period
      TimerFlag = 0;        // Lower flag
     }

    return 0;
}