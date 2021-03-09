/*
 * Pixels. Graphics library for TFT displays.
 *
 * Copyright (C) 2012-2013  Igor Repinetski
 *
 * The code is written in C/C++ for Arduino and can be easily ported to any microcontroller by rewritting the low level pin access functions.
 *
 * Text output methods of the library rely on Pixelmeister's font data format. See: http://pd4ml.com/pixelmeister
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 *
 * This library includes some code portions and algoritmic ideas derived from works of
 * - Andreas Schiffler -- aschiffler at ferzkopp dot net (SDL_gfx Project)
 * - K. Townsend http://microBuilder.eu (lpc1343codebase Project)
 */

/*
 * Pixels port to ILI9341 controller, SPI mode (ElecFreaks TFT2.2SP Shield)
 * SPI is in bit banging mode, as the shield does not connect the hardware SPI (SCL=13, SDA=11)
 * to the display controller
 */

#include "Pixels.h"

#ifndef PIXELS_ILI9341_H
#define PIXELS_ILI9341_H
#define PIXELS_MAIN

//ST7789v COMMANDS
#define ST7789v_NOP     0x00
#define ST7789v_SWRESET 0x01 //Software reset
#define ST7789v_RDDID   0x04 //Read display ID
#define ST7789v_RDDST   0x09 //Read display status

#define ST7789v_SLPIN   0x10 //Sleep in
#define ST7789v_SLPOUT  0x11 //Sleep out
#define ST7789v_PTLON   0x12 //Partial mode on
#define ST7789v_NORON   0x13 //Partial off (Normal)

#define ST7789v_INVOFF  0x20 //Display inversion off
#define ST7789v_INVON   0x21 //Display inversion on
#define ST7789v_DISPOFF 0x28 //Display off
#define ST7789v_DISPON  0x29 //Display on
#define ST7789v_CASET   0x2A //Column address set
#define ST7789v_RASET   0x2B //Row address set
#define ST7789v_RAMWR   0x2C //Memory write
#define ST7789v_RAMRD   0x2E //Memory read

#define ST7789v_PTLAR   0x30 //Partial sart/end address set
#define ST7789v_COLMOD  0x3A //Interface pixel format
#define ST7789v_MADCTL  0x36 //Memory data access control

#define ST7789v_FRMCTR1 0xB1 //RGB Control
#define ST7789v_FRMCTR2 0xB2 //Porch control
#define ST7789v_FRMCTR3 0xB3 //Frame Rate Control 1

#define ST7789v_GCTRL	  0xB7 //Gate Control
#define ST7789v_VCOMS   0xBB //VCOMS Setting
#define ST7789v_PWCTR1  0xC0 //LCM Control
#define ST7789v_PWCTR2  0xC1 //ID Setting
#define ST7789v_PWCTR3  0xC2 //VDV and VRH Command Enable
#define ST7789v_PWCTR4  0xC3 //VRH Set
#define ST7789v_PWCTR5  0xC4 //VDV Setting
#define ST7789v_VMCTR1  0xC5 //VCOMS Offset Set
#define ST7789v_FRCTRL2 0xC6 //Frame Rate Control in Normal Mode

#define ST7789v_RDID1   0xDA //Read ID1
#define ST7789v_RDID2   0xDB //Read ID2
#define ST7789v_RDID3   0xDC //Read ID3

#define ST7789v_PWCTR6  0xFC //NVM Setting

#define ST7789v_GMCTRP1 0xE0 //Positive Voltage Gamma Control
#define ST7789v_GMCTRN1 0xE1 //Negative Voltage Gamma Control
#define ST7789v_WRDISBV 0x51 //Write Display Brightness
#define ST7789v_WRCTRLD 0x53 //Write CTRL Display


#if defined(PIXELS_ANTIALIASING_H)
#define PixelsBase PixelsAntialiased
#endif

class Pixels : public PixelsBase
#if defined(PIXELS_SPISW_H)
                                    , public SPIsw
#elif defined(PIXELS_SPIHW_H)
                                    , public SPIhw
#elif defined(PIXELS_PPI8_H)
                                    , public PPI8
#elif defined(PIXELS_PPI16_H)
                                    , public PPI16
#endif
{
protected:
    virtual void deviceWriteData(uint8_t high, uint8_t low) {
        writeData(high, low);
    }

    virtual void setRegion(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    virtual void quickFill(int b, int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    virtual void setFillDirection(uint8_t direction);

    virtual void scrollCmd();

public:
    Pixels() : PixelsBase(240, 320) { // ElecFreaks TFT2.2SP shield as default
        scrollSupported = true;
    }

    Pixels(uint16_t width, uint16_t height) : PixelsBase( width, height) {
        scrollSupported = true;
    }

    virtual void init();
		
		void displayOn()
		{
			writeCmd(0x29);    //Display on
			LL_GPIO_SetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin);
		}
		
		void displayOff()
		{
			writeCmd(0x28);    //Display off
			LL_GPIO_ResetOutputPin(LCD_BL_GPIO_Port, LCD_BL_Pin);
		}
};

#if defined(PIXELS_ANTIALIASING_H)
#undef PixelsBase
#endif



void Pixels::init() {


  LCD_CS_SET;
    LCD_RST_SET;
	  delay_1ms(1);
    LCD_RST_CLR;
    delay_1ms(50);
    LCD_RST_SET;
    delay_1ms(120);
	LCD_CS_CLR;
	
	writeCmd(ST7789v_SLPOUT);
  //delay_1ms(10);//500	
	//writeCmd(ST7789v_MADCTL);
	//writeData(0x00);
	writeCmd(ST7789v_COLMOD);
	writeData(0x05);
	
		//--------------------------------ST7789V Frame rate setting----------------------------------//
	writeCmd(ST7789v_FRMCTR2);
	writeData(0x0C);
	writeData(0x0C);
	writeData(0x00);
	writeData(0x33);
	writeData(0x33);
	//LCD_CS_SET;
	
	//---------------------------------ST7789V Power setting--------------------------------------//
	writeCmd(ST7789v_GCTRL);
	writeData(0x35);
	writeCmd(ST7789v_VCOMS);
	writeData(0x28);
	writeCmd(ST7789v_PWCTR1);
	writeData(0x2C);
	writeCmd(ST7789v_PWCTR3);
	writeData(0x01);
	writeCmd(ST7789v_PWCTR4);
	writeData(0x0B);
	writeCmd(ST7789v_PWCTR5);
	writeData(0x20);
	writeCmd(ST7789v_FRCTRL2);
	writeData(0x0f);
	writeCmd(ST7789v_INVOFF); //Inversion OFF
	writeCmd(0xd0);
	writeData(0xa4);
	writeData(0xa1);
	
	//------------------------------display and color format setting--------------------------------//
	//writeCmd(ST7789v_MADCTL);
	//writeData(0x20, 1);
	//writeData(0x80, 1);

	/*
	writeCmd(ST7789v_CASET);
	writeData(0x00);
	writeData(0x00);
	writeData(0x00);
	writeData(0xF0); //240
	
	writeCmd(ST7789v_RASET);
	writeData(0x00);
	writeData(0x00);
	writeData(0x01);
	writeData(0x40); //320
		*/
	//--------------------------------ST7789V gamma setting---------------------------------------//
	writeCmd(ST7789v_GMCTRP1);
	writeData(0xd0);
	writeData(0x01);
	writeData(0x08);
	writeData(0x0f);
	writeData(0x11);
	writeData(0x2a);
	writeData(0x36);
	writeData(0x55);
	writeData(0x44);
	writeData(0x3a);
	writeData(0x0b);
	writeData(0x06);	
	writeData(0x11);
	writeData(0x20);
		
	writeCmd(ST7789v_GMCTRN1);
	writeData(0xd0);
	writeData(0x02);
	writeData(0x07);
	writeData(0x0a);
	writeData(0x0b);
	writeData(0x18);
	writeData(0x34);
	writeData(0x43);
	writeData(0x4a);
	writeData(0x2b);
	writeData(0x1b);
	writeData(0x1c);	
	writeData(0x22);
	writeData(0x1f);
	///*
	writeCmd(ST7789v_NORON);
	//delay_1ms(10);
	
//	ST7789v_Clear(ST7789v_BLACK);
	//writeCmd(ST7789v_DISPON);
	//delay_1ms(100);
	

	

	
	//
	writeCmd(ST7789v_DISPON);
	
	//delay_1ms(1);
	LCD_CS_SET;
}

void Pixels::scrollCmd() {
    chipSelect();
    writeCmd(0x37);
    writeData(highByte(currentScroll));
    writeData(lowByte(currentScroll));
    chipDeselect();
}

void Pixels::setFillDirection(uint8_t direction) {
    fillDirection = direction;
}

void Pixels::quickFill (int color, int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    
		LCD_CS_CLR;

    setRegion(x1, y1, x2, y2);

    int32_t counter = (int32_t)(x2 - x1 + 1) * (y2 - y1 + 1);
		
    //registerSelect();

    uint8_t lo = lowByte(color);
    uint8_t hi = highByte(color);

    for (int16_t i = 0; i < counter / 20; i++) {
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);
				writeData(hi, lo);

    }
    for (int32_t i = 0; i < counter % 20; i++) {
      writeData(hi, lo);  
    }

    LCD_CS_SET;
}

void Pixels::setRegion(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    if ( orientation != PORTRAIT ) {
        int16_t buf;
        switch( orientation ) {
        case LANDSCAPE:
            buf = x1;
            x1 = deviceWidth - y1 - 1;
            y1 = buf;
            buf = x2;
            x2 = deviceWidth - y2 - 1;
            y2 = buf;
            break;
        case PORTRAIT_FLIP:
            y1 = deviceHeight - y1 - 1;
            y2 = deviceHeight - y2 - 1;
            x1 = deviceWidth - x1 - 1;
            x2 = deviceWidth - x2 - 1;
            break;
        case LANDSCAPE_FLIP:
            buf = y1;
            y1 = deviceHeight - x1 - 1;
            x1 = buf;
            buf = y2;
            y2 = deviceHeight - x2 - 1;
            x2 = buf;
            break;
        }

        if (x2 < x1) {
            swap(x1, x2);
        }
        if (y2 < y1) {
            swap(y1, y2);
        }
    }

    writeCmd(0x2a);
    writeData(x1>>8);
    writeData(x1);
    writeData(x2>>8);
    writeData(x2);
    writeCmd(0x2b);
    writeData(y1>>8);
    writeData(y1);
    writeData(y2>>8);
    writeData(y2);
    writeCmd(0x2c);
}
#endif
