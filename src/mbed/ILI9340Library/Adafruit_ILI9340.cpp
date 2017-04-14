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


#include "mbed.h"
#include "Adafruit_ILI9340.h"
#include "stdint.h"

#include "glcdfont.c"

#define OSET_BIT(dpin) dpin->write(1)
#define OCLEAR_BIT(dpin) dpin->write(0)

#define digitalWrite(dpin,value) ((value==0)?dpin->write(0):dpin->write(1))
#define digitalRead(dpin) (dpin->read())

// Single constructor either for software or hardware SPI
Adafruit_ILI9340::Adafruit_ILI9340(SPI *defspi):
  WIDTH(ILI9340_TFTWIDTH), HEIGHT(ILI9340_TFTHEIGHT)
{
  hwSPI = defspi;
}

void Adafruit_ILI9340::setMISO(DigitalIn *gport)
{
  misoport = gport;
}

void Adafruit_ILI9340::setMOSI(DigitalOut *gport)
{
  mosiport = gport;
}

void Adafruit_ILI9340::setCLK(DigitalOut *gport)
{
  clkport = gport;
}

void Adafruit_ILI9340::setRST(DigitalOut *gport)
{
  rstport = gport;
}

void Adafruit_ILI9340::setCS(DigitalOut *gport)
{
  csport = gport;
}

void Adafruit_ILI9340::setDC(DigitalOut *gport)
{
  dcport = gport;
}

void Adafruit_ILI9340::spiwrite(uint8_t c) {
  if (hwSPI) {
    hwSPI->write(c);
  } else {
    // Fast SPI bitbang swiped from LPD8806 library
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) {        
        OSET_BIT(mosiport);
      } else {
        
        OCLEAR_BIT(mosiport);
      }      
      OSET_BIT(clkport);      
      OCLEAR_BIT(clkport);
    }
  }
}


void Adafruit_ILI9340::writecommand(uint8_t c) {
  OCLEAR_BIT(dcport);
  if (!hwSPI)
    OCLEAR_BIT(clkport);

  OCLEAR_BIT(csport);
  spiwrite(c);
  OSET_BIT(csport);
}


void Adafruit_ILI9340::writedata(uint8_t c) {
  OSET_BIT(dcport);
  if (!hwSPI)
    OCLEAR_BIT(clkport);

  OCLEAR_BIT(csport);
  spiwrite(c);
  OSET_BIT(csport);
} 

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Adafruit_ILI9340::commandList(uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    writecommand(pgm_read_byte(addr++)); //   Read, issue command
    numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
      writedata(pgm_read_byte(addr++));  //     Read, issue argument
    }

    if(ms) {
      ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      wait_ms(ms);
    }
  }
}

static uint8_t ILI9340_init[] = {
  1,
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0X12, 0X81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  // Power control
  ILI9340_PWCTR1,   1, 0x23,
  ILI9340_PWCTR2,   1, 0x10,
  // VCM control
  ILI9340_VMCTR1,   2, 0x3E, 0x28,
  ILI9340_VMCTR2,   1, 0x86,
  // Memory access control
  ILI9340_MADCTL,   1, ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR,
  ILI9340_PIXFMT,   1, 0x55,
  ILI9340_FRMCTR1,  2, 0x00, 0x18,
  // Display function control
  ILI9340_DFUNCTR,  3, 0x08, 0x82, 0x27,
  // 3Gamma function disable
  0xF2,             1, 0x00,
  // Select gamma curve
  ILI9340_GAMMASET, 1, 0x01,
  // Set gamma
  ILI9340_GMCTRP1,  15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9340_GMCTRN1,  15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  // Exit sleep
  ILI9340_SLPOUT,   DELAY, 120,
  // Display on
  ILI9340_DISPON,   0,
};

void Adafruit_ILI9340::begin(void) {

  digitalWrite(rstport, LOW);
  if(hwSPI) {  
    hwSPI->frequency(16000000);
  } else {
    OCLEAR_BIT(clkport);
    OCLEAR_BIT(mosiport);
  }
  // toggle RST low to reset

  digitalWrite(rstport, HIGH);
  wait_ms(5);
  digitalWrite(rstport, LOW);
  wait_ms(20);
  digitalWrite(rstport, HIGH);
  wait_ms(150);

  commandList(ILI9340_init);
}


void Adafruit_ILI9340::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1,
 uint16_t y1) {

  writecommand(ILI9340_CASET); // Column addr set
  writedata(x0 >> 8);
  writedata(x0 & 0xFF);     // XSTART 
  writedata(x1 >> 8);
  writedata(x1 & 0xFF);     // XEND

  writecommand(ILI9340_PASET); // Row addr set
  writedata(y0>>8);
  writedata(y0);     // YSTART
  writedata(y1>>8);
  writedata(y1);     // YEND

  writecommand(ILI9340_RAMWR); // write to RAM
}


void Adafruit_ILI9340::pushColor(uint16_t color) {
  
  OSET_BIT(dcport);
  
  OCLEAR_BIT(csport);

  spiwrite(color >> 8);
  spiwrite(color);

  OSET_BIT(csport);
  
}

void Adafruit_ILI9340::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x,y,x+1,y+1);

  OSET_BIT(dcport);
  OCLEAR_BIT(csport);

  spiwrite(color >> 8);
  spiwrite(color);

  OSET_BIT(csport);
  
}


void Adafruit_ILI9340::drawFastVLine(int16_t x, int16_t y, int16_t h,
 uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;

  if((y+h-1) >= _height) 
    h = _height-y;

  setAddrWindow(x, y, x, y+h-1);

  uint8_t hi = color >> 8, lo = color;

  OSET_BIT(dcport);
  OCLEAR_BIT(csport);

  while (h--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  OSET_BIT(csport);
  
}


void Adafruit_ILI9340::drawFastHLine(int16_t x, int16_t y, int16_t w,
  uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  uint8_t hi = color >> 8, lo = color;
  OSET_BIT(dcport);
  OCLEAR_BIT(csport);

  while (w--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  OSET_BIT(csport);
}

void Adafruit_ILI9340::fillScreen(uint16_t color) {
  fillRect(0, 0,  _width, _height, color);
}

// fill a rectangle
void Adafruit_ILI9340::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
  uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w-1, y+h-1);

  uint8_t hi = color >> 8, lo = color;

  OSET_BIT(dcport);
  //digitalWrite(_dc, HIGH);
  OCLEAR_BIT(csport);
  //digitalWrite(_cs, LOW);

  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      spiwrite(hi);
      spiwrite(lo);
    }
  }
  //digitalWrite(_cs, HIGH);
  OSET_BIT(csport);
}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Adafruit_ILI9340::Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void Adafruit_ILI9340::setRotation(uint8_t m) {

  writecommand(ILI9340_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     writedata(ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTWIDTH;
     _height = ILI9340_TFTHEIGHT;
     break;
   case 1:
     writedata(ILI9340_MADCTL_MV | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTHEIGHT;
     _height = ILI9340_TFTWIDTH;
     break;
  case 2:
    writedata(ILI9340_MADCTL_MY | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTWIDTH;
     _height = ILI9340_TFTHEIGHT;
    break;
   case 3:
     writedata(ILI9340_MADCTL_MV | ILI9340_MADCTL_MY | ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
     _width  = ILI9340_TFTHEIGHT;
     _height = ILI9340_TFTWIDTH;
     break;
  }
}

void Adafruit_ILI9340::invertDisplay(char i) {
  writecommand(i ? ILI9340_INVON : ILI9340_INVOFF);
}

// Adafruit_ILI9340 methods:

// Only supports horizontal/vertical lines
void Adafruit_ILI9340::drawLine(int16_t x0, int16_t y0,
                            int16_t x1, int16_t y1,
                            uint16_t color) {
  if (x0 == x1) {
    drawFastVLine(x0, y0, y1 - y0 + 1, color);
  } else if (y0 == y1) {
    drawFastHLine(x0, y0, x1 - x0 + 1, color);
  }
}

void Adafruit_ILI9340::write(uint8_t c) {
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_x > (_width - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
  }
}

// Draw a character
void Adafruit_ILI9340::drawChar(int16_t x, int16_t y, unsigned char c,
                uint16_t color, uint16_t bg, uint8_t size) {

  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;

  if (c > 0x7f)
    c = ' ';

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5)
      line = 0x0;
    else
      line = pgm_read_byte(font+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        }
      } else if (bg != color) {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else {  // big size
          fillRect(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void Adafruit_ILI9340::setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}

void Adafruit_ILI9340::setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}

void Adafruit_ILI9340::setTextColor(uint16_t c) {
  // For 'transparent' background, we'll set the bg 
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

void Adafruit_ILI9340::setTextColor(uint16_t c, uint16_t b) {
  textcolor   = c;
  textbgcolor = b;
}

void Adafruit_ILI9340::setTextWrap(char w) {
  wrap = w;
}

// Draw a rectangle
void Adafruit_ILI9340::drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y+h-1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x+w-1, y, h, color);
}
