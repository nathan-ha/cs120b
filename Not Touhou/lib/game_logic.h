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

#include <stdlib.h>
#include <time.h>

#include "../lib/helper.h"
#include "../lib/irAVR.h"
#include "../lib/music.h"
#include "../lib/periph.h"
#include "../lib/serialATmega.h"
#include "../lib/spiAVR.h"
#include "../lib/state_machines.h"
#include "../lib/timerISR.h"

#define PIN_JOYSTICK_X PORTC0
#define PIN_JOYSTICK_Y PORTC1
#define PIN_JOYSTICK_SW PORTC2

// shared variables
short health_player = 300;
short health_boss = 500;
char lcd_message_top[16] = "";
char lcd_message_bottom[16] = "";
unsigned char elapsed_time_seconds = 111;
unsigned short elapsed_time_ms = 222;
long ir_value = 0;
char win = 0;
char boss_phase = 1;
char bomb_count = 3;
unsigned int song_tick = 0;

struct vector {
  float x;
  float y;
  char x_dir;
  char y_dir;
  float speed;
};

struct vector player;
struct vector boss;

struct vector boss_bullets[30];
struct vector player_bullets[10];

const int boss_bullet_size = sizeof(boss_bullets) / sizeof(boss_bullets[0]);
const int player_bullets_size = sizeof(player_bullets) / sizeof(player_bullets[0]);

struct point {
  float x;
  float y;
};

struct point boss_prev;
struct point player_prev;

void game_init() {
  srand(time(NULL));

  health_player = 300;
  health_boss = 500;
  boss_phase = 1;
  bomb_count = 3;

  player.x = 64;
  player.y = 20;
  player.x_dir = 0;  // -1 for -x, 0 for no move, 1 for +x
  player.y_dir = 0;
  player.speed = 5;
  song_tick = 0;

  boss.x = 64;
  boss.y = 100;
  boss.x_dir = 0;
  boss.y_dir = 0;
  boss.speed = 1;

  win = 0;

  boss_prev.x = boss.x;
  boss_prev.y = boss.y;
  player_prev.x = player.x;
  player_prev.y = player.y;

  for (short i = 0; i < boss_bullet_size; i++) {
    boss_bullets[i].x = -1;
    boss_bullets[i].y = -1;
    boss_bullets[i].x_dir = 0;
    boss_bullets[i].y_dir = 0;
    boss_bullets[i].speed = rand_incl(2, 4);
  }

  for (short i = 0; i < player_bullets_size; i++) {
    player_bullets[i].x = -1;
    player_bullets[i].y = -1;
    player_bullets[i].x_dir = 0;
    player_bullets[i].y_dir = 0;
    player_bullets[i].speed = 10;
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

enum directions_4 { UP, DOWN, LEFT, RIGHT };
void player_shoot_bullet(directions_4 dir) {
  for (short i = 0; i < player_bullets_size; i++) {
    if (player_bullets[i].x >= 0) continue;
    player_bullets[i].x = player.x;
    player_bullets[i].y = player.y;
    // bullet direction
    // TODO: add multiple directions
    if (dir == UP) {
      player_bullets[i].x_dir = 0;
      player_bullets[i].y_dir = 1;
    }
    return;
  }
}

void boss_phase_1_init() {
  TFT_DRAW_RECTANGLE_SLOW(0, 0, 131, 131, 0xBAF);
  TFT_FLUSH();
  health_boss = 200;
  boss.speed = 3;
  boss.y = 100;
  boss.x = 64;
  for (short i = 0; i < boss_bullet_size; i++) {
    boss_bullets[i].speed = rand_incl(3, 5);
    boss_bullets[i].x = -1;
    boss_bullets[i].y = -1;
  }
}

void boss_phase_2_init() {
  TFT_DRAW_RECTANGLE_SLOW(0, 0, 131, 131, 0xFAB);
  TFT_FLUSH();
  health_boss = 1000;
  boss.speed = 0.5;
  boss.y = 100;
  boss.x = 64;
  for (short i = 0; i < boss_bullet_size; i++) {
    boss_bullets[i].speed = rand_incl(1, 3);
    boss_bullets[i].x = -1;
    boss_bullets[i].y = -1;
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

  const char player_shoot = 1;

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
  boss_shoot_bullet();
  for (short i = 0; i < boss_bullet_size; i++) {
    if (boss_bullets[i].x < 0) continue;  // skip invalid bullets
    boss_bullets[i].x += boss_bullets[i].speed * boss_bullets[i].x_dir;
    boss_bullets[i].y += boss_bullets[i].speed * boss_bullets[i].y_dir;
    if (boss_bullets[i].x > 128 || boss_bullets[i].x < 0) boss_bullets[i].x = -1;
    if (boss_bullets[i].y > 128 || boss_bullets[i].y < 0) boss_bullets[i].x = -1;
    // player gets hit
    if (fabs(boss_bullets[i].x - player.x) < 5 && fabs(boss_bullets[i].y - player.y) < 5) health_player -= 300;
  }

  // move player bullet logic
  for (short i = 0; i < player_bullets_size; i++) {
    if (player_bullets[i].x < 0) continue;  // skip invalid bullets
    player_bullets[i].x += player_bullets[i].speed * player_bullets[i].x_dir;
    player_bullets[i].y += player_bullets[i].speed * player_bullets[i].y_dir;
    if (player_bullets[i].x > 125 || player_bullets[i].x < 0) player_bullets[i].x = -1;
    if (player_bullets[i].y > 125 || player_bullets[i].y < 0) player_bullets[i].x = -1;
    // boss gets hit
    if (fabs(player_bullets[i].x - boss.x) < 5 && fabs(player_bullets[i].y - boss.y) < 5) {
      health_boss -= 20;
      player_bullets[i].x = -1;
    }
  }

  // player shoot bullet logic
  if (player_shoot) {
    player_shoot_bullet(UP);
  }
  // boss death
  if (health_boss <= 0) {
    if (boss_phase == 1) {  // phase 1: fast boy
      boss_phase_1_init();
    } else if (boss_phase == 2) {  // tanky boy
      boss_phase_2_init();
    } else {
      win = 1;
    }
    boss_phase++;
  }

  // bomb/special
  if (press && bomb_count > 0) {
    TFT_DRAW_RECTANGLE_SLOW(0, 0, 131, 131, 0x0FF);
    TFT_FLUSH();
    for (short i = 0; i < boss_bullet_size; i++) {
      boss_bullets[i].x = -1;
      boss_bullets[i].y = -1;
    }
    bomb_count--;
  }
}

void draw_game_screen() {
  // player
  TFT_DRAW_RECTANGLE(player_prev.x - 2, player_prev.y - 2, player_prev.x + 2, player_prev.y + 2,
                     0x000);  // erase old pixels
  for (short x = player.x - 2; x < player.x + 2; x++) {
    for (short y = player.y - 2; y < player.y + 2; y++) {
      TFT_DRAW_PIXEL(x, y, 0x0AF);
    }
  }

  // boss
  TFT_DRAW_RECTANGLE(boss_prev.x - 4, boss_prev.y - 4, boss_prev.x + 4, boss_prev.y + 4, 0x000);
  for (short x = boss.x - 4; x < boss.x + 4; x++) {
    for (short y = boss.y - 4; y < boss.y + 4; y++) {
      TFT_DRAW_PIXEL(x, y, 0xF00);
    }
  }

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

  // player bullets
  for (short i = 0; i < player_bullets_size; i++) {
    if (player_bullets[i].x >= 0) {
      const float bullet_x_offset = player_bullets[i].speed * player_bullets[i].x_dir;
      const float bullet_y_offset = player_bullets[i].speed * player_bullets[i].y_dir;
      TFT_DRAW_RECTANGLE(player_bullets[i].x - bullet_x_offset - 1, player_bullets[i].y - bullet_y_offset - 1,
                         player_bullets[i].x - bullet_x_offset + 1, player_bullets[i].y - bullet_y_offset + 1, 0x000);

      for (short x = player_bullets[i].x - 1; x < player_bullets[i].x + 1; x++) {
        for (short y = player_bullets[i].y - 1; y < player_bullets[i].y + 1; y++) {
          TFT_DRAW_PIXEL(x, y, 0x0FF);
        }
      }
    }
  }
  // save state to overwrite later
  player_prev.x = player.x;
  player_prev.y = player.y;
  boss_prev.x = boss.x;
  boss_prev.y = boss.y;
}

// message displayed during gameplay
void gameplay_message() {
  char tmp[16] = "";
  strncpy(lcd_message_top, "Boss hp:", 16 - 1);
  lcd_message_top[16 - 1] = '\0';

  snprintf(tmp, sizeof(tmp), "%d", health_boss);
  strncat(lcd_message_top, tmp, 16 - strlen(lcd_message_top) - 1);

  strncpy(lcd_message_bottom, "Time: ", 16 - 1);
  lcd_message_bottom[16 - 1] = '\0';

  snprintf(tmp, sizeof(tmp), "%d", elapsed_time_seconds);
  strncat(lcd_message_bottom, tmp, 16 - strlen(lcd_message_bottom) - 1);
  strncat(lcd_message_bottom, ".", 16 - strlen(lcd_message_bottom) - 1);

  snprintf(tmp, sizeof(tmp), "%d", elapsed_time_ms);
  strncat(lcd_message_bottom, tmp, 16 - strlen(lcd_message_bottom) - 1);
  strncat(lcd_message_bottom, " s", 16 - strlen(lcd_message_bottom) - 1);
}

// message displayed after dying
void death_message() {
  char tmp[16] = "";
  strncpy(lcd_message_top, "You Died...L", 16 - 1);
  lcd_message_top[16 - 1] = '\0';

  strncpy(lcd_message_bottom, "Time: ", 16 - 1);
  lcd_message_bottom[16 - 1] = '\0';

  snprintf(tmp, sizeof(tmp), "%d", elapsed_time_seconds);
  strncat(lcd_message_bottom, tmp, 16 - strlen(lcd_message_bottom) - 1);
  strncat(lcd_message_bottom, ".", 16 - strlen(lcd_message_bottom) - 1);

  snprintf(tmp, sizeof(tmp), "%d", elapsed_time_ms);
  strncat(lcd_message_bottom, tmp, 16 - strlen(lcd_message_bottom) - 1);
  strncat(lcd_message_bottom, " s", 16 - strlen(lcd_message_bottom) - 1);
}

void pause_message() {
  char tmp[16] = "";
  strncpy(lcd_message_top, "PAUSED", 16 - 1);
  lcd_message_top[16 - 1] = '\0';

  strncpy(lcd_message_bottom, "Time: ", 16 - 1);
  lcd_message_bottom[16 - 1] = '\0';

  snprintf(tmp, sizeof(tmp), "%d", elapsed_time_seconds);
  strncat(lcd_message_bottom, tmp, 16 - strlen(lcd_message_bottom) - 1);
  strncat(lcd_message_bottom, ".", 16 - strlen(lcd_message_bottom) - 1);

  snprintf(tmp, sizeof(tmp), "%d", elapsed_time_ms);
  strncat(lcd_message_bottom, tmp, 16 - strlen(lcd_message_bottom) - 1);
  strncat(lcd_message_bottom, " s", 16 - strlen(lcd_message_bottom) - 1);
}

void win_message() {
  char tmp[16] = "";
  strncpy(lcd_message_top, "YOU'RE WINNER!!!!!", 16 - 1);
  lcd_message_top[16 - 1] = '\0';

  strncpy(lcd_message_bottom, "Time: ", 16 - 1);
  lcd_message_bottom[16 - 1] = '\0';

  snprintf(tmp, sizeof(tmp), "%d", elapsed_time_seconds);
  strncat(lcd_message_bottom, tmp, 16 - strlen(lcd_message_bottom) - 1);
  strncat(lcd_message_bottom, ".", 16 - strlen(lcd_message_bottom) - 1);

  snprintf(tmp, sizeof(tmp), "%d", elapsed_time_ms);
  strncat(lcd_message_bottom, tmp, 16 - strlen(lcd_message_bottom) - 1);
  strncat(lcd_message_bottom, " s", 16 - strlen(lcd_message_bottom) - 1);
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