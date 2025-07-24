#ifndef TFT_FREETYPE_H
#define TFT_FREETYPE_H

#include "../common/com.h"
#include "../common/constants.h"


/**
 * @brief TFT display driver class (integrated FreeType font rendering function)
 * Responsible for hardware initialization, graphics drawing and text rendering of TFT screen, based on SPI communication and GPIO control
 * Supports Unicode character display, suitable for embedded scenarios that require high-quality text rendering
 */
class TFTFreetype {
public:
    /**
     * @brief Constructor
     * Initialize GPIO, SPI, LCD display and FreeType font engine
     * Turn on the screen backlight and initialize the display status
     */
    TFTFreetype();

    /**
     * @brief Destructor
     * Release FreeType resources, close SPI devices and GPIO pins, and clean up hardware resources
     */
    ~TFTFreetype();

    /**
     * @brief Draw a wide string at the specified position (Unicode supported)
     * @param x Draw the starting X coordinate (upper left corner)
     * @param y Draw the starting Y coordinate (upper left corner)
     * @param str The wide string to be drawn (wchar_t type, supports Chinese, English, etc.)
     * @param fg Text foreground color (16-bit RGB565 format)
     */
    void drawString(uint8_t x, uint8_t y, const wchar_t *str, uint16_t fg);

    /**
     * @brief Fill the entire screen with the specified color
     * @param color Fill color (16-bit RGB565 format)
     */
    void fillScreen(uint16_t color);

    /**
     * @brief Refresh the specified area to the specified color
     * @param color Fill color (16-bit RGB565 format)
     * @param x X coordinate of the upper left corner of the region
     * @param y Y coordinate of the upper left corner of the area
     * @param w Area Width
     * @param h Area Height
     */
    void freshScreen(uint16_t color, int x, int y, int w, int h);

private:
    int spi_fd;                      ///< SPI device file descriptor
    uint8_t spi_mode;                ///< SPI communication mode (default is SPI_MODE_3)
    uint32_t spi_speed;              ///< SPI communication rate (default is 6MHz)
    const char *gpio_chip;           ///< GPIO chip path (default is "gpiochip0")
    FT_Library library;              ///< FreeType font library example
    FT_Face face;                    ///< FreeType font face object (font instance)
    FT_GlyphSlot glyph_slot;         ///< FreeType Current glyph slot (stores the glyph information currently being rendered)
    struct gpiod_chip *chip;         ///< GPIO Chip handle
    struct gpiod_line *rst_line;     ///< Screen reset pin handle
    struct gpiod_line *dc_line;      ///< Data/command switching pin handle
    struct gpiod_line *blk_line;     ///< Backlight control pin handle
    struct gpiod_line *cs_line;      ///< SPI chip select pin handle

    /**
     * @brief Initialize GPIO control pins
     * Open the GPIO chip, obtain the reset, data/command, backlight, and SPI chip select pins and configure them as output mode
     */
    void gpioInit();

    /**
     * @brief Initialize the SPI communication interface
     * Open the SPI device file and configure the communication mode and rate
     */
    void spiInit();

    /**
     * @brief Sending commands to the LCD
     * @param cmd Command byte to be sent
     */
    void sendCommand(uint8_t cmd);

    /**
     * @brief Sending data to LCD
     * @param data Buffer of data to be sent
     * @param len Data length (bytes)
     */
    void sendData(uint8_t *data, int len);

    /**
     * @brief Initialize LCD display hardware
     * Execute the reset sequence, configure the display direction, color format and other parameters, and turn on the display
     */
    void lcdInit();

    /**
     * @brief Set up the LCD display window (drawing area)
     * @param x0 X coordinate of the upper left corner of the window
     * @param y0 Y coordinate of the upper left corner of the window
     * @param x1 X coordinate of the lower right corner of the window
     * @param y1 Y coordinate of the lower right corner of the window
     */
    void setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

    /**
     * @brief Load the glyph data of the specified character (FreeType internal operation)
     * @param c wide character to load
     */
    void loadGlyph(wchar_t c);

    /**
     * @brief Draws a single wide character at the specified position
     * @param x Draw the starting X coordinate
     * @param y Draw the starting Y coordinate
     * @param c The wide character to be drawn
     * @param fg Character foreground color (16-bit RGB565 format)
     */
    void drawChar(uint8_t x, uint8_t y, wchar_t c, uint16_t fg);

    /**
     * @brief Initialize the FreeType font engine
     * @param font_path Font file path (such as ttf font file)
     * @param font_size Font size (pixels)
     */
    void freetypeInit(const char *font_path, int font_size);
};

#endif
