/**
 ******************************************************************************
 * @file      display.c
 * @author    Alexander Alt
 * @brief     Contains functions necessary for initializing and updating the
 *            OLED display, as well as displaying text on the screen.
 *
 ******************************************************************************
 **/

#include "display.h"
#include "stm32f4xx_hal.h"
#include <string.h>

// Handle reference declared in main.c
extern I2C_HandleTypeDef hi2c1;

// Standard SSD1306 I2C 7-bit Address and control byte prefixes
#define SSD1306_I2C_ADDR        (0x3D << 1)
#define SSD1306_CONTROL_COMMAND 0x00
#define SSD1306_CONTROL_DATA    0x40

// Allocate RAM Frame buffer inside internal SRAM
uint8_t ScreenBuffer[OLED_WIDTH * (OLED_HEIGHT / 8)];

static void Display_WriteCommand(uint8_t command) {
    // Send 1 byte of command data over I2C to the 0x00 control register
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, SSD1306_CONTROL_COMMAND, I2C_MEMADD_SIZE_8BIT, &command, 1, 10);

    // Gives the display controller hardware time to digest the byte
    for(volatile uint32_t i = 0; i < 200; i++);
}

void ClearScreen(void) {
    memset(ScreenBuffer, 0, sizeof(ScreenBuffer));
}

void PixelDraw(uint8_t x, uint8_t y, uint8_t OnOrOff) {
    // Strict safety boundary shield: Drop transmission if coordinates fall outside RAM grid parameters
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;

    // Calculate array byte position mapping index
    uint32_t bufferIndex = x + (y / 8) * OLED_WIDTH;

    // Safety lock against array bounds corruption
    if (bufferIndex >= (OLED_WIDTH * (OLED_HEIGHT / 8))) return;

    if (OnOrOff) ScreenBuffer[bufferIndex] |= (1 << (y % 8));
    else ScreenBuffer[bufferIndex] &= ~(1 << (y % 8));
}

void CharWrite(uint8_t x, uint8_t y, char ch, const FontDef *font, uint8_t OnOrOff) {
    // 1. Structural boundary check: Prevent null pointer crashes
    if (font == NULL || font->data == NULL) return;

    // 2. Map standard ASCII spectrum printable ranges safely
    if (ch < 32 || ch > 126) ch = ' ';

    // 3. Calculate dynamic offset index based on incoming structure properties
    uint32_t bytesPerChar = font->width; // For 5x7 fonts, each character is exactly 5 slices
    uint32_t charIndex = (ch - 32) * bytesPerChar;

    // 4. Standard Fonts loop data horizontally (Columns first, then rows)
    for (uint8_t col = 0; col < font->width; col++) {
        // FIXED: Changed to uint16_t to match the structural properties declared in fonts.h
        uint16_t columnDataByte = font->data[charIndex + col];

        for (uint8_t row = 0; row < font->height; row++) {
            // Read the bits starting from the least significant bit (bottom to top)
            if ((columnDataByte >> row) & 0x01) PixelDraw(x + col, y + row, OnOrOff);
            else PixelDraw(x + col, y + row, !OnOrOff);
        }
    }
}

void StringWrite(uint8_t x, uint8_t y, const char *string, const FontDef *font, uint8_t OnOrOff) {
    if (string == NULL) return;

    while (*string != '\0') {
        if (x + font->width > OLED_WIDTH) {
            x = 0;
            y += font->height;
        }

        if (y + font->height > OLED_HEIGHT) break;

        CharWrite(x, y, *string, font, OnOrOff);

        x += font->width;
        string++;
    }
}

void ScreenUpdate(void) {
    Display_WriteCommand(0x21); // Column Address bounds command
    Display_WriteCommand(0);
    Display_WriteCommand(127);

    Display_WriteCommand(0x22); // Page Address bounds command
    Display_WriteCommand(0);
    Display_WriteCommand(7);

    // Safety size evaluation to guarantee no buffer overflows escape across the HAL bus
    uint32_t bufferSize = OLED_WIDTH * (OLED_HEIGHT / 8); // Equal to exactly 1024 bytes

    // Send the entire RAM buffer to the 0x40 Data register prefix over I2C
    HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDR, SSD1306_CONTROL_DATA, I2C_MEMADD_SIZE_8BIT, ScreenBuffer, bufferSize, 100);
}

void ScreenInit(void) {
    HAL_Delay(100);

    // 2. Verified initialization stream sequence for standard Adafruit SSD1306 panels
    Display_WriteCommand(0xAE); // Turn off display screen panel while configuring
    Display_WriteCommand(0xD5); // Set clock divide ratio / oscillator frequency oscillations
    Display_WriteCommand(0x80); // Suggested default ratio
    Display_WriteCommand(0xA8); // Set multiplex ratio configuration
    Display_WriteCommand(0x3F); // 64 lines high tracking height layout map
    Display_WriteCommand(0xD3); // Set display offset layout shift adjustments
    Display_WriteCommand(0x00); // No offset bounds mapping
    Display_WriteCommand(0x40); // Set startup line mapping register array base

    // CHARGE PUMP INITIALIZATION (CRITICAL PAIR)
    Display_WriteCommand(0x8D); // Turn on internal voltage charge pump converter setup
    Display_WriteCommand(0x14); // Enable internal charge pump (0x10 is off!)

    Display_WriteCommand(0x20); // Set memory addressing configuration parameters
    Display_WriteCommand(0x00); // Horizontal mode mapping strategy allocation

    // FIXED: PANEL MATCHING PARAMETERS (Updated tracking parameters to fit Adafruit panel architecture)
    Display_WriteCommand(0xA1); // Segment remap inversion setting layout match
    Display_WriteCommand(0xC8); // COM output scan orientation tracking layout

    Display_WriteCommand(0xDA); // COM pin hardware pin setup layout configuration
    Display_WriteCommand(0x12); // Alternative COM pin map structure layout

    // BRIGHTNESS SELECTION REGISTER
    Display_WriteCommand(0x81); // Contrast control entry setup
    Display_WriteCommand(0xFF); // Push panel current to maximum scale limits

    Display_WriteCommand(0xD9); // Set discharge precharge duration layout configuration
    Display_WriteCommand(0xF1); // Pre-charge phase operational intervals setup
    Display_WriteCommand(0xDB); // Set VCOMH deselect level regulator parameters
    Display_WriteCommand(0x40); // Max regulator out bounds matching

    Display_WriteCommand(0xA4); // Render display data RAM stream active output match
    Display_WriteCommand(0xA6); // Normal display pixel visibility mode mapping logic (0xA7 is inverse)
    Display_WriteCommand(0xAF); // DISPLAY OPERATIONAL POWER ON!

    ClearScreen();        // Wipe RAM data buffers completely clean
    ScreenUpdate();       // Flush clear baseline configuration to panels
}
