#include "tft_freetype.h"
#include <iostream>
#include <cstdlib>

/**
 * @brief Constructor to initialize the TFT screen and related resources
 * Initialize GPIO, SPI communication, LCD display and FreeType font library, turn on the backlight and clear the screen
 */
TFTFreetype::TFTFreetype() {
    gpioInit();
    spiInit();
    lcdInit();
    freetypeInit("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
    gpiod_line_set_value(blk_line, 1);
    usleep(50000);
    fillScreen(0x0000);
    usleep(2000000);
}

/**
 * @brief Destructor, releases all resources
 * Release FreeType library resources, close SPI devices, release GPIO pins and chip resources
 */
TFTFreetype::~TFTFreetype() {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    close(spi_fd);
    gpiod_line_release(rst_line);
    gpiod_line_release(dc_line);
    gpiod_line_release(blk_line);
    gpiod_line_release(cs_line);
    gpiod_chip_close(chip);
}

/**
 * @brief Initialize GPIO resources
 * Open the GPIO chip, get the control pins (reset, data/command, backlight, chip select) and configure them as output mode
 */
void TFTFreetype::gpioInit() {
    chip = gpiod_chip_open_by_name(gpio_chip);
    rst_line = gpiod_chip_get_line(chip, RST_PIN);
    dc_line = gpiod_chip_get_line(chip, DC_PIN);
    blk_line = gpiod_chip_get_line(chip, BLK_PIN);
    cs_line = gpiod_chip_get_line(chip, CS_PIN);

    // Configured as output mode
    gpiod_line_request_output(rst_line, "rst", 0);
    gpiod_line_request_output(dc_line, "dc", 0);
    gpiod_line_request_output(blk_line, "blk", 0);
    gpiod_line_request_output(cs_line, "cs", 1); // SPI is disabled by default
}

/**
 * @brief Initialize SPI communication
 * Open the SPI device file and configure the SPI communication speed
 */
void TFTFreetype::spiInit() {
    spi_fd = open(SPI_DEV, O_RDWR);
    // ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
}

/**
 * @brief Send control commands to the LCD
 * @param cmd Control command to send
 */
void TFTFreetype::sendCommand(uint8_t cmd) {
    gpiod_line_set_value(cs_line, 0);
    gpiod_line_set_value(dc_line, 0);
    write(spi_fd, &cmd, 1);
    gpiod_line_set_value(cs_line, 1);
}

/**
 * @brief Sending data to LCD
 * @param data Data buffer pointer
 * @param len Data length
 */
void TFTFreetype::sendData(uint8_t *data, int len) {
    gpiod_line_set_value(dc_line, 1);
    gpiod_line_set_value(cs_line, 0);
    write(spi_fd, data, len);
    gpiod_line_set_value(cs_line, 1);
}

/**
 * @brief Initialize LCD display
 * Perform a hardware reset, configure display direction, color mode and other parameters, and finally turn on the display
 */
void TFTFreetype::lcdInit() {
    // Hardware Reset
    gpiod_line_set_value(rst_line, 0);
    usleep(100000);
    gpiod_line_set_value(rst_line, 1);
    usleep(120000);

    sendCommand(0x11);  // Sleep out
    usleep(500000);

    sendCommand(0x36);  // MADCTL
    uint8_t madctl[] = {0xC8}; // RGB order adjustment
    sendData(madctl, 1);

    sendCommand(0x3A);  // Color Mode
    uint8_t colmod[] = {0x05}; // RGB565
    sendData(colmod, 1);

    sendCommand(0x29);  // Turn on display
}

/**
 * @brief Set LCD display window (area)
 * @param x0 The x coordinate of the upper left corner of the window
 * @param y0 The y coordinate of the upper left corner of the window
 * @param x1 The x coordinate of the lower right corner of the window
 * @param y1 The y coordinate of the lower right corner of the window
 */
void TFTFreetype::setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    sendCommand(0x2A);
    uint8_t col_data[] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};  // High position first
    sendData(col_data, 4);

    sendCommand(0x2B);
    uint8_t row_data[] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};
    sendData(row_data, 4);
}

/**
 * @brief Fill the entire screen with the specified color
 * @param color Fill color (RGB565 format)
 */
void TFTFreetype::fillScreen(uint16_t color) {
    uint8_t color_hi = color >> 8;
    uint8_t color_lo = color & 0xFF;
    uint8_t buf[128];

    setWindow(0, 0, 128, 160);
    sendCommand(0x2C);

    for (int i = 0; i < 128; i += 2) {
        buf[i] = color_hi;
        buf[i + 1] = color_lo;
    }
    for (int y = 0; y < 128; y++) {
        for (int x = 0; x < 160 * 2; x += 128) {
            sendData(buf, 128);
        }
    }
}

/**
 * @brief Refresh the specified area with the specified color
 * @param color Refresh color (RGB565 format)
 * @param x The x coordinate of the upper left corner of the region
 * @param y The y coordinate of the upper left corner of the region
 * @param w Area Width
 * @param h Area Height
 */
void TFTFreetype::freshScreen(uint16_t color, int x, int y, int w, int h) {
    uint8_t color_hi = color >> 8;
    uint8_t color_lo = color & 0xFF;
    uint8_t buf[128];

    setWindow(x, y, w, h);
    sendCommand(0x2C);

    for (int i = 0; i < w; i += 2) {
        buf[i] = color_hi;
        buf[i + 1] = color_lo;
    }
    for (int y = 0; y < w; y++) {
        for (int x = 0; x < h * 2; x += w) {
            sendData(buf, w);
        }
    }
}

/**
 * @brief Initialize the FreeType font library
 * @param font_path Font file path
 * @param font_size Font size (pixels)
 */
void TFTFreetype::freetypeInit(const char *font_path, int font_size) {
    FT_Error error = FT_Init_FreeType(&library);
    if (error) {
        std::cerr << "FT_Init_FreeType error: " << error << std::endl;
        std::exit(1);
    }

    error = FT_New_Face(library, font_path, 0, &face);
    if (error) {
        std::cerr << "FT_New_Face error: " << error << std::endl;
        std::exit(1);
    }

    error = FT_Set_Pixel_Sizes(face, 0, font_size);
    if (error) {
        std::cerr << "FT_Set_Pixel_Sizes error: " << error << std::endl;
        std::exit(1);
    }
}

/**
 * @brief Loads the glyph data for the specified character
 * @param c character to load (wide character)
 */
void TFTFreetype::loadGlyph(wchar_t c) {
    FT_UInt glyph_index = FT_Get_Char_Index(face, c);
    if (glyph_index == 0) {
        std::cerr << "FT_Get_Char_Index error: character not found" << std::endl;
        std::exit(1);
    }

    FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
    glyph_slot = face->glyph;
}

/**
 * @brief Draws a single character at the specified position
 * @param x Draw the starting point x coordinate
 * @param y Draw the starting point y coordinate
 * @param c The character to be drawn (wide character)
 * @param fg Character foreground color (RGB565 format)
 */
void TFTFreetype::drawChar(uint8_t x, uint8_t y, wchar_t c, uint16_t fg) {
    loadGlyph(c);

    int bitmap_left = glyph_slot->bitmap_left;
    int bitmap_top = glyph_slot->bitmap_top;
    int bitmap_width = glyph_slot->bitmap.width;
    int bitmap_rows = glyph_slot->bitmap.rows;

    setWindow(x + bitmap_left, y - bitmap_top, x + bitmap_left + bitmap_width - 1, y - bitmap_top + bitmap_rows - 1);
    sendCommand(0x2C);

    uint8_t *bitmap_data = glyph_slot->bitmap.buffer;
    for (int row = 0; row < bitmap_rows; row++) {
        for (int col = 0; col < bitmap_width; col++) {
            uint8_t pixel = bitmap_data[row * bitmap_width + col];
            uint16_t color = (pixel > 0) ? fg : 0x0000;
            uint8_t color_hi = color >> 8;
            uint8_t color_lo = color & 0xFF;
            sendData(&color_hi, 1);
            sendData(&color_lo, 1);
        }
    }
}

/**
 * @brief Draws a string at the specified position
 * @param x Draw the starting point x coordinate
 * @param y Draw the starting point y coordinate
 * @param str The string to draw (in wide characters)
 * @param fg String foreground color (RGB565 format)
 * @note Contains automatic line wrapping processing, wrapping when exceeding the screen width
 */
void TFTFreetype::drawString(uint8_t x, uint8_t y, const wchar_t *str, uint16_t fg) {
    while (*str) {
        drawChar(x, y, *str++, fg);
        x += glyph_slot->advance.x >> 6; // 6 bits per pixel
        if (x >= 160 - 8) { // Line break processing
            x = 0;
            y += glyph_slot->advance.y >> 6;
        }
    }
}
