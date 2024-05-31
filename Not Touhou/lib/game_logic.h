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

#include <stdlib.h>
#include <time.h>

#define PIN_JOYSTICK_X PORTC0
#define PIN_JOYSTICK_Y PORTC1
#define PIN_JOYSTICK_SW PORTC2

// shared variables
short health_player = 300;
short health_boss = 1000;
char lcd_message_top[16] = "";
char lcd_message_bottom[16] = "";
unsigned char elapsed_time_seconds = 111;
unsigned short elapsed_time_ms = 222;

struct vector {
  float x;
  float y;
  char x_dir;
  char y_dir;
  float speed;
};

struct vector player;
struct vector boss;

struct vector boss_bullets[80];
struct vector player_bullets[10];

const int boss_bullet_size = sizeof(boss_bullets) / sizeof(boss_bullets[0]);
const int player_bullets_size = sizeof(player_bullets) / sizeof(player_bullets[0]);

struct point {
  float x;
  float y;
};
struct point boss_prev;
struct point player_prev;
struct point player_bullets_prev[player_bullets_size];

void game_init() {
  srand(time(NULL));

  player.x = 64;
  player.y = 20;
  player.x_dir = 0;  // -1 for -x, 0 for no move, 1 for +x
  player.y_dir = 0;
  player.speed = 1.5;

  boss.x = 100;
  boss.y = 64;
  boss.x_dir = 0;
  boss.y_dir = 0;
  boss.speed = 0.4;

  boss_prev.x = boss.x;
  boss_prev.y = boss.y;
  player_prev.x = player.x;
  player_prev.y = player.y;

  for (short i = 0; i < boss_bullet_size; i++) {
    boss_bullets[i].x = -1;
    boss_bullets[i].y = -1;
    boss_bullets[i].x_dir = 0;
    boss_bullets[i].y_dir = 0;
    boss_bullets[i].speed = rand_incl(0.3, 1.2);
  }

  for (short i = 0; i < player_bullets_size; i++) {
    player_bullets[i].x = -1;
    player_bullets[i].y = -1;
    player_bullets[i].x_dir = 0;
    player_bullets[i].y_dir = 0;
    player_bullets[i].speed = 6;
  }

  for (short i = 0; i < player_bullets_size; i++) {
    player_bullets_prev[i].x = -1;
    player_bullets_prev[i].y = -1;
  }
}
void boss_shoot_bullet() {
  // look for empty bullet slot and put bullet there
  for (short i = 0; i < boss_bullet_size; i++) {
    if (boss_bullets[i].x < 0) {
      boss_bullets[i].x = boss.x;
      boss_bullets[i].y = boss.y;

      // bullet direction
      // x
      if (fabs(boss.x - player.x) <= 5 && boss.y < player.y && boss.y < 120) {
        boss_bullets[i].y_dir = 1;
        boss_bullets[i].x_dir = 0;
      } else if (fabs(boss.x - player.x) <= 5 && boss.y > player.y && boss.y > 0) {
        boss_bullets[i].y_dir = -1;
        boss_bullets[i].x_dir = 0;
      } else if (boss.x < player.x && boss.x < 120) {
        boss_bullets[i].x_dir = 1;
      } else if (boss.x > player.x && boss.x > 0) {
        boss_bullets[i].x_dir = -1;
      }
      // y
      if (fabs(boss.y - player.y) <= 5 && boss.x < player.x && boss.x < 120) {
        boss_bullets[i].y_dir = 0;
        boss_bullets[i].x_dir = 1;
      } else if (fabs(boss.y - player.y) <= 5 && boss.x > player.x && boss.x > 0) {
        boss_bullets[i].y_dir = 0;
        boss_bullets[i].x_dir = -1;
      } else if (boss.y < player.y && boss.y < 120) {
        boss_bullets[i].y_dir = 1;
      } else if (boss.y > player.y && boss.y > 0) {
        boss_bullets[i].y_dir = -1;
      }

      return;
    }
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

  // move boss bullet logic
  static char i = 0;
  if (i == 0) boss_shoot_bullet();
  for (short i = 0; i < boss_bullet_size; i++) {
    if (boss_bullets[i].x < 0) continue;  // skip invalid bullets
    boss_bullets[i].x += boss_bullets[i].speed * boss_bullets[i].x_dir;
    boss_bullets[i].y += boss_bullets[i].speed * boss_bullets[i].y_dir;
    if (boss_bullets[i].x > 128 || boss_bullets[i].x < 0) boss_bullets[i].x = -1;
    if (boss_bullets[i].y > 128 || boss_bullets[i].y < 0) boss_bullets[i].x = -1;
    // player gets hit
    if (abs(boss_bullets[i].x - player.x) < 3 && abs(boss_bullets[i].y - player.y) < 3) health_player -= 300;
  }
  i = (i + 1) % 5;  // shoot bullet every 5 ticks

  // bomb/special
  if (press) {
    TFT_DRAW_RECTANGLE(0, 0, 131, 131, 0xF00);
    TFT_FLUSH();
  }
}

void draw_game_screen() {
  // player
  TFT_DRAW_RECTANGLE(player_prev.x - 2, player_prev.y - 2, player_prev.x + 2, player_prev.y + 2,
                     0x000);                                                          // erase old pixels
  TFT_DRAW_RECTANGLE(player.x - 2, player.y - 2, player.x + 2, player.y + 2, 0xFFF);  // draw new pixels

  // boss
  TFT_DRAW_RECTANGLE(boss_prev.x - 4, boss_prev.y - 4, boss_prev.x + 4, boss_prev.y + 4, 0x000);
  TFT_DRAW_RECTANGLE(boss.x - 4, boss.y - 4, boss.x + 4, boss.y + 4, 0x00F);

  // boss bullets
  for (short i = 0; i < boss_bullet_size; i++) {
    if (boss_bullets[i].x >= 0) {
      const float bullet_x_offset = boss_bullets[i].speed * boss_bullets[i].x_dir;
      const float bullet_y_offset = boss_bullets[i].speed * boss_bullets[i].y_dir;
      TFT_DRAW_RECTANGLE(boss_bullets[i].x - bullet_x_offset - 1, boss_bullets[i].y - bullet_y_offset - 1,
                         boss_bullets[i].x - bullet_x_offset + 1, boss_bullets[i].y - bullet_y_offset + 1, 0x000);

      TFT_DRAW_RECTANGLE(boss_bullets[i].x - 1, boss_bullets[i].y - 1, boss_bullets[i].x + 1, boss_bullets[i].y + 1,
                         0xFF0);
    }
  }
  // // save state to overwrite later
  player_prev.x = player.x;
  player_prev.y = player.y;
  boss_prev.x = boss.x;
  boss_prev.y = boss.y;

  // TODO optimimize this away
  for (short i = 0; i < player_bullets_size; i++) {
    player_bullets_prev[i].x = player_bullets[i].x;
    player_bullets_prev[i].y = player_bullets[i].y;
  }
}

// message displayed during gameplay
void gameplay_message() {
  char tmp[3] = "";
  strcpy(lcd_message_top, "Boss health:");
  sprintf(tmp, "%d", health_boss);
  strcat(lcd_message_top, tmp);

  strcpy(lcd_message_bottom, "Time: ");
  sprintf(tmp, "%d", elapsed_time_seconds);
  strcat(lcd_message_bottom, tmp);
  strcat(lcd_message_bottom, ".");
  sprintf(tmp, "%d", elapsed_time_ms);
  strcat(lcd_message_bottom, tmp);
  strcat(lcd_message_bottom, " s");
}

// message displayed after dying
void death_message() {
  strcpy(lcd_message_top, "You Died...L");

  char tmp[3];
  strcpy(lcd_message_bottom, "Time: ");
  sprintf(tmp, "%d", elapsed_time_seconds);
  strcat(lcd_message_bottom, tmp);
  strcat(lcd_message_bottom, ".");
  sprintf(tmp, "%d", elapsed_time_ms);
  strcat(lcd_message_bottom, tmp);
  strcat(lcd_message_bottom, " s");
}

// used for testing
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

#endif