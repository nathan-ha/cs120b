/*         Nathan Ha 

*          Discussion Section: 22

 *         Assignment: Lab #2  Exercise #2

 *         Exercise Description: Takes a number from user and outputs the number in binary using LEDs. Then the program asks
              for user to select a bit to slowly shift right 

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *        Demo Video: https://www.youtube.com/watch?v=AR-V8v-pDMs

 */

#include <Arduino.h>

bool nthBitState(int num, int n);
void turnOffAllPins();
void intToLED(int input);


int pins[4] = {2, 3, 4, 5};
void setup()
{
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(pins[i], OUTPUT);
  }
}

void loop()
{
  Serial.println("-----------------------------------------------");
  Serial.println("Input a number between 0 and 15 (inclusive)\n");

  while (Serial.available() == 0) {} // wait until user inputs something
  int input = Serial.parseInt(); // get user input
  Serial.println(input);
  Serial.println();
  intToLED(input);

  // pt 2 -- take bit, isolate it, and shift it
  Serial.println("Select bit to shift to the right (0-3).");
  
  while (Serial.available() == 0) {}
  int selected_bit = Serial.parseInt(); 
  int mask = 0x1 << selected_bit;   // 1101
  Serial.println(selected_bit);    //& 0001 << 2
  Serial.println();                //= 0100 

  input &= mask;                      
  intToLED(input);                    
  if (input != 0) {
    while (input != 1) { // when int is 1, the bit is completely right shifted 
      delay(200);
      input >>= 1;
      intToLED(input);
    }
  }

}

// returns the state of the nth bit (1 or 0 for true/false)
bool nthBitState(int num, int n) {
  // bit shift the nth bit to the lsb 
  // and mask it to get only the last bit
 	return (num >> n) & 0x0001;
}

void turnOffLEDs() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(pins[i], LOW); 
  }
}

void intToLED(int input) {
  for (int i = 0; i < 4; i++) {
    // toggle the bit for the each corresponding pin
    if (nthBitState(input, i) == true) {
     	digitalWrite(pins[i], HIGH); 
    } else {
     	digitalWrite(pins[i], LOW);
    }
  }
}
