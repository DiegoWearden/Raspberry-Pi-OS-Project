#ifndef KERNEL_H
#define KERNEL_H

#ifdef __ASSEMBLER__
// Place any assembly-specific declarations or macros here
#else

#ifdef __cplusplus
extern "C" {
#endif

void kernel_init(void);

#ifdef __cplusplus
}
#endif

extern "C" void uart_init_from_assembly();

#ifdef __cplusplus
extern "C" {
#endif

void print();

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__
#endif // KERNEL_H
