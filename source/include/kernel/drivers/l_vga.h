#ifndef L_VGA_H
#define L_VGA_H

#include <stdint.h>
#include <stddef.h>

enum VGA_COLOR {
	VGA_COLOR_BLACK 	= 0x0,
	VGA_COLOR_BLUE 		= 0x1,
	VGA_COLOR_GREEN 	= 0x2,
	VGA_COLOR_CYAN 		= 0x3,
	VGA_COLOR_RED 		= 0x4,
	VGA_COLOR_MAGENTA 	= 0x5,
	VGA_COLOR_BROWN 	= 0x6,
	VGA_COLOR_LIGHT_GREY 	= 0x7,
	VGA_COLOR_DARK_GREY 	= 0x8,
	VGA_COLOR_LBLUE 	= 0x9,
	VGA_COLOR_LGREEN 	= 0xA,
	VGA_COLOR_LCYAN 	= 0xB,
	VGA_COLOR_LRED 		= 0xC,
	VGA_COLOR_LMAGENTA 	= 0xD,
	VGA_COLOR_LBROWN 	= 0xE,
	VGA_COLOR_WHITE 	= 0xF
};

#define LIN_BUF 0xA0000
#define CGA_BUF 0xB8000

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) CGA_BUF; //Starts at 0xA0000; Between 0xB8000 and 0xA0000, there are 24.5 screens of space; 32.767 screens of space in linear buffer total (as CGA)

static inline uint8_t vga_entry_color(enum VGA_COLOR fg, enum VGA_COLOR bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char c, uint8_t vga_color) {
	return (uint16_t) c | (uint16_t) vga_color << 8;
}

#endif
