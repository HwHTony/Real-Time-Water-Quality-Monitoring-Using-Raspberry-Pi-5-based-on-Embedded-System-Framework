#ifndef TFT_FREETYPE_H
#define TFT_FREETYPE_H

#include "com.h"

//init tft, spi, free_type
void tft_init();
void tft_clear();

// draw string
void draw_string(uint8_t x, uint8_t y, const wchar_t *str, uint16_t fg);
// flood the screen
void fill_screen(uint16_t color); 
void fresh_screen(uint16_t color, int x, int y, int w, int h) ;

#endif