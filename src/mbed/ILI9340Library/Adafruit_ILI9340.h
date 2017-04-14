/***************************************************
  This is an Arduino Library for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

// Modified for mbed
// by Georgios Moralis 

#ifndef _ADAFRUIT_ILI9340H_
#define _ADAFRUIT_ILI9340H_

#include "mbed.h"

//#include "core_cm3.h"
#include "stdint.h"

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))

#define HIGH	0x01
#define LOW		0x00

#define ILI9340_TFTWIDTH  240
#define ILI9340_TFTHEIGHT 320

#define ILI9340_NOP     0x00
#define ILI9340_SWRESET 0x01
#define ILI9340_RDDID   0x04
#define ILI9340_RDDST   0x09

#define ILI9340_SLPIN   0x10
#define ILI9340_SLPOUT  0x11
#define ILI9340_PTLON   0x12
#define ILI9340_NORON   0x13

#define ILI9340_RDMODE  0x0A
#define ILI9340_RDMADCTL  0x0B
#define ILI9340_RDPIXFMT  0x0C
#define ILI9340_RDIMGFMT  0x0A
#define ILI9340_RDSELFDIAG  0x0F

#define ILI9340_INVOFF  0x20
#define ILI9340_INVON   0x21
#define ILI9340_GAMMASET 0x26
#define ILI9340_DISPOFF 0x28
#define ILI9340_DISPON  0x29

#define ILI9340_CASET   0x2A
#define ILI9340_PASET   0x2B
#define ILI9340_RAMWR   0x2C
#define ILI9340_RAMRD   0x2E

#define ILI9340_PTLAR   0x30
#define ILI9340_MADCTL  0x36


#define ILI9340_MADCTL_MY  0x80
#define ILI9340_MADCTL_MX  0x40
#define ILI9340_MADCTL_MV  0x20
#define ILI9340_MADCTL_ML  0x10
#define ILI9340_MADCTL_RGB 0x00
#define ILI9340_MADCTL_BGR 0x08
#define ILI9340_MADCTL_MH  0x04

#define ILI9340_PIXFMT  0x3A

#define ILI9340_FRMCTR1 0xB1
#define ILI9340_FRMCTR2 0xB2
#define ILI9340_FRMCTR3 0xB3
#define ILI9340_INVCTR  0xB4
#define ILI9340_DFUNCTR 0xB6

#define ILI9340_PWCTR1  0xC0
#define ILI9340_PWCTR2  0xC1
#define ILI9340_PWCTR3  0xC2
#define ILI9340_PWCTR4  0xC3
#define ILI9340_PWCTR5  0xC4
#define ILI9340_VMCTR1  0xC5
#define ILI9340_VMCTR2  0xC7

#define ILI9340_RDID1   0xDA
#define ILI9340_RDID2   0xDB
#define ILI9340_RDID3   0xDC
#define ILI9340_RDID4   0xDD

#define ILI9340_GMCTRP1 0xE0
#define ILI9340_GMCTRN1 0xE1
/*
#define ILI9340_PWCTR6  0xFC

*/

// Color definitions
#define	ILI9340_BLACK   0x0000
#define	ILI9340_BLUE    0x001F
#define	ILI9340_RED     0xF800
#define	ILI9340_GREEN   0x07E0
#define ILI9340_CYAN    0x07FF
#define ILI9340_MAGENTA 0xF81F
#define ILI9340_YELLOW  0xFFE0
#define ILI9340_WHITE   0xFFFF


class Adafruit_ILI9340
{

public:

    Adafruit_ILI9340(SPI *defspi);

    void
        begin(),
        drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size),
        drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
        drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
        drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),
        drawPixel(int16_t x, int16_t y, uint16_t color),
        drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
        fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
        fillScreen(uint16_t color),
        invertDisplay(char i),
        pushColor(uint16_t color),
        setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1),
        setCursor(int16_t x, int16_t y),
        setRotation(uint8_t r),
        setTextColor(uint16_t c),
        setTextColor(uint16_t c, uint16_t bg),
        setTextSize(uint8_t s),
        setTextWrap(char w),
        write(uint8_t);

    void setMISO(DigitalIn *gport);
    void setMOSI(DigitalOut *gport);
    void setCLK(DigitalOut *gport);
    void setRST(DigitalOut *gport);
    void setCS(DigitalOut *gport);
    void setDC(DigitalOut *gport);

    uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

    /* These are not for current use, 8-bit protocol only! */
    uint8_t  readdata(void),
             readcommand8(uint8_t);
    /*
    uint16_t readcommand16(uint8_t);
    uint32_t readcommand32(uint8_t);
    void     dummyclock(void);
    */

    void	spiwrite(uint8_t),
            writecommand(uint8_t c),
            writedata(uint8_t d),
            commandList(uint8_t *addr);
    uint8_t  spiread(void);

protected:
    // This is the 'raw' display w/h - never changes
    const int16_t WIDTH, HEIGHT;
    // Display w/h as modified by current rotation
    int16_t _width, _height,
            cursor_x, cursor_y;
    uint16_t textcolor, textbgcolor;
    uint8_t textsize, rotation;
    // If set, 'wrap' text at right edge of display
    char wrap;

private:
    uint8_t  tabcolor;

    SPI	*hwSPI;
    DigitalOut *mosiport, *clkport, *dcport, *rstport, *csport;
    DigitalIn *misoport;
};

#endif

