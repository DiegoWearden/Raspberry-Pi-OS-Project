#ifndef _BOOT_H
#define _BOOT_H

extern "C" void delay(unsigned long);
extern "C" void put32(unsigned long, unsigned int);
extern "C" unsigned int get32(unsigned long);
extern "C" int get_el(void);
extern "C" int get_core_number(void);

#endif /*_BOOT_H */
