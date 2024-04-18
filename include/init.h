#ifndef KERNEL_H
#define KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

void kernel_init(void);

#ifdef __cplusplus
}
#endif

extern "C" void uart_init_from_assembly();

#endif // KERNEL_H