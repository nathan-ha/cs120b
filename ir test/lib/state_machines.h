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

#include <stdio.h>
#include <string.h>

#include "../lib/LCD.h"
#include "../lib/game_logic.h"
#include "../lib/helper.h"
#include "../lib/irAVR.h"
#include "../lib/music.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/spiAVR.h"
#include "../lib/timerISR.h"

const unsigned short BUZZER_PERIOD = 220;
const unsigned short DISPLAY_PERIOD = 20;
const unsigned short GAME_PERIOD = 20;
const unsigned short LCD_PERIOD = 300;
const unsigned short TIMER_PERIOD = 1;
const unsigned short IR_PERIOD = 100;

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
      pbuzzer_change_freq(pgm_read_word(&night_of_knights[i]));
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
      state = GAME_PLAYING;
      break;
    case GAME_PLAYING:
      // if (health_player <= 0) state = GAME_LOSE;
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
      game_init();
      break;
    case GAME_PLAYING:
      gameplay_message();
      game_loop();
      break;
    case GAME_PAUSE:
      break;
    case GAME_LOSE:
      TFT_DRAW_RECTANGLE(0, 0, 130, 130, 0xFFF);
      death_message();
      break;
    case GAME_WIN:
      break;
    default:
      break;
  }
  return state;
}

enum lcd_state { LCD_INIT, LCD_CLEAR, LCD_TOP, LCD_BOTTOM };
int tick_lcd(int state) {
  switch (state) {
    case LCD_INIT:
      state = LCD_CLEAR;
      break;
    case LCD_CLEAR:
      state = LCD_TOP;
      break;
    case LCD_TOP:
      state = LCD_BOTTOM;
      break;
    case LCD_BOTTOM:
      state = LCD_INIT;
      break;
    default:
      break;
  }
  switch (state) {
    case LCD_CLEAR:
      lcd_clear();
      break;
    case LCD_TOP:
      lcd_goto_xy(0, 0);
      lcd_write_str(lcd_message_top);
      break;
    case LCD_BOTTOM:
      lcd_goto_xy(1, 0);
      lcd_write_str(lcd_message_bottom);
      break;
    default:
      break;
  }
  return state;
}

enum timer_state { TIMER_INIT, TIMER_TICK };
int tick_timer(int state) {
  switch (state) {
    case TIMER_INIT:
      elapsed_time_seconds = 0;
      elapsed_time_ms = 0;
      state = TIMER_TICK;
      break;
    case TIMER_TICK:
      break;
    default:
      break;
  }
  switch (state) {
    case TIMER_TICK:
      if (health_player <= 0) break;
      if (++elapsed_time_ms >= 1000) {
        elapsed_time_ms = 0;
        elapsed_time_seconds++;
      }
      break;
    default:
      break;
  }
  return state;
}

enum ir_state { IR_INIT, IR_RECEIVE };
int tick_ir(int state) {
  switch (state) {
    case IR_INIT:
      state = IR_RECEIVE;
      break;
    case IR_RECEIVE:
      break;
    default:
      break;
  }
  switch (state) {
    case IR_RECEIVE:
      // decode_results d;
      // if (IRdecode(&d)) {
      //   serial_println(d.value);
      //   IRresume();
      // }
    default:
      break;
  }
  return state;
}

#endif
