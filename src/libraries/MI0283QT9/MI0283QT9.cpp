#ifdef __cplusplus
extern "C" {
#endif
  #include <inttypes.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include <util/delay.h>
#ifdef __cplusplus
}
#endif
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "../digitalWriteFast/digitalWriteFast.h"
#include "fonts.h"
#include "MI0283QT9.h"


#define PRINT_STARTX    (2)
#define PRINT_STARTY    (2)

//#define SOFTWARE_SPI

#if (defined(__AVR_ATmega1280__) || \
     defined(__AVR_ATmega1281__) || \
     defined(__AVR_ATmega2560__) || \
     defined(__AVR_ATmega2561__))      //--- Arduino Mega ---

# define LED_PIN        (9) //PH6
# define RST_PIN        (8)
# define CS_PIN         (7)
# if defined(SOFTWARE_SPI)
#  define MOSI_PIN      (11)
#  define MISO_PIN      (12)
#  define CLK_PIN       (13)
# else
#  define MOSI_PIN      (51)
#  define MISO_PIN      (50)
#  define CLK_PIN       (52)
# endif

#elif (defined(__AVR_ATmega644__) || \
       defined(__AVR_ATmega644P__))    //--- Arduino 644 (www.mafu-foto.de) ---

# define LED_PIN        (3) //PB3
# define RST_PIN        (12)
# define CS_PIN         (13)
# define MOSI_PIN       (5)
# define MISO_PIN       (6)
# define CLK_PIN        (7)

#elif defined(__AVR_ATmega32U4__)      //--- Arduino Leonardo ---

# define LED_PIN        (9) //PB5
# define RST_PIN        (8)
# define CS_PIN         (7)
# define MOSI_PIN       (16) //PB2
# define MISO_PIN       (14) //PB3
# define CLK_PIN        (15) //PB1

#else                                  //--- Arduino Uno ---

# define LED_PIN        (9) //PB1
# define RST_PIN        (8)
# define CS_PIN         (7)
# define MOSI_PIN       (11)
# define MISO_PIN       (12)
# define CLK_PIN        (13)

#endif


#define LED_ENABLE()    digitalWriteFast(LED_PIN, HIGH)
#define LED_DISABLE()   digitalWriteFast(LED_PIN, LOW)

#define RST_DISABLE()   digitalWriteFast(RST_PIN, HIGH)
#define RST_ENABLE()    digitalWriteFast(RST_PIN, LOW)

#define CS_DISABLE()    digitalWriteFast(CS_PIN, HIGH)
#define CS_ENABLE()     digitalWriteFast(CS_PIN, LOW)

#define MOSI_HIGH()     digitalWriteFast(MOSI_PIN, HIGH)
#define MOSI_LOW()      digitalWriteFast(MOSI_PIN, LOW)

#define MISO_READ()     digitalReadFast(MISO_PIN)

#define CLK_HIGH()      digitalWriteFast(CLK_PIN, HIGH)
#define CLK_LOW()       digitalWriteFast(CLK_PIN, LOW)


//-------------------- Constructor --------------------


MI0283QT9::MI0283QT9(void)
{
  return;
}


//-------------------- Public --------------------


void MI0283QT9::init(uint8_t clock_div)
{
  //init pins
  pinMode(LED_PIN, OUTPUT);
  digitalWriteFast(LED_PIN, LOW);
  led(50);
  pinMode(RST_PIN, OUTPUT);
  digitalWriteFast(RST_PIN, LOW);
  pinMode(CS_PIN, OUTPUT);
  digitalWriteFast(CS_PIN, HIGH);
  pinMode(CLK_PIN, OUTPUT);
  digitalWriteFast(CLK_PIN, LOW);
  pinMode(MOSI_PIN, OUTPUT);
  digitalWriteFast(MOSI_PIN, LOW);
  pinMode(MISO_PIN, INPUT);
  digitalWriteFast(MISO_PIN, HIGH); //pull-up

#if !defined(SOFTWARE_SPI)
  //SS has to be output or input with pull-up
# if (defined(__AVR_ATmega1280__) || \
      defined(__AVR_ATmega1281__) || \
      defined(__AVR_ATmega2560__) || \
      defined(__AVR_ATmega2561__))     //--- Arduino Mega ---
#  define SS_PORTBIT (0) //PB0
# elif (defined(__AVR_ATmega644__) || \
        defined(__AVR_ATmega644P__))   //--- Arduino 644 ---
#  define SS_PORTBIT (4) //PB4
# elif defined(__AVR_ATmega32U4__)     //--- Arduino Leonardo ---
#  define SS_PORTBIT (0) //PB0
# else                                 //--- Arduino Uno ---
#  define SS_PORTBIT (2) //PB2
# endif
  if(!(DDRB & (1<<SS_PORTBIT))) //SS is input
  {
      PORTB |= (1<<SS_PORTBIT); //pull-up on
  }

  //init hardware spi
  switch(clock_div)
  {
    case 2:
      SPCR = (1<<SPE)|(1<<MSTR); //enable SPI, Master, clk=Fcpu/4
      SPSR = (1<<SPI2X); //clk*2 = Fcpu/2
      break;
    case 4:
      SPCR = (1<<SPE)|(1<<MSTR); //enable SPI, Master, clk=Fcpu/4
      SPSR = (0<<SPI2X); //clk*2 = off
      break;
    case 8:
      SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //enable SPI, Master, clk=Fcpu/16
      SPSR = (1<<SPI2X); //clk*2 = Fcpu/8
      break;
    case 16:
      SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //enable SPI, Master, clk=Fcpu/16
      SPSR = (0<<SPI2X); //clk*2 = off
      break;
    case 32:
      SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1); //enable SPI, Master, clk=Fcpu/64
      SPSR = (1<<SPI2X); //clk*2 = Fcpu/32
      break;
  }
#endif

  //reset display
  reset();
  p_size = 0;
  p_fg   = COLOR_BLACK;
  p_bg   = COLOR_WHITE;

  return;
}


void MI0283QT9::led(uint8_t power)
{
  if(power == 0) //off
  {
    analogWrite(LED_PIN, 0);
    LED_DISABLE();
  }
  else if(power >= 100) //100%
  {
    analogWrite(LED_PIN, 255);
    LED_ENABLE();
  }
  else //1...99%
  {
    analogWrite(LED_PIN, (uint16_t)power*255/100);
  }

  return;
}


void MI0283QT9::setOrientation(uint16_t o)
{
  #define MEM_Y   (7) //MY row address order
  #define MEM_X   (6) //MX column address order 
  #define MEM_V   (5) //MV row / column exchange 
  #define MEM_L   (4) //ML vertical refresh order
  #define MEM_H   (2) //MH horizontal refresh order
  #define MEM_BGR (3) //RGB-BGR Order 

  wr_cmd(LCD_CMD_MEMACCESS_CTRL);

  switch(o)
  {
    default:
    case 0:
      lcd_orientation = 0;
      lcd_width  = 320;
      lcd_height = 240;
      wr_data((1<<MEM_BGR) | (1<<MEM_X) | (1<<MEM_Y) | (1<<MEM_V));
      break;

    case 90:
      lcd_orientation = 90;
      lcd_width  = 240;
      lcd_height = 320;
      wr_data((1<<MEM_BGR) | (1<<MEM_X));
      break;

    case 180:
      lcd_orientation = 180;
      lcd_width  = 320;
      lcd_height = 240;
      wr_data((1<<MEM_BGR) | (1<<MEM_L) | (1<<MEM_V));
      break;

    case 270:
      lcd_orientation = 270;
      lcd_width  = 240;
      lcd_height = 320;
      wr_data((1<<MEM_BGR) | (1<<MEM_Y));
      break;
  }
  
  setArea(0, 0, lcd_width-1, lcd_height-1);

  p_x = PRINT_STARTX;
  p_y = PRINT_STARTY;

  return;
}


uint16_t MI0283QT9::getWidth(void)
{
  return lcd_width;
}


uint16_t MI0283QT9::getHeight(void)
{
  return lcd_height;
}


void MI0283QT9::setArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  if((x1 >= lcd_width) ||
     (y1 >= lcd_height))
  {
    return;
  }

  wr_cmd(LCD_CMD_COLUMN);
  wr_data16(x0);
  wr_data16(x1);

  wr_cmd(LCD_CMD_PAGE);
  wr_data16(y0);
  wr_data16(y1);

  return;
}


void MI0283QT9::setCursor(uint16_t x, uint16_t y)
{
  setArea(x, y, x, y);

  return;
}


void MI0283QT9::clear(uint16_t color)
{
  uint16_t size;

  setArea(0, 0, lcd_width-1, lcd_height-1);

  drawStart();
  for(size=(320UL*240UL/8UL); size!=0; size--)
  {
    draw(color); //1
    draw(color); //2
    draw(color); //3
    draw(color); //4
    draw(color); //5
    draw(color); //6
    draw(color); //7
    draw(color); //8
  }
  drawStop();

  return;
}


void MI0283QT9::drawStart(void)
{
  wr_cmd(LCD_CMD_WRITE);
  CS_ENABLE();

  return;
}


void MI0283QT9::draw(uint16_t color)
{

  //9th bit
  MOSI_HIGH(); //data
  CLK_LOW();
#if defined(SOFTWARE_SPI)
  CLK_HIGH();
#else
  SPCR &= ~(1<<SPE); //disable SPI
  CLK_HIGH();
  SPCR |= (1<<SPE); //enable SPI
#endif

  wr_spi(color>>8);

  //9th bit
  MOSI_HIGH(); //data
  CLK_LOW();
#if defined(SOFTWARE_SPI)
  CLK_HIGH();
#else
  SPCR &= ~(1<<SPE); //disable SPI
  CLK_HIGH();
  SPCR |= (1<<SPE); //enable SPI
#endif

  wr_spi(color);

  return;
}


void MI0283QT9::drawStop(void)
{
  CS_DISABLE();

  return;
}


void MI0283QT9::drawPixel(uint16_t x0, uint16_t y0, uint16_t color)
{
  if((x0 >= lcd_width) ||
     (y0 >= lcd_height))
  {
    return;
  }

  setCursor(x0, y0);

  drawStart();
  draw(color);
  drawStop();

  return;
}


void MI0283QT9::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	int16_t dx, dy, dx2, dy2, err, stepx, stepy;

  if(x0 >= lcd_width)
  {
    x0 = lcd_width-1;
  }
  if(x1 >= lcd_width)
  {
    x1 = lcd_width-1;
  }
  if(y0 >= lcd_height)
  {
    y0 = lcd_height-1;
  }
  if(y1 >= lcd_height)
  {
    y1 = lcd_height-1;
  }

  if((x0 == x1) ||
     (y0 == y1)) //horizontal or vertical line
  {
    fillRect(x0, y0, x1, y1, color);
  }
  else
  {
    //calculate direction
    dx = x1 - x0;
    dy = y1 - y0;
    if(dx < 0) { dx = -dx; stepx = -1; } else { stepx = +1; }
    if(dy < 0) { dy = -dy; stepy = -1; } else { stepy = +1; }
    dx2 = dx << 1;
    dy2 = dy << 1;
    //draw line
    setArea(0, 0, lcd_width-1, lcd_height-1);
    drawPixel(x0, y0, color);
    if(dx > dy)
    {
      err = dy2 - dx;
      while(x0 != x1)
      {
        if(err >= 0)
        {
          y0  += stepy;
          err -= dx2;
        }
        x0  += stepx;
        err += dy2;
        drawPixel(x0, y0, color);
      }
    }
    else
    {
      err = dx2 - dy;
      while(y0 != y1)
      {
        if(err >= 0)
        {
          x0  += stepx;
          err -= dy2;
        }
        y0  += stepy;
        err += dx2;
        drawPixel(x0, y0, color);
      }
    }
  }

  return;
}


void MI0283QT9::drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  fillRect(x0, y0, x0, y1, color);
  fillRect(x0, y1, x1, y1, color);
  fillRect(x1, y0, x1, y1, color);
  fillRect(x0, y0, x1, y0, color);

  return;
}


void MI0283QT9::fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  uint32_t size;
  uint16_t tmp, i;

  if(x0 > x1)
  {
    tmp = x0;
    x0  = x1;
    x1  = tmp;
  }
  if(y0 > y1)
  {
    tmp = y0;
    y0  = y1;
    y1  = tmp;
  }

  if(x1 >= lcd_width)
  {
    x1 = lcd_width-1;
  }
  if(y1 >= lcd_height)
  {
    y1 = lcd_height-1;
  }

  setArea(x0, y0, x1, y1);

  drawStart();
  size = (uint32_t)(1+(x1-x0)) * (uint32_t)(1+(y1-y0));
  tmp  = size/8;
  if(tmp != 0)
  {
    for(i=tmp; i!=0; i--)
    {
      draw(color); //1
      draw(color); //2
      draw(color); //3
      draw(color); //4
      draw(color); //5
      draw(color); //6
      draw(color); //7
      draw(color); //8
    }
    for(i=size%8; i!=0; i--)
    {
      draw(color);
    }
  }
  else
  {
    for(i=size; i!=0; i--)
    {
      draw(color);
    }
  }
  drawStop();

  return;
}


void MI0283QT9::drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
{
  int16_t err, x, y;
  
  err = -radius;
  x   = radius;
  y   = 0;

  setArea(0, 0, lcd_width-1, lcd_height-1);

  while(x >= y)
  {
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);

    err += y;
    y++;
    err += y;
    if(err >= 0)
    {
      x--;
      err -= x;
      err -= x;
    }
  }

  return;
}


void MI0283QT9::fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)
{
  int16_t err, x, y;
  
  err = -radius;
  x   = radius;
  y   = 0;

  setArea(0, 0, lcd_width-1, lcd_height-1);

  while(x >= y)
  {
    drawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
    drawLine(x0 - x, y0 - y, x0 + x, y0 - y, color);
    drawLine(x0 - y, y0 + x, x0 + y, y0 + x, color);
    drawLine(x0 - y, y0 - x, x0 + y, y0 - x, color);

    err += y;
    y++;
    err += y;
    if(err >= 0)
    {
      x--;
      err -= x;
      err -= x;
    }
  }

  return;
}


uint16_t MI0283QT9::drawChar(uint16_t x, uint16_t y, char c, uint8_t size, uint16_t color, uint16_t bg_color)
{
  uint16_t ret;
#if FONT_WIDTH <= 8
  uint8_t data, mask;
#elif FONT_WIDTH <= 16
  uint16_t data, mask;
#elif FONT_WIDTH <= 32
  uint32_t data, mask;
#endif
  uint8_t i, j, width, height;
  const prog_uint8_t *ptr;

  i      = (uint8_t)c;
#if FONT_WIDTH <= 8
  ptr    = &font_PGM[(i-FONT_START)*(8*FONT_HEIGHT/8)];
#elif FONT_WIDTH <= 16
  ptr    = &font_PGM[(i-FONT_START)*(16*FONT_HEIGHT/8)];
#elif FONT_WIDTH <= 32
  ptr    = &font_PGM[(i-FONT_START)*(32*FONT_HEIGHT/8)];
#endif
  width  = FONT_WIDTH;
  height = FONT_HEIGHT;

  if(size <= 1)
  {
    ret = x+width;
    if((ret-1) >= lcd_width)
    {
      return lcd_width+1;
    }
    else if((y+height-1) >= lcd_height)
    {
      return lcd_width+1;
    }

    setArea(x, y, (x+width-1), (y+height-1));

    drawStart();
    for(; height!=0; height--)
    {
#if FONT_WIDTH <= 8
      data = pgm_read_byte(ptr); ptr+=1;
#elif FONT_WIDTH <= 16
      data = pgm_read_word(ptr); ptr+=2;
#elif FONT_WIDTH <= 32
      data = pgm_read_dword(ptr); ptr+=4;
#endif
      for(mask=(1<<(width-1)); mask!=0; mask>>=1)
      {
        if(data & mask)
        {
          draw(color);
        }
        else
        {
          draw(bg_color);
        }
      }
    }
    drawStop();
  }
  else
  {
    ret = x+(width*size);
    if((ret-1) >= lcd_width)
    {
      return lcd_width+1;
    }
    else if((y+(height*size)-1) >= lcd_height)
    {
      return lcd_width+1;
    }
    
    setArea(x, y, (x+(width*size)-1), (y+(height*size)-1));

    drawStart();
    for(; height!=0; height--)
    {
#if FONT_WIDTH <= 8
      data = pgm_read_byte(ptr); ptr+=1;
#elif FONT_WIDTH <= 16
      data = pgm_read_word(ptr); ptr+=2;
#elif FONT_WIDTH <= 32
      data = pgm_read_dword(ptr); ptr+=4;
#endif
      for(i=size; i!=0; i--)
      {
        for(mask=(1<<(width-1)); mask!=0; mask>>=1)
        {
          if(data & mask)
          {
            for(j=size; j!=0; j--)
            {
              draw(color);
            }
          }
          else
          {
            for(j=size; j!=0; j--)
            {
              draw(bg_color);
            }
          }
        }
      }
    }
    drawStop();
  }

  return ret;
}


uint16_t MI0283QT9::drawText(uint16_t x, uint16_t y, char *s, uint8_t size, uint16_t color, uint16_t bg_color)
{
  while(*s != 0)
  {
    x = drawChar(x, y, (char)*s++, size, color, bg_color);
    if(x > lcd_width)
    {
      break;
    }
  }

  return x;
}


uint16_t MI0283QT9::drawText(uint16_t x, uint16_t y, int i, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char tmp[16];

  itoa(i, tmp, 10);

  return drawText(x, y, tmp, size, color, bg_color);
}


uint16_t MI0283QT9::drawText(uint16_t x, uint16_t y, unsigned int i, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char tmp[16];

  utoa(i, tmp, 10);

  return drawText(x, y, tmp, size, color, bg_color);
}


uint16_t MI0283QT9::drawText(uint16_t x, uint16_t y, long l, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char tmp[32];

  ltoa(l, tmp, 10);

  return drawText(x, y, tmp, size, color, bg_color);
}


uint16_t MI0283QT9::drawText(uint16_t x, uint16_t y, unsigned long l, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char tmp[32];

  ultoa(l, tmp, 10);

  return drawText(x, y, tmp, size, color, bg_color);
}


uint16_t MI0283QT9::drawText(uint16_t x, uint16_t y, String &s, uint8_t size, uint16_t color, uint16_t bg_color)
{
  uint16_t i;

  for(i=0; i < s.length(); i++) 
  {
    x = drawChar(x, y, (char)s[i], size, color, bg_color);
    if(x > lcd_width)
    {
      break;
    }
  }

  return x;
}


uint16_t MI0283QT9::drawTextPGM(uint16_t x, uint16_t y, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char c;

  c = pgm_read_byte(s++);
  while(c != 0)
  {
    x = drawChar(x, y, c, size, color, bg_color);
    if(x > lcd_width)
    {
      break;
    }
    c = pgm_read_byte(s++);
  }

  return x;
}


uint16_t MI0283QT9::drawMLText(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *s, uint8_t size, uint16_t color, uint16_t bg_color)
{
  uint16_t x=x0, y=y0, wlen, llen;
  char c;
  char *wstart;

  fillRect(x0, y0, x1, y1, bg_color);

  llen   = (x1-x0)/(FONT_WIDTH*size); //line len in chars
  wstart = s;
  while(*s && (y < y1))
  {
    c = *s++;
    if(c == '\n') //new line
    {
      x  = x0;
      y += (FONT_HEIGHT*size)+1;
      continue;
    }
    else if(c == '\r') //skip
    {
      continue;
    }

    if(c == ' ') //start of a new word
    {
      wstart = s;
      if(x == x0) //do nothing
      {
        continue;
      }
    }

    if(c)
    {
      if((x+(FONT_WIDTH*size)) > x1) //new line
      {
        if(c == ' ') //do not start with space
        {
          x  = x0;
          y += (FONT_HEIGHT*size)+1;
        }
        else
        {
          wlen = (s-wstart);
          if(wlen > llen) //word too long
          {
            x  = x0;
            y += (FONT_HEIGHT*size)+1;
            if(y < y1)
            {
              x = drawChar(x, y, c, size, color, bg_color);
            }
          }
          else
          {
            fillRect(x-(wlen*FONT_WIDTH*size), y, x1, (y+(FONT_HEIGHT*size)), bg_color); //clear word
            x  = x0;
            y += (FONT_HEIGHT*size)+1;
            s  = wstart;
          }
        }
      }
      else
      {
        x = drawChar(x, y, c, size, color, bg_color);
      }
    }
  }

  return x;
}


uint16_t MI0283QT9::drawMLText(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, String &s, uint8_t size, uint16_t color, uint16_t bg_color)
{
  uint16_t x=x0, y=y0, wstart, wlen, llen, p;
  char c;

  fillRect(x0, y0, x1, y1, bg_color);

  llen   = (x1-x0)/(FONT_WIDTH*size); //line len in chars
  wstart = 0;
  for(p=0; (p < s.length()) && (y < y1); p++) 
  {
    c = (char)s[p];
    if(c == '\n') //new line
    {
      x  = x0;
      y += (FONT_HEIGHT*size)+1;
      continue;
    }
    else if(c == '\r') //skip
    {
      continue;
    }

    if(c == ' ') //start of a new word
    {
      wstart = p;
      if(x == x0) //do nothing
      {
        continue;
      }
    }

    if(c)
    {
      if((x+(FONT_WIDTH*size)) > x1) //new line
      {
        if(c == ' ') //do not start with space
        {
          x  = x0;
          y += (FONT_HEIGHT*size)+1;
        }
        else
        {
          wlen = (p-wstart);
          if(wlen > llen) //word too long
          {
            x  = x0;
            y += (FONT_HEIGHT*size)+1;
            if(y < y1)
            {
              x = drawChar(x, y, c, size, color, bg_color);
            }
          }
          else
          {
            fillRect(x-(wlen*FONT_WIDTH*size), y, x1, (y+(FONT_HEIGHT*size)), bg_color); //clear word
            x  = x0;
            y += (FONT_HEIGHT*size)+1;
            p  = wstart;
          }
        }
      }
      else
      {
        x = drawChar(x, y, c, size, color, bg_color);
      }
    }
  }

  return x;
}


uint16_t MI0283QT9::drawMLTextPGM(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color)
{
  uint16_t x=x0, y=y0, wlen, llen;
  char c;
  PGM_P wstart;

  fillRect(x0, y0, x1, y1, bg_color);

  llen   = (x1-x0)/(FONT_WIDTH*size); //line len in chars
  wstart = s;
  c = pgm_read_byte(s++);
  while((c != 0) && (y < y1))
  {
    if(c == '\n') //new line
    {
      x  = x0;
      y += (FONT_HEIGHT*size)+1;
      c = pgm_read_byte(s++);
      continue;
    }
    else if(c == '\r') //skip
    {
      c = pgm_read_byte(s++);
      continue;
    }

    if(c == ' ') //start of a new word
    {
      wstart = s;
      if(x == x0) //do nothing
      {
        c = pgm_read_byte(s++);
        continue;
      }
    }

    if(c)
    {
      if((x+(FONT_WIDTH*size)) > x1) //new line
      {
        if(c == ' ') //do not start with space
        {
          x  = x0;
          y += (FONT_HEIGHT*size)+1;
        }
        else
        {
          wlen = (s-wstart);
          if(wlen > llen) //word too long
          {
            x  = x0;
            y += (FONT_HEIGHT*size)+1;
            if(y < y1)
            {
              x = drawChar(x, y, c, size, color, bg_color);
            }
          }
          else
          {
            fillRect(x-(wlen*FONT_WIDTH*size), y, x1, (y+(FONT_HEIGHT*size)), bg_color); //clear word
            x  = x0;
            y += (FONT_HEIGHT*size)+1;
            s = wstart;
          }
        }
      }
      else
      {
        x = drawChar(x, y, c, size, color, bg_color);
      }
    }
    c = pgm_read_byte(s++);
  }

  return x;
}


uint16_t MI0283QT9::drawInteger(uint16_t x, uint16_t y, char val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char tmp[16+1];

  itoa((int)val, tmp, base);

  return drawText(x, y, tmp, size, color, bg_color);
}


uint16_t MI0283QT9::drawInteger(uint16_t x, uint16_t y, unsigned char val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char tmp[16+1];

  itoa((int)val, tmp, base);

  return drawText(x, y, tmp, size, color, bg_color);
}


uint16_t MI0283QT9::drawInteger(uint16_t x, uint16_t y, int val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char tmp[16+1];

  itoa(val, tmp, base);

  return drawText(x, y, tmp, size, color, bg_color);
}


uint16_t MI0283QT9::drawInteger(uint16_t x, uint16_t y, long val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color)
{
  char tmp[32+1];

  ltoa(val, tmp, base);

  return drawText(x, y, tmp, size, color, bg_color);
}


void MI0283QT9::printOptions(uint8_t size, uint16_t color, uint16_t bg_color)
{
  p_size = size;
  p_fg   = color;
  p_bg   = bg_color;

  return;
}


void MI0283QT9::printClear(void)
{
  clear(p_bg);

  p_x = PRINT_STARTX;
  p_y = PRINT_STARTY;

  return;
}


void MI0283QT9::printXY(uint16_t x, uint16_t y)
{
  p_x = x;
  p_y = y;

  return;
}


uint16_t MI0283QT9::printGetX(void)
{
  return p_x;
}


uint16_t MI0283QT9::printGetY(void)
{
  return p_y;
}


void MI0283QT9::printPGM(PGM_P s)
{
  uint16_t x=p_x, y=p_y, x_last;
  char c;
  
  c = pgm_read_byte(s++);
  while(c != 0)
  {
    if(c == '\n') //new line
    {
      x  = PRINT_STARTX;
      y += (FONT_HEIGHT*p_size)+1;
      if(y >= lcd_height)
      {
        y = PRINT_STARTY;
      }
    }
    else if(c == '\r') //skip
    {
      //do nothing
    }
    else
    {
      x_last = x;
      x = drawChar(x, y, c, p_size, p_fg, p_bg);
      if(x > lcd_width)
      {
        fillRect(x_last, y, lcd_width-1, y+(FONT_HEIGHT*p_size), p_bg);
        x  = PRINT_STARTX;
        y += (FONT_HEIGHT*p_size)+1;
        if(y >= lcd_height)
        {
          y = PRINT_STARTY;
        }
        x = drawChar(x, y, c, p_size, p_fg, p_bg);
      }
    }
    c = pgm_read_byte(s++);
  }

  p_x = x;
  p_y = y;

  return;
}


size_t MI0283QT9::write(uint8_t c)
{
  uint16_t x=p_x, y=p_y;

  if(c == '\n')
  {
    x  = PRINT_STARTX;
    y += (FONT_HEIGHT*p_size)+1;
    if(y >= lcd_height)
    {
      y = PRINT_STARTY;
    }
  }
  else if (c == '\r') //skip
  {
    //do nothing
  }
  else
  {
    x = drawChar(x, y, c, p_size, p_fg, p_bg);
    if(x > lcd_width)
    {
      fillRect(p_x, y, lcd_width-1, y+(FONT_HEIGHT*p_size), p_bg);
      x  = PRINT_STARTX;
      y += (FONT_HEIGHT*p_size)+1;
      if(y >= lcd_height)
      {
        y = PRINT_STARTY;
      }
      x = drawChar(x, y, c, p_size, p_fg, p_bg);
    }
  }

  p_x = x;
  p_y = y;

  return 1;
}


size_t MI0283QT9::write(const char *s)
{
  size_t len=0;

  while(*s)
  {
    write((uint8_t)*s++);
    len++;
  }

  return len;
}


size_t MI0283QT9::write(const uint8_t *s, size_t size)
{
  size_t len=0;

  while(size != 0)
  {
    write((uint8_t)*s++);
    size--;
    len++;
  }

  return len;
}


//-------------------- Private --------------------


void MI0283QT9::reset(void)
{
  //SPI speed-down
#if !defined(SOFTWARE_SPI)
  uint8_t spcr, spsr;
  spcr = SPCR;
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //enable SPI, Master, clk=Fcpu/16
  spsr = SPSR;
  SPSR = (1<<SPI2X); //clk*2 -> clk=Fcpu/8
#endif

  //reset
  CS_DISABLE();
  RST_ENABLE();
  delay_10ms(5);
  RST_DISABLE();
  delay_10ms(12);

  //lcd_wrcmd(LCD_CMD_RESET);
  //delay_10ms(12);
  wr_cmd(LCD_CMD_DISPLAY_OFF);
  delay_10ms(2);

  //send init commands
  wr_cmd(LCD_CMD_POWER_CTRLB);
  wr_data(0x00);
  wr_data(0x83); //83 81 AA
  wr_data(0x30);

  wr_cmd(LCD_CMD_POWERON_SEQ_CTRL);
  wr_data(0x64); //64 67
  wr_data(0x03);
  wr_data(0x12);
  wr_data(0x81);

  wr_cmd(LCD_CMD_DRV_TIMING_CTRLA);
  wr_data(0x85);
  wr_data(0x01);
  wr_data(0x79); //79 78

  wr_cmd(LCD_CMD_POWER_CTRLA);
  wr_data(0x39);
  wr_data(0X2C);
  wr_data(0x00);
  wr_data(0x34);
  wr_data(0x02);

  wr_cmd(LCD_CMD_PUMP_RATIO_CTRL);
  wr_data(0x20);

  wr_cmd(LCD_CMD_DRV_TIMING_CTRLB);
  wr_data(0x00);
  wr_data(0x00);

  wr_cmd(LCD_CMD_POWER_CTRL1);
  wr_data(0x26); //26 25
  
  wr_cmd(LCD_CMD_POWER_CTRL2);
  wr_data(0x11);

  wr_cmd(LCD_CMD_VCOM_CTRL1);
  wr_data(0x35);
  wr_data(0x3E);

  wr_cmd(LCD_CMD_VCOM_CTRL2);
  wr_data(0xBE); //BE 94

  wr_cmd(LCD_CMD_FRAME_CTRL);
  wr_data(0x00);
  wr_data(0x1B); //1B 70

  //gamma control
  /*wr_cmd(LCD_CMD_ENABLE_3G);
  wr_data(0x08); //08 00
  wr_cmd(LCD_CMD_GAMMA);
  wr_data(0x01); //G2.2
  wr_cmd(LCD_CMD_POS_GAMMA);
  uint8_t pgama[15] = {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0x87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00};
  //uint8_t pgama[15] = {0x0F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0x87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00};
  for(uint8_t i=0; i<15; i++)
  {
    wr_data(pgama[i]);
  }
  wr_cmd(LCD_CMD_NEG_GAMMA);
  uint8_t ngama[15] = {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F};
  //uint8_t ngama[15] = {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x0F};
  for(uint8_t i=0; i<15; i++)
  {
    wr_data(ngama[i]);
  }*/

  wr_cmd(LCD_CMD_DISPLAY_CTRL);
  wr_data(0x0A);
  wr_data(0x82);
  wr_data(0x27);
  wr_data(0x00);

  wr_cmd(LCD_CMD_ENTRY_MODE);
  wr_data(0x07);

  wr_cmd(LCD_CMD_PIXEL_FORMAT);
  wr_data(0x55); //16bit

  //display options
  setOrientation(0);

  //clear display buffer
  clear(0);

  //display on / sleep out
  wr_cmd(LCD_CMD_SLEEPOUT);
  delay_10ms(12);
  wr_cmd(LCD_CMD_DISPLAY_ON);
  delay_10ms(2);

  //restore SPI settings
#if !defined(SOFTWARE_SPI)
  SPCR = spcr;
  SPSR = spsr;
#endif

  return;
}


void MI0283QT9::wr_cmd(uint8_t cmd)
{
  CS_ENABLE();

  //9th bit
  MOSI_LOW(); //cmd
  CLK_LOW();
#if defined(SOFTWARE_SPI)
  CLK_HIGH();
#else
  SPCR &= ~(1<<SPE); //disable SPI
  CLK_HIGH();
  SPCR |= (1<<SPE); //enable SPI
#endif

  wr_spi(cmd);

  CS_DISABLE();

  return;
}


void MI0283QT9::wr_data16(uint16_t data)
{
  CS_ENABLE();

  //9th bit
  MOSI_HIGH(); //data
  CLK_LOW();
#if defined(SOFTWARE_SPI)
  CLK_HIGH();
#else
  SPCR &= ~(1<<SPE); //disable SPI
  CLK_HIGH();
  SPCR |= (1<<SPE); //enable SPI
#endif

  wr_spi(data>>8);

  //9th bit
  MOSI_HIGH(); //data
  CLK_LOW();
#if defined(SOFTWARE_SPI)
  CLK_HIGH();
#else
  SPCR &= ~(1<<SPE); //disable SPI
  CLK_HIGH();
  SPCR |= (1<<SPE); //enable SPI
#endif

  wr_spi(data);

  CS_DISABLE();

  return;
}


void MI0283QT9::wr_data(uint8_t data)
{
  CS_ENABLE();

  //9th bit
  MOSI_HIGH(); //data
  CLK_LOW();
#if defined(SOFTWARE_SPI)
  CLK_HIGH();
#else
  SPCR &= ~(1<<SPE); //disable SPI
  CLK_HIGH();
  SPCR |= (1<<SPE); //enable SPI
#endif

  wr_spi(data);

  CS_DISABLE();

  return;
}


void MI0283QT9::wr_spi(uint8_t data)
{
#if defined(SOFTWARE_SPI)
  for(uint8_t mask=0x80; mask!=0; mask>>=1)
  {
    CLK_LOW();
    if(mask & data)
    {
      MOSI_HIGH();
    }
    else
    {
      MOSI_LOW();
    }
    CLK_HIGH();
  }
  CLK_LOW();

#else
	SPDR = data;
  while(!(SPSR & (1<<SPIF)));
#endif

  return;
}


void MI0283QT9::delay_10ms(uint8_t ms) //delay of 10ms * x
{
  for(; ms!=0; ms--)
  {
    _delay_ms(10);
  }

  return;
}
