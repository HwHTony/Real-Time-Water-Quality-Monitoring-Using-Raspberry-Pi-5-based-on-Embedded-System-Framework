#ifndef TFT_FREETYPE_H
#define TFT_FREETYPE_H

#include "../common/com.h"
#include "../common/constants.h"


/**
 * @brief TFT显示屏驱动类（集成FreeType字体渲染功能）
 * 负责TFT屏幕的硬件初始化、图形绘制及文字渲染，基于SPI通信和GPIO控制
 * 支持Unicode字符显示，适用于需要高质量文字渲染的嵌入式场景
 */
class TFTFreetype {
public:
    /**
     * @brief 构造函数
     * 初始化GPIO、SPI、LCD显示屏及FreeType字体引擎
     * 开启屏幕背光并初始化显示状态
     */
    TFTFreetype();

    /**
     * @brief 析构函数
     * 释放FreeType资源、关闭SPI设备及GPIO引脚，清理硬件资源
     */
    ~TFTFreetype();

    /**
     * @brief 在指定位置绘制宽字符串（支持Unicode）
     * @param x 绘制起始X坐标（左上角）
     * @param y 绘制起始Y坐标（左上角）
     * @param str 待绘制的宽字符串（wchar_t类型，支持中文、英文等）
     * @param fg 文字前景色（16位RGB565格式）
     */
    void drawString(uint8_t x, uint8_t y, const wchar_t *str, uint16_t fg);

    /**
     * @brief 填充整个屏幕为指定颜色
     * @param color 填充颜色（16位RGB565格式）
     */
    void fillScreen(uint16_t color);

    /**
     * @brief 刷新指定区域为指定颜色
     * @param color 填充颜色（16位RGB565格式）
     * @param x 区域左上角X坐标
     * @param y 区域左上角Y坐标
     * @param w 区域宽度
     * @param h 区域高度
     */
    void freshScreen(uint16_t color, int x, int y, int w, int h);

private:
    int spi_fd;                      ///< SPI设备文件描述符
    uint8_t spi_mode;                ///< SPI通信模式（默认为SPI_MODE_3）
    uint32_t spi_speed;              ///< SPI通信速率（默认为6MHz）
    const char *gpio_chip;           ///< GPIO芯片路径（默认为"gpiochip0"）
    FT_Library library;              ///< FreeType字体库实例
    FT_Face face;                    ///< FreeType字体face对象（字体实例）
    FT_GlyphSlot glyph_slot;         ///< FreeType当前字形槽（存储当前渲染的字形信息）
    struct gpiod_chip *chip;         ///< GPIO芯片句柄
    struct gpiod_line *rst_line;     ///< 屏幕复位引脚句柄
    struct gpiod_line *dc_line;      ///< 数据/命令切换引脚句柄
    struct gpiod_line *blk_line;     ///< 背光控制引脚句柄
    struct gpiod_line *cs_line;      ///< SPI片选引脚句柄

    /**
     * @brief 初始化GPIO控制引脚
     * 打开GPIO芯片，获取复位、数据/命令、背光、SPI片选引脚并配置为输出模式
     */
    void gpioInit();

    /**
     * @brief 初始化SPI通信接口
     * 打开SPI设备文件，配置通信模式和速率
     */
    void spiInit();

    /**
     * @brief 向LCD发送命令
     * @param cmd 待发送的命令字节
     */
    void sendCommand(uint8_t cmd);

    /**
     * @brief 向LCD发送数据
     * @param data 待发送的数据缓冲区
     * @param len 数据长度（字节数）
     */
    void sendData(uint8_t *data, int len);

    /**
     * @brief 初始化LCD显示屏硬件
     * 执行复位序列，配置显示方向、颜色格式等参数，开启显示
     */
    void lcdInit();

    /**
     * @brief 设置LCD显示窗口（绘图区域）
     * @param x0 窗口左上角X坐标
     * @param y0 窗口左上角Y坐标
     * @param x1 窗口右下角X坐标
     * @param y1 窗口右下角Y坐标
     */
    void setWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

    /**
     * @brief 加载指定字符的字形数据（FreeType内部操作）
     * @param c 待加载的宽字符
     */
    void loadGlyph(wchar_t c);

    /**
     * @brief 在指定位置绘制单个宽字符
     * @param x 绘制起始X坐标
     * @param y 绘制起始Y坐标
     * @param c 待绘制的宽字符
     * @param fg 字符前景色（16位RGB565格式）
     */
    void drawChar(uint8_t x, uint8_t y, wchar_t c, uint16_t fg);

    /**
     * @brief 初始化FreeType字体引擎
     * @param font_path 字体文件路径（如ttf字体文件）
     * @param font_size 字体大小（像素）
     */
    void freetypeInit(const char *font_path, int font_size);
};

#endif