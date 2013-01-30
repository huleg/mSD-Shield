#ifndef MI0283QT9_h
#define MI0283QT9_h


#ifdef __cplusplus
extern "C" {
#endif
  #include <inttypes.h>
  #include <avr/pgmspace.h>
  #include "fonts.h"
#ifdef __cplusplus
}
#endif
#include "Print.h"


#ifndef RGB
#define RGB(r,g,b)   (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //5 red | 6 green | 5 blue
#endif

#ifndef COLOR_WHITE
#define COLOR_WHITE  RGB(255,255,255)
#endif
#ifndef COLOR_BLACK
#define COLOR_BLACK  RGB(  0,  0,  0)
#endif
#ifndef COLOR_RED
#define COLOR_RED    RGB(255,  0,  0)
#endif
#ifndef COLOR_GREEN
#define COLOR_GREEN  RGB(  0,255,  0)
#endif
#ifndef COLOR_BLUE
#define COLOR_BLUE   RGB(  0,  0,255)
#endif
#ifndef COLOR_YELLOW
#define COLOR_YELLOW RGB(255,255,  0)
#endif

#ifndef DEC
# define DEC (10)
#endif
#ifndef HEX
# define HEX (16)
#endif
#ifndef OCT
# define OCT (8)
#endif
#ifndef BIN
# define BIN (2)
#endif

#define LCD_CMD_NOP                    0x00
#define LCD_CMD_RESET                  0x01
#define LCD_CMD_SLEEPIN                0x10
#define LCD_CMD_SLEEPOUT               0x11
#define LCD_CMD_PARTIAL_MODE           0x12
#define LCD_CMD_NORMAL_MODE            0x13
#define LCD_CMD_INV_OFF                0x20
#define LCD_CMD_INV_ON                 0x21
#define LCD_CMD_GAMMA                  0x26
#define LCD_CMD_DISPLAY_OFF            0x28
#define LCD_CMD_DISPLAY_ON             0x29
#define LCD_CMD_COLUMN                 0x2A
#define LCD_CMD_PAGE                   0x2B
#define LCD_CMD_WRITE                  0x2C
#define LCD_CMD_READ                   0x2E
#define LCD_CMD_PARTIAL_AREA           0x30
#define LCD_CMD_TEARING_OFF            0x34
#define LCD_CMD_TEARING_ON             0x35
#define LCD_CMD_MEMACCESS_CTRL         0x36
#define LCD_CMD_IDLE_OFF               0x38
#define LCD_CMD_IDLE_ON                0x39
#define LCD_CMD_PIXEL_FORMAT           0x3A
#define LCD_CMD_WRITE_CNT              0x3C
#define LCD_CMD_READ_CNT               0x3E
#define LCD_CMD_BRIGHTNESS             0x51
#define LCD_CMD_BRIGHTNESS_CTRL        0x53
#define LCD_CMD_RGB_CTRL               0xB0
#define LCD_CMD_FRAME_CTRL             0xB1 //normal mode
#define LCD_CMD_FRAME_CTRL_IDLE        0xB2 //idle mode
#define LCD_CMD_FRAME_CTRL_PART        0xB3 //partial mode
#define LCD_CMD_INV_CTRL               0xB4
#define LCD_CMD_DISPLAY_CTRL           0xB6
#define LCD_CMD_ENTRY_MODE             0xB7
#define LCD_CMD_POWER_CTRL1            0xC0
#define LCD_CMD_POWER_CTRL2            0xC1
#define LCD_CMD_VCOM_CTRL1             0xC5
#define LCD_CMD_VCOM_CTRL2             0xC7
#define LCD_CMD_POWER_CTRLA            0xCB
#define LCD_CMD_POWER_CTRLB            0xCF
#define LCD_CMD_POS_GAMMA              0xE0
#define LCD_CMD_NEG_GAMMA              0xE1
#define LCD_CMD_DRV_TIMING_CTRLA       0xE8
#define LCD_CMD_DRV_TIMING_CTRLB       0xEA
#define LCD_CMD_POWERON_SEQ_CTRL       0xED
#define LCD_CMD_ENABLE_3G              0xF2
#define LCD_CMD_INTERF_CTRL            0xF6
#define LCD_CMD_PUMP_RATIO_CTRL        0xF7


class MI0283QT9 : public Print
{
  public:
    uint16_t lcd_orientation;
    uint16_t lcd_width, lcd_height;

    MI0283QT9();
    void init(uint8_t clock_div); //2 4 8 16 32
    void led(uint8_t power); //0-100

    void setOrientation(uint16_t o); //0 90 180 270
    uint16_t getWidth(void);
    uint16_t getHeight(void);
    void setArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void setCursor(uint16_t x, uint16_t y);

    void clear(uint16_t color);
    void drawStart(void);
    void draw(uint16_t color);
    void drawStop(void);
    void drawPixel(uint16_t x0, uint16_t y0, uint16_t color);
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
    void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
    void fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);

    uint16_t drawChar(uint16_t x, uint16_t y, char c, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, char *s, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, int i, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, unsigned int i, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, long l, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, unsigned long l, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, String &s, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawTextPGM(uint16_t x, uint16_t y, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color);

    uint16_t drawMLText(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *s, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawMLText(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, String &s, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawMLTextPGM(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color);

    uint16_t drawInteger(uint16_t x, uint16_t y, char val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color); //base = DEC, HEX, OCT, BIN
    uint16_t drawInteger(uint16_t x, uint16_t y, unsigned char val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawInteger(uint16_t x, uint16_t y, int val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawInteger(uint16_t x, uint16_t y, long val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);

    void printOptions(uint8_t size, uint16_t color, uint16_t bg_color);
    void printClear(void);
    void printXY(uint16_t x, uint16_t y);
    uint16_t printGetX(void);
    uint16_t printGetY(void);
    void printPGM(PGM_P s);
    virtual size_t write(uint8_t c);
    virtual size_t write(const char *s);
    virtual size_t write(const uint8_t *s, size_t size);

  private:
    uint8_t p_size;
    uint16_t p_fg, p_bg;
    uint16_t p_x, p_y;

    void reset(void);
    void wr_cmd(uint8_t cmd);
    void wr_data16(uint16_t data);
    void wr_data(uint8_t data);
    void wr_spi(uint8_t data);
    void delay_10ms(uint8_t ms);
};


#endif //MI0283QT9_h
