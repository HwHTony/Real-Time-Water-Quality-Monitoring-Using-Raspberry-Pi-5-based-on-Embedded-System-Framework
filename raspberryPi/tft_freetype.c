#include "tft_freetype.h"

// init tft screen and offer freetype interface

int spi_fd;
uint8_t spi_mode = SPI_MODE_3;
uint32_t spi_speed = 6000000;


// GPIO flag（raspberry pi default）
const char *gpio_chip = "gpiochip0";

// FreeType related varies
FT_Library library;
FT_Face face;
FT_GlyphSlot glyph_slot;

// init GPIO struct
struct gpiod_chip *chip;
struct gpiod_line *rst_line, *dc_line, *blk_line, *cs_line;

// init GPIO
static void gpio_init() {
    chip = gpiod_chip_open_by_name(gpio_chip);
    rst_line = gpiod_chip_get_line(chip, RST_PIN);
    dc_line = gpiod_chip_get_line(chip, DC_PIN);
    blk_line = gpiod_chip_get_line(chip, BLK_PIN);
    cs_line = gpiod_chip_get_line(chip, CS_PIN);

    // set output
    gpiod_line_request_output(rst_line, "rst", 0);
    gpiod_line_request_output(dc_line, "dc", 0);
    gpiod_line_request_output(blk_line, "blk", 0);
    gpiod_line_request_output(cs_line, "cs", 1); // 默认禁用SPI
}

// SPI init
static void spi_init() {
    spi_fd = open(SPI_DEV, O_RDWR);
    // ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
}

// send command（DC=0）
static void send_command(uint8_t cmd) {
    gpiod_line_set_value(cs_line, 0);
    gpiod_line_set_value(dc_line, 0);
    write(spi_fd, &cmd, 1);
    gpiod_line_set_value(cs_line, 1);
}

// send data（DC=1）
static void send_data(uint8_t *data, int len) {
    gpiod_line_set_value(dc_line, 1);
    gpiod_line_set_value(cs_line, 0);
    write(spi_fd, data, len);
    gpiod_line_set_value(cs_line, 1);
}

// init tft
static void lcd_init() {
    // reset hardware
    gpiod_line_set_value(rst_line, 0);
    usleep(100000);
    gpiod_line_set_value(rst_line, 1);
    usleep(120000);

    send_command(0x11);  // Sleep out
    usleep(500000);
    
    send_command(0x36);  // MADCTL
    uint8_t madctl[] = {0xC8}; // RGB turn changed
    send_data(madctl, 1);
    
    send_command(0x3A);  // color mode
    uint8_t colmod[] = {0x05}; // RGB565
    send_data(colmod, 1);
    
    send_command(0x29);  // open show
}

// set display area
static void set_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    send_command(0x2A);
    uint8_t col_data[] = {x0>>8, x0&0xFF, x1>>8, x1&0xFF};  // MSB first
    send_data(col_data, 4);
    
    send_command(0x2B);
    uint8_t row_data[] = {y0>>8, y0&0xFF, y1>>8, y1&0xFF};
    send_data(row_data, 4);
}

// fill out color（RGB565 mode）
void fill_screen(uint16_t color) 
{
    uint8_t color_hi = color >> 8;
    uint8_t color_lo = color & 0xFF;
    uint8_t buf[128];
    
    set_window(0, 0, 128, 160);
    send_command(0x2C);
    
    for(int i=0; i<128; i+=2) {
        buf[i] = color_hi;
        buf[i+1] = color_lo;
    }
    for(int y=0; y<128; y++) {
        for(int x=0; x<160 * 2; x+=128) {
            send_data(buf, 128);
        }
    }
}

// fill out color（RGB565）
void fresh_screen(uint16_t color, int x, int y, int w, int h) 
{
    uint8_t color_hi = color >> 8;
    uint8_t color_lo = color & 0xFF;
    uint8_t buf[128];
    
    set_window(x, y, w, h);
    send_command(0x2C);
    
    for(int i=0; i<w; i+=2) {
        buf[i] = color_hi;
        buf[i+1] = color_lo;
    }
    for(int y=0; y<w; y++) {
        for(int x=0; x<h * 2; x+=w) {
            send_data(buf, w);
        }
    }
}

// init FreeType 
static void freetype_init(const char *font_path, int font_size) {
    FT_Error error = FT_Init_FreeType(&library);
    if (error) {
        printf("FT_Init_FreeType error: %d\n", error);
        exit(1);
    }
    
    error = FT_New_Face(library, font_path, 0, &face);
    if (error) {
        printf("FT_New_Face error: %d\n", error);
        exit(1);
    }
    
    error = FT_Set_Pixel_Sizes(face, 0, font_size);
    if (error) {
        printf("FT_Set_Pixel_Sizes error: %d\n", error);
        exit(1);
    }
}

// Load character bitmap
static void load_glyph(wchar_t c) {
    FT_UInt glyph_index = FT_Get_Char_Index(face, c);
    if (glyph_index == 0) {
        printf("FT_Get_Char_Index error: character not found\n");
        exit(1);
    }
    
    FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
    glyph_slot = face->glyph;
}

// draw character
static void draw_char(uint8_t x, uint8_t y, wchar_t c, uint16_t fg) {
    load_glyph(c);
    
    int bitmap_left = glyph_slot->bitmap_left;
    int bitmap_top = glyph_slot->bitmap_top;
    int bitmap_width = glyph_slot->bitmap.width;
    int bitmap_rows = glyph_slot->bitmap.rows;
    
    set_window(x + bitmap_left, y - bitmap_top, x + bitmap_left + bitmap_width - 1, y - bitmap_top + bitmap_rows - 1);
    send_command(0x2C);
    
    uint8_t *bitmap_data = glyph_slot->bitmap.buffer;
    for (int row = 0; row < bitmap_rows; row++) {
        for (int col = 0; col < bitmap_width; col++) {
            uint8_t pixel = bitmap_data[row * bitmap_width + col];
            uint16_t color = (pixel > 0) ? fg : 0x0000;
            uint8_t color_hi = color >> 8;
            uint8_t color_lo = color & 0xFF;
            send_data(&color_hi, 1);
            send_data(&color_lo, 1);
        }
    }
}

// draw string
void draw_string(uint8_t x, uint8_t y, const wchar_t *str, uint16_t fg) {
    while (*str) {
        draw_char(x, y, *str++, fg);
        x += glyph_slot->advance.x >> 6; // 6 bits per pixel
        if (x >= 160 - 8) { // Line wrapping processing
            x = 0;
            y += glyph_slot->advance.y >> 6;
        }
    }
}


void tft_init()
{
    gpio_init();
    spi_init();
    lcd_init();

    // init FreeType
    freetype_init("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    
    // Turn on backlight
    gpiod_line_set_value(blk_line, 1);
    usleep(50000);

    // clear screen
    fill_screen(0x0000);
    usleep(2000000);
}

void tft_clear()
{
    // free resource
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    close(spi_fd);
    gpiod_line_release(rst_line);
    gpiod_line_release(dc_line);
    gpiod_line_release(blk_line);
    gpiod_line_release(cs_line);
    gpiod_chip_close(chip);
}