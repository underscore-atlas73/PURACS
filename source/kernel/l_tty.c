#include "kernel/drivers/l_vga.h"
#include <kernel/l_tty.h>
#include <stdint.h>

Terminal terminals[8] = {0};

void terminal_init(Terminal* terminal, uint16_t* buffer, size_t buffer_size) {
	terminal->terminal_row = 0;
	terminal->terminal_column = 0;
	terminal->terminal_color = 0x07;
        terminal->terminal_buffer = buffer;
	terminal->buffer_size = buffer_size & (SIZE_MAX - 1); //on 32-bit, 0xFFFFFFFE or 0x11111111...0
	for (size_t i = 0; i < buffer_size / 2; i++) {
		terminal->terminal_buffer[i] = vga_entry(' ', terminal->terminal_color);
	}
}

void terminal_putentryat(uint8_t streamIndex, unsigned char c, uint8_t color, size_t x, size_t y) {
	size_t pos;
        if ((pos = y * VGA_WIDTH + x) > terminals[streamIndex].buffer_size / 2)
		return;
        
	terminals[streamIndex].terminal_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

void terminal_putchar(uint8_t streamIndex, char c) {
	if (c == '\n') {
		terminal_newline(streamIndex);
		return;
        }
        if (c == '\t') {
		if ((terminals[streamIndex].terminal_column += 8 - terminals[streamIndex].terminal_column % 8) > VGA_WIDTH)
			terminal_newline(streamIndex);
		return;
        }
	
	terminal_putentryat(streamIndex, c, terminals[streamIndex].terminal_color, terminals[streamIndex].terminal_column, terminals[streamIndex].terminal_row);
	if (++(terminals[streamIndex].terminal_column) == VGA_WIDTH)
		terminal_newline(streamIndex);
}

void terminal_write(uint8_t streamIndex, const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) 
		terminal_putchar(streamIndex, data[i]);
}

static inline void terminal_scroll(uint8_t streamIndex) {
	size_t i = 0;;
	for (; i < terminals[streamIndex].buffer_size - VGA_WIDTH; i++)
		terminals[streamIndex].terminal_buffer[i] = terminals[streamIndex].terminal_buffer[i + VGA_WIDTH];  
}
void terminal_newline(uint8_t streamIndex) {
	terminals[streamIndex].terminal_column = 0;
	if (++(terminals[streamIndex].terminal_row) == (terminals[streamIndex].buffer_size / 2) / VGA_WIDTH) {
		terminals[streamIndex].terminal_row--;
		terminal_scroll(streamIndex);
		//TODO: Implement full scrolling + buffering. We have enough room between 0xB8000 and 0xBFFFF for 8 screens
	}
}

void terminal_flush(uint8_t streamIndex) { //unnecessarily slow when streamIndex is already the terminal being displayed; need videocard accelerated BitBLT. 
	size_t i;
        for (i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i++) {
		((uint8_t*)VGA_MEMORY)[i] = ((uint8_t*)terminals[streamIndex].terminal_buffer)[i];
        }
}
