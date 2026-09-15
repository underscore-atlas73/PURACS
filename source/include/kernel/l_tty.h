#ifndef L_TTY_H
#define L_TTY_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <kernel/drivers/l_vga.h>

typedef struct {
	size_t		terminal_row;
	size_t		terminal_column;
	uint8_t		terminal_color;
        uint16_t*	terminal_buffer;
        size_t		buffer_size;
} Terminal;

extern Terminal terminals[8];

void terminal_init(Terminal* terminal, uint16_t* buffer, size_t buffer_size); //buf size in bytes
void terminal_putchar(uint8_t streamIndex, char c);
void terminal_write(uint8_t streamIndex, const char* data, size_t size);
void terminal_newline(uint8_t streamIndex);
void terminal_flush(uint8_t streamIndex);
//#define terminal_writestring(str) (terminal_write(str, strlen(str)))

#endif
