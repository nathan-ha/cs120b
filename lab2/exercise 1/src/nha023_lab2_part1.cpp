/*         Nathan Ha 

*          Discussion Section: 22

 *         Assignment: Lab #2  Exercise #1

 *         Exercise Description: Takes a number from user and outputs the number in binary using LEDs   

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *         Demo Video: https://www.youtube.com/watch?v=dTSNTP8lPcM


 */

#include <Arduino.h>

int pins[4] = {2, 3, 4, 5};
void setup()
{
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(pins[i], OUTPUT);
  }
}

// returns the state of the nth bit (1 or 0 for true/false)
bool nthBitState(int num, int n) {
  // bit shift the nth bit to the lsb 
  // and mask it to get only the last bit
 	return (num >> n) & 0x0001;
}

void loop()
{
  Serial.println("-----------------------------------------------");
  Serial.println("Input a number between 0 and 15 (inclusive)\n");
  
  while (Serial.available() == 0) {} // wait until user inputs something
  
  int input = Serial.parseInt(); // get user input
  Serial.print("Entered number in hex: ");
  Serial.println(input, HEX);
  
  Serial.println("Writing to pins...");
  for (int i = 0; i < 4; i++) {
    // toggle the bit for the each corresponding pin
    if (nthBitState(input, i) == true) {
     	digitalWrite(pins[i], HIGH); 
    } else {
     	digitalWrite(pins[i], LOW);
    }
  }
  Serial.println("Done.");
}
