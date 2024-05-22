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

    for (short i = 64; i < 80; i++) {
      for (short j = 64; j < 80; j++) {
        TFT_DRAW_PIXEL(i, j, 0xfff);
      }
    }
    // TFT_FLUSH();
  }
}