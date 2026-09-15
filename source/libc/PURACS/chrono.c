#include <PURACS/chrono.h>
#include <kernel/drivers/pit.h>
//#include <stdio.h>

void ksleep(uint64_t ms) {
    uint64_t target_ticks = sysclock + ms;
	//printf("%p\n%p", &target_ticks, __builtin_frame_address(0));

    while (sysclock < target_ticks) {
		//printf("%u\n", sysclock);
        __asm__ __volatile__ ("hlt" : : : "memory");
    }
}
