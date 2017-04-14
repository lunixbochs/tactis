#include <stdio.h>
#include <string.h>

#include "mbed.h"
#include "ILI9340Library/Adafruit_ILI9340.h"

extern "C" {
#include "tactis/cpu.h"
}

DigitalOut myled(dp28);

// LCD setup
DigitalOut cs_pin(dp10);
DigitalOut rst_pin(dp11);
DigitalOut dc_pin(dp9);

SPI spi1(dp2, dp1, dp6);
Adafruit_ILI9340 tft(&spi1);

Timer t;

void write(const char *string) {
    while (*string) tft.write(*string++);
}

void writeLine(int x, int _y, const char *string) {
    static int y = 0;
    if (!y) y = _y;
    tft.setCursor(x, y);
    write(string);
    y += 13;
}

void writeCode(int x, int y) {
    tft.setCursor(x, y);
    writeLine(x, y, "MOV 0, ACC");
    writeLine(x, y, "MOV LEFT, DOWN");
    writeLine(x, y, "JRO RIGHT");
    writeLine(x, y, "NOP");
    writeLine(x, y, "NOP");
    writeLine(x, y, "ADD UP");
    writeLine(x, y, "ADD UP");
    writeLine(x, y, "ADD UP");
    writeLine(x, y, "JMP 1");
    writeLine(x, y, "ADD UP");
    writeLine(x, y, "ADD UP");
    writeLine(x, y, "1: ADD UP");
    writeLine(x, y, "NOP");
    writeLine(x, y, "ADD UP");
    writeLine(x, y, "MOV ACC, DOWN");
}

void writeStatus() {
    tft.setCursor(240, 28);
    write("ACC");
    tft.setCursor(240, 41);
    write("0");

    tft.setCursor(240, 68);
    write("BAK");
    tft.setCursor(240, 81);
    write("(0)");

    tft.setCursor(240, 110);
    write("LAST");
    tft.setCursor(240, 123);
    write("N/A");

    tft.setCursor(240, 150);
    write("MODE");
    tft.setCursor(240, 163);
    write("IDLE");

    tft.setCursor(240, 190);
    write("IDLE");
    tft.setCursor(240, 203);
    write("0%");
}

void drawDoubleRect(int x, int y, int width, int height, int color) {
    tft.drawRect(x, y, width, height, color);
    tft.drawRect(x + 2, y + 2, width - 4, height - 4, color);
}

void drawDoubleLine(int x1, int y1, int x2, int y2, int color) {
    tft.drawLine(x1, y1, x2, y2, color);
    if (x1 == x2) {
        tft.drawLine(x1 + 2, y1, x2 + 2, y2, color);
    } else {
        tft.drawLine(x1, y1 + 2, x2, y2 + 2, color);
    }
}

int main() {
    tft.setRST(&rst_pin);
    tft.setCS(&cs_pin);
    tft.setDC(&dc_pin);
    tft.begin();

    tft.fillRect(0, 0, 240, 320, 0);
    tft.setRotation(3);

    tft.fillScreen(ILI9340_BLACK);
    drawDoubleRect(34, 18, 252, 202, ILI9340_WHITE);
    drawDoubleLine(220, 20, 220, 216, ILI9340_WHITE);
    for (int i = 18; i < 200; i += 40) {
      drawDoubleLine(222, i, 282, i, ILI9340_WHITE);
    }
    // writeCode(40, 23);
    writeStatus();
    
    tft.setCursor(80, 6);
    write("-999");
    
    tft.setCursor(200, 6);
    write("-999");
    
    tft.setCursor(80, 224);
    write("-999");
    
    tft.setCursor(200, 224);
    write("-999");
    
    tft.setCursor(5, 60);
    write("-999");
    
    tft.setCursor(5, 170);
    write("-999");

    tft.setCursor(290, 60);
    write("-999");
    
    tft.setCursor(290, 170);
    write("-999");

    parse_error err;
    node_t *cpu = cpu_new("MOV ACC, 1", &err, NULL, NULL);
    cpu_step(cpu);
    cpu_latch(cpu);
    cpu_free(cpu);

#if 0
    int x = 0, y = 0;

    int status = 0;
    // t.start();
    int c = 0;
    while(1) {
        if (c == '\n') {
            y++;
        } else {
            text[y][x++] = c;
        }
        /*
        if (t.read_ms() > 1000) {
            status = !status;
            myled = status;
            t.stop();
            t.reset();
            t.start();
        }
        */
    }
#endif
}
