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
 * Parallel interface 16bit layer
 */

#include "Pixels.h"
#include "main.h"

#ifdef PIXELS_MAIN
#error Pixels_PPI16.h must be included before Pixels_<CONTROLLER>.h
#endif

#ifndef PIXELS_PPI16_H
#define PIXELS_PPI16_H

//#define pulseLowWR LL_GPIO_ResetOutputPin(LCD_WR_GPIO_Port, LCD_WR_Pin);LL_GPIO_SetOutputPin(LCD_WR_GPIO_Port, LCD_WR_Pin);

class PPI16 {
private:
protected:
    void reset() 
		{
			LL_GPIO_SetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
			delay_1ms(1);
			LL_GPIO_ResetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
			delay_1ms(10);
			LL_GPIO_SetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
			delay_1ms(120);
    }

    void initInterface();

    void writeCmd(uint8_t data) 
		{
			//LL_GPIO_ResetOutputPin(LCD_RS_GPIO_Port, LCD_RS_Pin);
			LCD_RS_CLR;
			//uint16_t val = b << 1;
			
			spi_write_byte(SPI1,data);

    }

    void writeData(uint8_t data) 
		{
			//LL_GPIO_SetOutputPin(LCD_RS_GPIO_Port, LCD_RS_Pin);
			LCD_RS_SET;
     	//uint16_t val = (data << 1);
			spi_write_byte(SPI1,data);

    }

    void writeData(uint8_t hi, uint8_t lo) 
		{
			//LL_GPIO_SetOutputPin(LCD_RS_GPIO_Port, LCD_RS_Pin);
			LCD_RS_SET;
      spi_write_byte(SPI1,hi);
			spi_write_byte(SPI1,lo);

    }

    void writeDataTwice(uint8_t data) 
		{
			LL_GPIO_SetOutputPin(LCD_RS_GPIO_Port, LCD_RS_Pin);
      spi_write_byte(SPI1,highByte(data));
			spi_write_byte(SPI1,lowByte(data));
    }
		
    void writeCmdData(uint8_t cmd, uint16_t data) {
        writeCmd(cmd);
        writeData(highByte(data), lowByte(data));
    }

public:
    /**
     * Overrides SPI pins
     * @param scl
     * @param sda
     * @param cs chip select
     * @param rst reset
     * @param wr write pin
     */

    inline void setSpiPins(uint8_t scl, uint8_t sda, uint8_t cs, uint8_t rst, uint8_t wr) {
    }

    /**
     * Overrides PPI pins
     * @param cs chip select
     */
    inline void setPpiPins(uint8_t rs, uint8_t wr, uint8_t cs, uint8_t rst, uint8_t rd) {
    }

    inline void registerSelect() {
      LL_GPIO_SetOutputPin(LCD_RS_GPIO_Port, LCD_RS_Pin);  
    }
};

void PPI16::initInterface() {
/*
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_ALL);

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = LL_GPIO_PIN_ALL;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
	LL_GPIO_SetOutputPin(LCD_RD_GPIO_Port, LCD_RD_Pin);
	LL_GPIO_SetOutputPin(LCD_WR_GPIO_Port, LCD_WR_Pin);

  reset();*/
}
#endif
