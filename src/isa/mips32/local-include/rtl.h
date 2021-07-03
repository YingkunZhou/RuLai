#ifndef __MIPS32_RTL_H__
#define __MIPS32_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

static inline void rtl_lr(DecodeExecState *s, rtlreg_t* dest, int r, int width) {
  rtl_mv(s, dest, &reg_l(r));
}

static inline void rtl_sr(DecodeExecState *s, int r, const rtlreg_t *src1, int width) {
  if (r != 0) { rtl_mv(s, &reg_l(r), src1); }
}

#endif
