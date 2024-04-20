#ifndef _BOOT_H
#define _BOOT_H
#include "stdint.h"

extern "C" void delay(unsigned long);
extern "C" void put32(unsigned long, unsigned int);
extern "C" unsigned int get32(unsigned long);
extern "C" int get_el(void);
extern "C" int get_core_number(void);

extern "C" void monitor(uintptr_t);
extern "C" void wakeUpCores(void);

extern "C" int atomic_exchange(volatile int* addr, int new_value);



#endif /*_BOOT_H */
