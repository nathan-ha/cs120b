#include "../lib/helper.h"
#include "../lib/spiAVR.h"

int main() {
  SPI_INIT();  // Initialize SPI
  TFT_INIT();  // Initialize ST7735 display

  // TFT_DRAW_PIXEL(120, 120, 0xF00);

  // for (int i = 0; i < 128; i++) {
  //   for (int j = 0; j < 128; j++) {
  //     TFT_DRAW_PIXEL(i, j, 0xFFF);
  //   }
  // }

  // TFT_DRAW_RECTANGLE(64, 64, 127, 100, 0xFFF);

  short x1 = 10, y1 = 50, dir_flag = 0, dir_flagy = 0;

  // TFT_DRAW_RECTANGLE(0, 0, 127, 127, 0xFFF);

  // TFT_SEND_COMMAND(0x12);  // partial mode

  while (1) {
    // for (int i = 0; i < 64; i++) {
    //   for (int j = 0; j < 64; j++) {
    //     TFT_DRAW_PIXEL(i, j, 0x00F);
    //   }
    // }
    // TFT_FLUSH();
    // for (int i = 0; i < 128; i++) {
    //   TFT_DRAW_PIXEL(i, i * i, 0xFF);
    // }

    // for (short i = 64; i < 80; i++) {
    //   for (short j = 64; j < 80; j++) {
    //     TFT_DRAW_PIXEL(i, j, 0xfff);
    //   }
    // }

    // TFT_FLUSH();
    // for (int i = 0; i < 128; i++) {
    //   TFT_DRAW_PIXEL(i, i*i, 0x0F0);
    // }

    // TFT_DRAW_PIXEL(127, 127, 0xFFF);
    TFT_DRAW_RECTANGLE(x1, y1, x1 + 10, y1 + 10, 0xFFF);
    _delay_ms(30);
    TFT_DRAW_RECTANGLE(x1, y1, x1 + 10, y1 + 10, 0);

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
  }
}