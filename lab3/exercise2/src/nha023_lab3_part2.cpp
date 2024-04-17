/*         Nathan Ha 

*          Discussion Section: 22

 *         Assignment: Lab #3   Exercise #2

 *         Exercise Description: Implements bathroom light FSM. Light turns on if either user presses button or room goes dark.
              The potentiometer also adjusts the color of the RGB LED.

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *        Demo Video: 

 */



#include "Arduino.h"
#include "../lib/timerISR.h"

//Define digital pins here:
int light_sensor = A4;
int button = 5;
int r_led = 11;
int g_led = 10;
int b_led = 9;
int potentiometer = A5;


//You will need to determine the value for the dark_threshold based on the light level in your room
//(Tip: Use Serial.println() to output the photoresistor value in your current room and set the dark_threshold to a value slightly below that)
int dark_threshold = 50;

//4 digit 7-segment display pins and implementation (you will only be using the first digit in this lab):
int a = 8;
int b = 2;
int c = 4;
int d = 12;
int e = 13;
int f = 7;
int g = 6;
int d1 = A1;
int LEDS[7] {a, b, c, d, e, f, g};
int nums[11][7] {
  {1, 1, 1, 1, 1, 1, 0}, //0
  {0, 1, 1, 0, 0, 0, 0}, //1
  {1, 1, 0, 1, 1, 0, 1}, //2
  {1, 1, 1, 1, 0, 0, 1}, //3
  {0, 1, 1, 0, 0, 1, 1}, //4
  {1, 0, 1, 1, 0, 1, 1}, //5
  {1, 0, 1, 1, 1, 1, 1}, //6
  {1, 1, 1, 0, 0, 0, 0}, //7
  {1, 1, 1, 1, 1, 1, 1}, //8
  {1, 1, 1, 1, 0, 1, 1}, //9
  {0, 0, 0, 0, 0, 0, 0} //off
// a  b  c  d  e  f  g
};

void write_rgb_helper(int red, int green, int blue) {
  analogWrite(r_led, red);
  analogWrite(g_led, green);
  analogWrite(b_led, blue);
}

void write_rgb(bool enabled) {
  if (!enabled) {
     write_rgb_helper(0, 0, 0);
     return;
  }
  switch (map(analogRead(potentiometer), 0, 1023, 1, 5)) {
    case 1: write_rgb_helper(30, 0, 0); break;
    case 2: write_rgb_helper(0, 30, 0); break;
    case 3: write_rgb_helper(0, 0, 30); break;
    case 4: write_rgb_helper(10, 10, 10); break;
    default: write_rgb_helper(0, 0, 0); break;
  }
}

//Call this function in your code to output the integer x on the first digit of the 4 digit 7-segment display
void outNum(int x) {
  for (int i = 0; i < 7; i++) {
    if (nums[x][i] == 1) {
      digitalWrite(LEDS[i], 1);
    }
    else {
      digitalWrite(LEDS[i], 0);
    }
  }
}

enum states {INIT, OFF_RELEASE, ON_PUSH, ON_RELEASE, OFF_PUSH, ON_TIMED} state;
int time = 0;

//TODO: Implement your synch SM
void Tick() {

  bool BUTTON_ON = digitalRead(button);
  bool DETECTED_DARK = analogRead(light_sensor) < dark_threshold;

  // State Transistions
  switch(state) {
    case INIT: state = OFF_RELEASE; break;
    case OFF_RELEASE: 
      if (BUTTON_ON) {
        state = ON_PUSH;
      } else if (DETECTED_DARK) {
        state = ON_TIMED;
        time = 0;
      }
      break;
    case ON_PUSH:
      if (!BUTTON_ON) {
        state = ON_RELEASE;
      }
      break;
    case ON_RELEASE:
      if (BUTTON_ON) {
        state = OFF_PUSH;
      }
      break;
    case OFF_PUSH:
      if (!BUTTON_ON) {
        state = OFF_RELEASE;
      }
      break;
    case ON_TIMED:
      if (BUTTON_ON) {
        state = OFF_PUSH;
      } 
      else if (time >= 9) {
        state = OFF_RELEASE;
      }
      else {
        if (DETECTED_DARK) {
          time = 0;
        } else {
          time++;
        }
      } 
    default: break;
  }
  // State Actions
  switch(state) {
    case OFF_RELEASE: write_rgb(false); break;
    case ON_PUSH: write_rgb(true); break;
    case ON_RELEASE: write_rgb(true); break;
    case OFF_PUSH: write_rgb(false); break;
    case ON_TIMED: write_rgb(true); break;
    default: break;
  }
}

void setup() {
	for (int i = 0; i < 7; i++) {
      pinMode(LEDS[i], OUTPUT);
  }
  pinMode(d1, OUTPUT);
  pinMode(light_sensor, INPUT);
  pinMode(button, INPUT);
  pinMode(potentiometer, INPUT);

  Serial.begin(9600);
  

    state = INIT; //initilzes initila state of SM
    TimerSet(1000);//sets the timer to 1 second
    TimerOn();//starts the timer
}

void loop() {
  Tick();//Calls a single SM tick
  Serial.println(analogRead(light_sensor));
  while(!TimerFlag){} //Waits for the timer to go off
  TimerFlag = 0; //Resets the timer flag
}
