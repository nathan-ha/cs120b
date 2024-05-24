#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

/*        Your Name & E-mail: Nathan Ha    nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Final Project

 *         Description: Logic for animations go here

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 */

#include "../lib/helper.h"
#include "../lib/music.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/spiAVR.h"
#include "../lib/timerISR.h"

void box_bouncing_loop() {
  static short x1 = 10, y1 = 50, dir_flag = 0, dir_flagy = 0;
  TFT_DRAW_RECTANGLE(x1, y1, x1 + 10, y1 + 10, 0);  // clear last rectangle
  if (x1 >= 120) {
    dir_flag = 0;
  } else if (x1 <= 0) {
    dir_flag = 1;
  }
  if (y1 >= 120) {
    dir_flagy = 0;
  } else if (y1 <= 0) {
    dir_flagy = 1;
  }
  if (dir_flag) {
    x1 += 2;
  } else {
    x1 -= 2;
  }
  if (dir_flagy) {
    y1 += 2;
  } else {
    y1 -= 2;
  }
  TFT_DRAW_RECTANGLE(x1, y1, x1 + 10, y1 + 10, 0xFFF);
}

void game_loop() {}
void draw_game_screen() {}
void erase_game_screen() {}

#endif