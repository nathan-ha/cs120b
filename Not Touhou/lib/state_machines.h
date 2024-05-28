/*        Your Name & E-mail: Nathan Ha    nha023@ucr.edu

*          Discussion Section: 22

 *         Assignment: Final Project

 *         Description: State Machine Logic Goes Here

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 */
#ifndef STATE_MACHINES_H
#define STATE_MACHINES_H


#include "../lib/helper.h"
#include "../lib/music.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/spiAVR.h"
#include "../lib/timerISR.h"
#include "../lib/game_logic.h"

enum passive_buzzer_state { PBUZZER_INIT, PLAY };
int tick_passive_buzzer(int state) {
  static int i = 0;

  switch (state) {
    case PBUZZER_INIT:
      state = PLAY;
      break;
    case PLAY:
      break;
    default:
      break;
  }

  switch (state) {
    case PLAY:
      pbuzzer_change_freq(night_of_knights[i]);
      i = (i + 1) % night_of_knight_length;
      break;
    default:
      break;
  }
  return state;
}

enum display_state { DISPLAY_INIT, REFRESH };
int tick_display(int state) {
  switch (state) {
    case DISPLAY_INIT:
      state = REFRESH;
      break;
    case REFRESH:
      break;
    default:
      break;
  }

  switch (state) {
    case REFRESH:
      draw_game_screen();
      game_loop();
      // box_bouncing_loop();  // TODO make game loop
      erase_game_screen();
      break;
    default:
      break;
  }
  return state;
}

enum game_state { GAME_INIT, GAME_START, GAME_PLAYING, GAME_PAUSE, GAME_LOSE, GAME_WIN };
int tick_game(int state) {
  switch (state) {
    case GAME_INIT:
      state = GAME_START;
      break;
    case GAME_START:
      // TODO wait for 3 seconds
      break;
    case GAME_PLAYING:
      break;
    case GAME_PAUSE:
      break;
    case GAME_LOSE:
      break;
    case GAME_WIN:
      break;
    default:
      break;
  }

  switch (state) {
    case GAME_START:
      break;
    case GAME_PLAYING:
      game_loop();
      break;
    case GAME_PAUSE:
      break;
    case GAME_LOSE:
      break;
    case GAME_WIN:
      break;
    default:
      break;
  }
  return state;
}

#endif
