# mSD-Shield

French description and additional functions:http://www.mon-club-elec.fr/pmwiki_reference_arduino/pmwiki.php?n=Main.LibrairieMI0283QT2Afficheur


## Installation

Copy the content of */libraries/* to your Arduino lib folder */arduino/libraries/* or to your user documents folder */My Documents/Arduino/libraries/*
If there are existing folders from a previous installation, please delete them before copying.

Further infos: http://arduino.cc/en/Guide/Libraries and http://arduino.cc/en/Hacking/Libraries


## Known Issues

If using the Ethernet-Shield together with the mSD-Shield, this must be initialized before using the SD-Card.

__mSD-Shield v1 (not mSD-Shield Mega-Edition):__

For Hardware-SPI support on Mega boards connect the mSD-Shield v1 as follows.
No Software changes are required.

         Mega   mSD-Shield v1
    SCK   52   ->   13
    MOSI  51   ->   11
    MISO  50   ->   12
    
Or if you dont want to make any pin changes, but use the mSD-Shield on the Mega, then Software-SPI has to be enabled in the Arduino libs. Uncomment the following lines:

    libraries/ADS7846/ADS7846.cpp:40 (#define SOFTWARE_SPI)
    libraries/MIO283QT2/MIO283QT2.cpp:28 (#define SOFTWARE_SPI)
    libraries/MIO283QT9/MIO283QT9.cpp:24 (#define SOFTWARE_SPI)
    libraries/SDcard/mmc.h:8 (#define SD_SOFTWARE_SPI)

For using the RTC on Mega boards the I2C pins have to be changed.

         Mega  mSD-Shield v1
    SDA  20   ->   A4
    SCL  21   ->   A5


## Libraries


### SDcard

    void init(uint8_t clock_div)
* clock_div - SPI clock divider: 2, 4, 8, 16, 32 (SPI clock = Main clock / clock_div)

Initialize the microcontroller peripherals.

    void service(void)
Service routine. This function has to be called every 10 milliseconds (100 Hz).

    uint8_t mount(void)
Initialize the memory card and mount it. The return value is 0 if the mounting failed, otherwise it is mounted successfully.

    void unmount(void)
Unmount the memory card.

See also the FatFS docu for more information about the file system functions (*f_open()*...):
*/libraries/SDcard/doc/00index_e.html* or http://elm-chan.org/fsw/ff/00index_e.html
To enable LFN support (long file names) set ```_USE_LFN``` to **1** in */libraries/SDcard/ffconf.h*. The *Demo2* has also LFN support.


### DS1307

    void start(void)
Start RTC.

    void stop(void)
Stop RTC.

    void get(uint8_t *sec, uint8_t *min, uint8_t *hour, uint8_t *day, uint8_t *month, uint16_t *year)
* sec - Second (pointer)
* min - Minute (pointer)
* hour - Hour (pointer)
* day - Day (pointer)
* month - Month (pointer)
* year - Year (pointer)

Get time and date from RTC.

    void set(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, uint16_t year)
* sec - Second
* min - Minute
* hour - Hour
* day - Day
* month - Month
* year - Year

Set time and date to RTC.


### MI0283QT2 / MI0283QT9

    void init(uint8_t clock_div)
* clock_div - SPI clock divider: 2, 4, 8, 16, 32 (SPI clock = Main clock / clock_div)

Initialize the microcontroller peripherals.

    void led(uint8_t power)
* power - Power: 0...100 (0-100%)

Backlight power control.

    void setOrientation(uint16_t o)
* o - Orientation: 0 90 180 270 (0 is default)

Set display orientation.

    uint16_t getWidth(void)
Returns display width. (depends on orientation)

    uint16_t getHeight(void)
Returns display height. (depends on orientation)

    void setArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
* x0 - X position of 1st point (left top)
* y0 - Y position of 1st point (left top)
* x1 - X position of 2nd point (right bottom)
* y1 - Y position of 2nd point (right bottom)

Set draw area.

    void setCursor(uint16_t x, uint16_t y)
* x - X position
* y - Y position

Set cursor.

    void clear(uint16_t color)
* color - Color

Clear the complete screen/display.

    void drawStart(void)
Select display for drawing.

    void draw(uint16_t color)
* color - Color

Draw pixel.

    void drawStop(void)
Deselect display from drawing.

    void drawPixel(uint16_t x0, uint16_t y0, uint16_t color)
* x0 - X position
* y0 - Y position
* color - Color

Draw pixel at position XY.

    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
* x0 - X position of 1st point
* y0 - Y position of 1st point
* x1 - X position of 2nd point
* y1 - Y position of 2nd point
* color - Color

Draw line.

    void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
* x0 - X position of 1st point
* y0 - Y position of 1st point
* x1 - X position of 2nd point
* y1 - Y position of 2nd point
* color - Color

Draw rectangle.

    void fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
* x0 - X position of 1st point
* y0 - Y position of 1st point
* x1 - X position of 2nd point
* y1 - Y position of 2nd point
* color - Color

Draw filled rectangle.

    void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
* x0 - X position of middle point
* y0 - Y position of middle point
* radius - Radius
* color - Color

Draw a circle.

    void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
* x0 - X position of middle point
* y0 - Y position of middle point
* radius - Radius
* color - Color

Draw filled circle.

    uint16_t drawChar(uint16_t x, uint16_t y, char c, uint8_t size, uint16_t color, uint16_t bg_color)
* x - X position (left top)
* y - Y position (left top)
* c - Character (RAM)
* size - Font size scale-up (1=normal size, 2=double size...)
* color - Font color
* bg_color - Background color

Draw single character from RAM, returns the X end position of the character.

    uint16_t drawText(uint16_t x, uint16_t y, text, uint8_t size, uint16_t color, uint16_t bg_color)
* x - X position (left top)
* y - Y position (left top)
* text - String, Integer, Long (RAM)
* size - Font size scale-up (1=normal size, 2=double size...)
* color - Font color
* bg_color - Background color

Draw text from RAM, returns the X end position of the text.

    uint16_t drawTextPGM(uint16_t x, uint16_t y, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color)
* x - X position (left top)
* y - Y position (left top)
* s - Pointer to String/Array (Flash), example: PSTR("test"), further infos about using flash memory: http://www.nongnu.org/avr-libc/user-manual/pgmspace.html
* size - Font size scale-up (1=normal size, 2=double size...)
* color - Font color
* bg_color - Background color

Draw text from Flash (PGM), returns the X end position of the character.

    uint16_t drawMLText(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, text, uint8_t size, uint16_t color, uint16_t bg_color)
* x0 - X position of 1st point
* y0 - Y position of 1st point
* x1 - X position of 2nd point
* y1 - Y position of 2nd point
* text - String/Array (RAM)
* size - Font size scale-up (1=normal size, 2=double size...)
* color - Font color
* bg_color - Background color

Draw multi-line text (line end = "\n") from RAM, returns the X end position of the text.

    uint16_t drawMLTextPGM(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color)
* x0 - X position of 1st point
* y0 - Y position of 1st point
* x1 - X position of 2nd point
* y1 - Y position of 2nd point
* s - Pointer to String/Array (Flash), example: PSTR("test"), further infos about using flash memory: http://www.nongnu.org/avr-libc/user-manual/pgmspace.html
* size - Font size scale-up (1=normal size, 2=double size...)
* color - Font color
* bg_color - Background color

Draw multi-line text (line end = "\n") from Flash (PGM), returns the X end position of the character.

    uint16_t drawInteger(uint16_t x, uint16_t y, val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color)
* x - X position (left top)
* y - Y position (left top)
* val - Char, Int, Long
* base - Base (DEC, HEX, OCT, BIN)
* size - Font size scale-up (1=normal size, 2=double size...)
* color - Font color
* bg_color - Background color

Draw integer, returns the X end position of the text.

#### Print functions:

    void printOptions(uint8_t size, uint16_t color, uint16_t bg_color)
* size - Font size
* color - Font color
* bg_color - Background color
 
Set print options.

    void printClear(void)
Clear screen and set print cursor to the upper left corner.

    void printXY(uint16_t x, uint16_t y)
* x - X position (left top)
* y - Y position (left top)

Set print cursor.

    uint16_t printGetX(void)
Returns current X position.

    uint16_t printGetY(void)
Returns current Y position.

    void printPGM(PGM_P s)
* s - Pointer to String/Array (Flash), example: PSTR("test"), further infos about using flash memory: http://www.nongnu.org/avr-libc/user-manual/pgmspace.html

Print text/data from Flash (PGM)

    void print(text)
    void print(int, base)
    void println(text)
    void println(int, base)
* text, int - String, Char, Integer, Long, Float (RAM)
* base - DEC, HEX, OCT, BIN

Print text from RAM.

#### Note

The text functions can draw characters from 0x20 to 0x7F and the default font is 8x12.
To change the font to 8x8 uncomment the ```FONT_8X8``` definition and comment the ```FONT_8X12``` definition in */libraries/MI0283QTx/fonts.h*
To support characters from 0x20 to 0xFF comment the ```FONT_END7F``` definition in */libraries/MI0283QTx/fonts.h*.

Use Umlaut characters as hex values, for example "Test \x84 \x94 \x81 \x8E \x99 \x9A" to display "Test ä ö ü Ä Ö Ü"

    0x84 = ä, 0x8E = Ä, 0x94 = ö, 0x99 = Ö, 0x81 = ü, 0x9A = Ü*


### ADS7846

    void init(void)
 Initialize the microcontroller peripherals.

    void setOrientation(uint16_t o)
* o - Orientation: 0 90 180 270 (0 is default)

Set display orientation.

    uint8_t setCalibration(CAL_POINT *lcd, CAL_POINT *tp)
* lcd - Point coordinates from the display screen
* tp - Point coordinates from the touch panel (ADS7846)

Set touch panel calibration points. Returns 0 on an error.

    uint8_t writeCalibration(unsigned int eeprom_addr)
* eeprom_addr - EEPROM address for writing

Write calibration data to EEPROM. Returns 0 on an error.

    uint8_t readCalibration(unsigned int eeprom_addr)
* eeprom_addr - EEPROM address for reading

Read calibration data from EEPROM. Returns 0 on an error.

    uint8_t doCalibration(MI0283QT2 *lcd, uint16_t eeprom_addr, uint8_t check_eeprom)
    uint8_t doCalibration(MI0283QT9 *lcd, uint16_t eeprom_addr, uint8_t check_eeprom)
* lcd - Pointer to display class
* eeprom_addr - EEPROM address for reading
* check_eeprom - Check EEPROM for calibration data

Do touch panel calibration. Returns 0 if the calibration data was read from EEPROM.

    uint16_t getX(void)
Returns last X position.

    uint16_t getY(void)
Returns last Y position.

    uint16_t getXraw(void)
Returns last raw X position (uncalibrated).

    uint16_t getYraw(void)
Returns last raw Y position (uncalibrated).

    uint8_t getPressure(void)
Returns actual pressure.

    void service(void)
Service routine. This function should be called in loop().
