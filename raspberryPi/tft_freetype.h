#ifndef TFT_FREETYPE_H
#define TFT_FREETYPE_H

#include "com.h"

// // 初始化 tft, spi, free_type
// void tft_init();
// void tft_clear();

// // 绘制字符串
// void draw_string(uint8_t x, uint8_t y, const wchar_t *str, uint16_t fg);
// // 刷屏
// void fill_screen(uint16_t color); 
// void fresh_screen(uint16_t color, int x, int y, int w, int h) ;

class TFTFreetype {
    public:
        TFTFreetype();
        ~TFTFreetype();
        void drawString(uint8_t x, uint8_t y, const wchar_t *str, uint16_t fg);
        void fillScreen(uint16_t color);
        void freshScreen(uint16_t color, int x, int y, int w, int h);
    private:
        int spi_fd;
        uint8_t spi_mode = SPI_MODE_3;
        uint32_t spi_speed = 6000000;
        const char *gpio_chip = "gpiochip0";
        FT_Library library;
        FT_Face face;
        FT_GlyphSlot glyph_slot;
        struct gpiod_chip *chip;
        struct gpiod_line *rst_line, *dc_line, *blk_line, *cs_line;
        void gpioInit();
        void spiInit();
        void sendCommand(uint8_t cmd);
        void sendData(uint8_t *data, int len);
        void lcdInit();
        void setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
        void loadGlyph(wchar_t c);
        void drawChar(uint8_t x, uint8_t y, wchar_t c, uint16_t fg);
        void freetypeInit(const char *font_path, int font_size);
    };

#endif