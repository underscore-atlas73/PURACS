#ifndef L_PIC_H
#define L_PIC_H

#include <stdint.h>

#define PIC1_COMMAND 0x20  // Master PIC command port
#define PIC1_DATA    0x21  // Master PIC data port
#define PIC2_COMMAND 0xA0  // Slave PIC command port
#define PIC2_DATA    0xA1  // Slave PIC data port

#define ICW1_INIT    0x10  // Initialization command
#define ICW1_ICW4    0x01  // Indicates that ICW4 will be present
#define ICW4_8086    0x01  // 8086/88 (x86) mode

#define PIC_EOI 0x20

#define PIC_PIT		0x0
#define PIC_KB		0x1
#define PIC_CASC	0x2	//NEVER RAISED.
#define PIC_COM2	0x3
#define PIC_COM1	0x4
#define PIC_LPT2	0x5
#define PIC_FLOP	0x6
#define PIC_LPT1	0x7
#define PIC_SPUR	0x7
#define PIC_RTC		0x8
#define PIC_PER0	0x9
#define PIC_PER1	0xA
#define PIC_MOUS	0xB
#define PIC_COPR	0xC
#define PIC_FPU		0xC
#define PIC_pATA	0xD	//Primary, not parallel
#define PIC_sATA	0xE	//Secondary, not serial

void PIC_set_mask(uint8_t);
void PIC_clear_mask(uint8_t);
void PIC_remap(int offset1, int offset2);
void PIC_send_eoi(uint8_t irq);

#endif
