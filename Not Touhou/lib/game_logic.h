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

#define PIN_JOYSTICK_X PORTC0
#define PIN_JOYSTICK_Y PORTC1
#define PIN_JOYSTICK_SW PORTC2

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

short health_player = 300;
short health_boss = 1000;
char message[16] = "";
struct vector {
  float x;
  float y;
  char x_dir;
  char y_dir;
  float speed;
};

struct vector player;
struct vector boss;

struct vector boss_bullets[100];
struct vector player_bullets[20];

const int boss_bullet_size = sizeof(boss_bullets) / sizeof(boss_bullets[0]);
const int player_bullets_size = sizeof(player_bullets) / sizeof(player_bullets[0]);

struct point {
  float x;
  float y;
};
struct point boss_prev;
struct point player_prev;
struct point boss_bullets_prev[boss_bullet_size];
struct point player_bullets_prev[player_bullets_size];

void game_init() {
  player.x = 64;
  player.y = 20;
  player.x_dir = 0;  // -1 for -x, 0 for no move, 1 for +x
  player.y_dir = 0;
  player.speed = 1;

  boss.x = 100;
  boss.y = 64;
  boss.x_dir = 0;
  boss.y_dir = 0;
  boss.speed = 0.2;

  boss_prev.x = boss.x;
  boss_prev.y = boss.y;
  player_prev.x = player.x;
  player_prev.y = player.y;

  for (short i = 0; i < boss_bullet_size; i++) {
    boss_bullets[i].x = -1;
    boss_bullets[i].y = -1;
    boss_bullets[i].x_dir = 0;
    boss_bullets[i].y_dir = 0;
    boss_bullets[i].speed = 0;
  }

  for (short i = 0; i < player_bullets_size; i++) {
    player_bullets[i].x = -1;
    player_bullets[i].y = -1;
    player_bullets[i].x_dir = 0;
    player_bullets[i].y_dir = 0;
    player_bullets[i].speed = 0;
  }

  for (short i = 0; i < boss_bullet_size; i++) {
    boss_bullets_prev[i].x = -1;
    boss_bullets_prev[i].y = -1;
  }

  for (short i = 0; i < player_bullets_size; i++) {
    player_bullets_prev[i].x = -1;
    player_bullets_prev[i].y = -1;
  }
}
void game_loop() {
  const int stick_x = ADC_read(PIN_JOYSTICK_X);
  const int stick_y = ADC_read(PIN_JOYSTICK_Y);

  const char press = ADC_read(PIN_JOYSTICK_SW) < 500;
  const char up = stick_x > 900;
  const char down = stick_x < 200;
  const char right = stick_y > 900;
  const char left = stick_y < 200;
  const char neutral = !up && !down && !press && !left && !right;

  const char player_shoot = 1;  // TODO add IR button here

  // player move logic
  if (up && player.y < 125) player.y += player.speed;
  if (left && player.x > 3) player.x -= player.speed;
  if (right && player.x < 125) player.x += player.speed;
  if (down && player.y > 3) player.y -= player.speed;

  // boss move logic
  if (boss.x < player.x && boss.x < 120) {
    boss.x += boss.speed;
  } else if (boss.y < player.y && boss.y < 120) {
    boss.y += boss.speed;
  }
  if (boss.x > player.x && boss.x > 10) {
    boss.x -= boss.speed;
  } else if (boss.y > player.y && boss.y > 10) {
    boss.y -= boss.speed;
  }
}
void boss_shoot_bullet() {}
void draw_game_screen() {
  // player
  TFT_DRAW_RECTANGLE(player_prev.x - 2, player_prev.y - 2, player_prev.x + 2, player_prev.y + 2, 0x000); // erase old pixels
  TFT_DRAW_RECTANGLE(player.x - 2, player.y - 2, player.x + 2, player.y + 2, 0xFFF); // draw new pixels

  // boss
  TFT_DRAW_RECTANGLE(boss_prev.x - 4, boss_prev.y - 4, boss_prev.x + 4, boss_prev.y + 4, 0x000);
  TFT_DRAW_RECTANGLE(boss.x - 4, boss.y - 4, boss.x + 4, boss.y + 4, 0xF00);

  // save state to overwrite later
  player_prev.x = player.x;
  player_prev.y = player.y;
  boss_prev.x = boss.x;
  boss_prev.y = boss.y;
}
void erase_game_screen() {}

#endif