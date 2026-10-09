/**
 ******************************************************************************
 * @file      fonts.h
 * @author    Alexander Alt
 * @brief     Header file for fonts.c
 *
 ******************************************************************************
 **/

#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>

typedef struct {
	const uint8_t width;
	const uint8_t height;
	const uint16_t *data;
}FontDef;

extern FontDef Font_5x7;
#endif
