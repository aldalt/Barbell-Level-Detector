/**
 ******************************************************************************
 * @file      display.h
 * @author    Alexander Alt
 * @brief     Header file for display.c
 *
 ******************************************************************************
 **/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "main.h"   // Gives access to STM32 HAL peripheral types
#include "fonts.h"  // Access to custom FontDef structure definitions
#include <stdint.h> // Standard integer types (uint8_t, uint32_t)
#include <string.h>
#include <stdio.h>

// Display configuration constraints
#define OLED_WIDTH   128
#define OLED_HEIGHT  64

// Public API Functions (Visible to main.c)
void ScreenInit(void);
void ClearScreen(void);
void ScreenUpdate(void);
void PixelDraw(uint8_t x, uint8_t y, uint8_t OnOrOff);
void CharWrite(uint8_t x, uint8_t y, char ch, const FontDef *font, uint8_t OnOrOff);
void StringWrite(uint8_t x, uint8_t y, const char *string, const FontDef *font, uint8_t OnOrOff);

#endif
