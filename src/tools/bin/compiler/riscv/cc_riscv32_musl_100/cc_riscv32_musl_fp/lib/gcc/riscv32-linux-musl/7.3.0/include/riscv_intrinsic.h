#ifndef _RISCV_INTRINSIC_H_
#define _RISCV_INTRINSIC_H_

__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
prefetch_data (const unsigned char a, unsigned int b)
{
  __builtin_riscv_prefd (a, b);
}

__extension__ extern __inline void
__attribute__ ((__always_inline__, __gnu_inline__, __artificial__))
prefetch_inst (const unsigned char a, unsigned int b)
{
  __builtin_riscv_prefi (a, b);
}

#endif
