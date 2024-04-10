/*         Nathan Ha 

*          Discussion Section: 22

 *         Assignment: Lab #2  Exercise #3

 *         Exercise Description: Takes a number from user and outputs the number in binary using LEDs. Then the program asks
              for user to select a bit to slowly shift right.

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *        Demo Video: 

 */


#include <Arduino.h>


bool nthBitState(int num, int n);
void turnOffAllPins();
void intToLED(int input);

//Define digital pins here:
int pins[12] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

//7-segment display pins and implementation (change pin values if necessary):
int a = 6;
int b = 7;
int c = 8;
int d = 9;
int e = 10;
int f = 11;
int g = 12;
int dp = 13;
int LEDS[7] {a, b, c, d, e, f, g};
//Some values for a-g are provided as examples, implement the remaining a-g values corresponding to the comments below:
//1 = Segment ON, 0 = Segment OFF
int nums[17][7] {
  {1, 1, 1, 1, 1, 1, 0}, //0
  {0, 1, 1, 0, 0, 0, 0}, //1
  {1, 1, 0, 1, 1, 0, 1}, //2
  {1, 1, 1, 1, 0, 0, 1}, //3
  {0, 1, 1, 0, 0, 1, 1}, //4 		<- Implement!
  {1, 0, 1, 1, 0, 1, 1}, //5 		<- Implement!
  {1, 0, 1, 1, 1, 1, 1}, //6 		<- Implement!
  {1, 1, 1, 0, 0, 0, 0}, //7 		<- Implement!
  {1, 1, 1, 1, 1, 1, 1}, //8 		<- Implement!
  {1, 1, 0, 0, 1, 1, 1}, //9 		<- Implement!
  {1, 1, 1, 0, 1, 1, 1}, //10 (A)
  {0, 0, 1, 1, 1, 1, 1}, //11 (b)
  {1, 0, 0, 1, 1, 1, 0}, //12 (C)	<- Implement in hex!
  {0, 1, 1, 1, 1, 0, 1}, //13 (d)	<- Implement in hex!
  {1, 0, 0, 1, 1, 1, 1}, //14 (E)	<- Implement in hex!
  {1, 0, 0, 0, 1, 1, 1}, //15 (F)	<- Implement in hex!
  {0, 0, 0, 0, 0, 0, 0} //off
// a  b  c  d  e  f  g
};


//Call this function in your code to output the integer x on the 7-segment display in hex
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 0; i < 12; i++) {
  pinMode(pins[i], OUTPUT);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("-----------------------------------------------");
  Serial.println("Input a number between 0 and 15 (inclusive)\n");

  while (Serial.available() == 0) {} // wait until user inputs something
  int input = Serial.parseInt(); // get user input
  Serial.println(input);
  Serial.println();
  intToLED(input);
  outNum(input);

  // pt 2 stuff
  // pt 2 -- take bit, isolate it, and shift it
  Serial.println("Select bit to shift to the right (0-3).");
  while (Serial.available() == 0) {}
  int selected_bit = Serial.parseInt(); 
  int mask = 0x1 << selected_bit;   // 1101
  Serial.println(selected_bit);    //& 0001 << 2
  Serial.println();                //= 0100 

  input &= mask;                      
  intToLED(input);    
  outNum(input);                
  if (input != 0) {
    while (input != 1) { // when int is 1, the bit is completely right shifted 
      delay(500);
      input >>= 1;
      intToLED(input);
      outNum(input);
    }
  } else {
    intToLED(0);    
    outNum(0);  
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
