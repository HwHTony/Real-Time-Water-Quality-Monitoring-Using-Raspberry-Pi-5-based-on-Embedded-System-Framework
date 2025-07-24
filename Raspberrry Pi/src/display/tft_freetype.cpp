#include "tft_freetype.h"
#include <iostream>
#include <cstdlib>

/**
 * @brief 构造函数，初始化TFT屏幕及相关资源
 * 初始化GPIO、SPI通信、LCD显示屏和FreeType字体库，点亮背光并清屏
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
 * @brief 析构函数，释放所有资源
 * 释放FreeType库资源、关闭SPI设备、释放GPIO引脚和芯片资源
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
 * @brief 初始化GPIO资源
 * 打开GPIO芯片，获取控制引脚（复位、数据/命令、背光、片选）并配置为输出模式
 */
void TFTFreetype::gpioInit() {
    chip = gpiod_chip_open_by_name(gpio_chip);
    rst_line = gpiod_chip_get_line(chip, RST_PIN);
    dc_line = gpiod_chip_get_line(chip, DC_PIN);
    blk_line = gpiod_chip_get_line(chip, BLK_PIN);
    cs_line = gpiod_chip_get_line(chip, CS_PIN);

    // 配置为输出模式
    gpiod_line_request_output(rst_line, "rst", 0);
    gpiod_line_request_output(dc_line, "dc", 0);
    gpiod_line_request_output(blk_line, "blk", 0);
    gpiod_line_request_output(cs_line, "cs", 1); // 默认禁用SPI
}

/**
 * @brief 初始化SPI通信
 * 打开SPI设备文件，配置SPI通信速度
 */
void TFTFreetype::spiInit() {
    spi_fd = open(SPI_DEV, O_RDWR);
    // ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
}

/**
 * @brief 向LCD发送控制命令
 * @param cmd 要发送的控制命令
 */
void TFTFreetype::sendCommand(uint8_t cmd) {
    gpiod_line_set_value(cs_line, 0);
    gpiod_line_set_value(dc_line, 0);
    write(spi_fd, &cmd, 1);
    gpiod_line_set_value(cs_line, 1);
}

/**
 * @brief 向LCD发送数据
 * @param data 数据缓冲区指针
 * @param len 数据长度
 */
void TFTFreetype::sendData(uint8_t *data, int len) {
    gpiod_line_set_value(dc_line, 1);
    gpiod_line_set_value(cs_line, 0);
    write(spi_fd, data, len);
    gpiod_line_set_value(cs_line, 1);
}

/**
 * @brief 初始化LCD显示屏
 * 执行硬件复位，配置显示方向、颜色模式等参数，最终开启显示
 */
void TFTFreetype::lcdInit() {
    // 硬件复位
    gpiod_line_set_value(rst_line, 0);
    usleep(100000);
    gpiod_line_set_value(rst_line, 1);
    usleep(120000);

    sendCommand(0x11);  // Sleep out
    usleep(500000);

    sendCommand(0x36);  // MADCTL
    uint8_t madctl[] = {0xC8}; // RGB顺序调整
    sendData(madctl, 1);

    sendCommand(0x3A);  // 颜色模式
    uint8_t colmod[] = {0x05}; // RGB565
    sendData(colmod, 1);

    sendCommand(0x29);  // 开启显示
}

/**
 * @brief 设置LCD显示窗口（区域）
 * @param x0 窗口左上角x坐标
 * @param y0 窗口左上角y坐标
 * @param x1 窗口右下角x坐标
 * @param y1 窗口右下角y坐标
 */
void TFTFreetype::setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    sendCommand(0x2A);
    uint8_t col_data[] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};  // 高位在前
    sendData(col_data, 4);

    sendCommand(0x2B);
    uint8_t row_data[] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};
    sendData(row_data, 4);
}

/**
 * @brief 用指定颜色填充整个屏幕
 * @param color 填充颜色（RGB565格式）
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
 * @brief 用指定颜色刷新指定区域
 * @param color 刷新颜色（RGB565格式）
 * @param x 区域左上角x坐标
 * @param y 区域左上角y坐标
 * @param w 区域宽度
 * @param h 区域高度
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
 * @brief 初始化FreeType字体库
 * @param font_path 字体文件路径
 * @param font_size 字体大小（像素）
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
 * @brief 加载指定字符的字形数据
 * @param c 要加载的字符（宽字符）
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
 * @brief 在指定位置绘制单个字符
 * @param x 绘制起点x坐标
 * @param y 绘制起点y坐标
 * @param c 要绘制的字符（宽字符）
 * @param fg 字符前景色（RGB565格式）
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
 * @brief 在指定位置绘制字符串
 * @param x 绘制起点x坐标
 * @param y 绘制起点y坐标
 * @param str 要绘制的字符串（宽字符）
 * @param fg 字符串前景色（RGB565格式）
 * @note 包含自动换行处理，超出屏幕宽度时换行
 */
void TFTFreetype::drawString(uint8_t x, uint8_t y, const wchar_t *str, uint16_t fg) {
    while (*str) {
        drawChar(x, y, *str++, fg);
        x += glyph_slot->advance.x >> 6; // 每个像素6个位
        if (x >= 160 - 8) { // 换行处理
            x = 0;
            y += glyph_slot->advance.y >> 6;
        }
    }
}