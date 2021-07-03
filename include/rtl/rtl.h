#ifndef __RTL_RTL_H__
#define __RTL_RTL_H__

#include <rtl/rtl-basic.h>
#include <cpu/decode.h>

#define id_src1 (&s->src1)
#define id_src2 (&s->src2)
#define id_dest (&s->dest)

#define dsrc1 (id_src1->preg)
#define dsrc2 (id_src2->preg)
#define ddest (id_dest->preg)
#define s0    (&s->tmp_reg[0])
#define s1    (&s->tmp_reg[1])
#define s2    (&s->tmp_reg[2])
#define t0    (&s->tmp_reg[3])

/* RTL basic instructions */

#define make_rtl_compute_reg(name) \
  static inline void rtl_ ## name(DecodeExecState *s, \
      rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = c_ ## name (*src1, *src2); \
  }

#define make_rtl_compute_imm(name) \
  static inline void rtl_ ## name ## i(DecodeExecState *s, \
      rtlreg_t* dest, const rtlreg_t* src1, sword_t imm) { \
    *dest = c_ ## name (*src1, imm); \
  }

#define make_rtl_compute_reg_imm(name) \
  make_rtl_compute_reg(name) \
  make_rtl_compute_imm(name) \

// compute
/* rtl_add(i) definition */
make_rtl_compute_reg_imm(add)
/* rtl_sub(i) definition */
make_rtl_compute_reg_imm(sub)
/* rtl_and(i) definition */
make_rtl_compute_reg_imm(and)
/* rtl_or(i) definition */
make_rtl_compute_reg_imm(or)
/* rtl_xor(i) definition */
make_rtl_compute_reg_imm(xor)
/* rtl_shl(i) definition */
make_rtl_compute_reg_imm(shl)
/* rtl_shr(i) definition */
make_rtl_compute_reg_imm(shr)
/* rtl_sar(i) definition */
make_rtl_compute_reg_imm(sar)

#ifdef ISA64
/* rtl_addw(i) definition */
make_rtl_compute_reg_imm(addw)
/* rtl_subw(i) definition */
make_rtl_compute_reg_imm(subw)
/* rtl_shlw(i) definition */
make_rtl_compute_reg_imm(shlw)
/* rtl_shrw(i) definition */
make_rtl_compute_reg_imm(shrw)
/* rtl_sarw(i) definition */
make_rtl_compute_reg_imm(sarw)

#define rtl_addiw rtl_addwi
#define rtl_shliw rtl_shlwi
#define rtl_shriw rtl_shrwi
#define rtl_sariw rtl_sarwi
#endif

static inline void rtl_setrelop(DecodeExecState *s, uint32_t relop,
    rtlreg_t *dest, const rtlreg_t *src1, const rtlreg_t *src2) {
  *dest = interpret_relop(relop, *src1, *src2);
}

static inline void rtl_setrelopi(DecodeExecState *s, uint32_t relop,
    rtlreg_t *dest, const rtlreg_t *src1, sword_t imm) {
  *dest = interpret_relop(relop, *src1, imm);
}

// mul/div

make_rtl_compute_reg(mul_lo)
make_rtl_compute_reg(mul_hi)
make_rtl_compute_reg(imul_lo)
make_rtl_compute_reg(imul_hi)
make_rtl_compute_reg(div_q)
make_rtl_compute_reg(div_r)
make_rtl_compute_reg(idiv_q)
make_rtl_compute_reg(idiv_r)

#ifdef ISA64
make_rtl_compute_reg(mulw)
make_rtl_compute_reg(divw)
make_rtl_compute_reg(divuw)
make_rtl_compute_reg(remw)
make_rtl_compute_reg(remuw)
#endif

static inline void rtl_div64_q(DecodeExecState *s, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline void rtl_div64_r(DecodeExecState *s, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline void rtl_idiv64_q(DecodeExecState *s, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline void rtl_idiv64_r(DecodeExecState *s, rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend % divisor;
}

// memory

static inline void rtl_lm(DecodeExecState *s, rtlreg_t *dest,
    const rtlreg_t* addr, word_t offset, int len) {
  word_t val = vaddr_read(*addr + offset, len);
  if (!isa_has_mem_exception()) *dest = val;
}

static inline void rtl_sm(DecodeExecState *s, const rtlreg_t* addr,
    word_t offset, const rtlreg_t* src1, int len) {
  vaddr_write(*addr + offset, *src1, len);
}

static inline void rtl_lms(DecodeExecState *s, rtlreg_t *dest,
    const rtlreg_t* addr, word_t offset, int len) {
  word_t val = vaddr_read(*addr + offset, len);
  if (!isa_has_mem_exception()) {
    switch (len) {
      case 4: *dest = (sword_t)(int32_t)val; return;
      case 1: *dest = (sword_t)( int8_t)val; return;
      case 2: *dest = (sword_t)(int16_t)val; return;
      default: assert(0);
    }
  }
}

static inline  void rtl_host_lm(DecodeExecState *s, rtlreg_t* dest,
    const void *addr, int len) {
  switch (len) {
    case 4: *dest = *(uint32_t *)addr; return;
    case 1: *dest = *( uint8_t *)addr; return;
    case 2: *dest = *(uint16_t *)addr; return;
    default: assert(0);
  }
}

static inline void rtl_host_sm(DecodeExecState *s, void *addr,
    const rtlreg_t *src1, int len) {
  switch (len) {
    case 4: *(uint32_t *)addr = *src1; return;
    case 1: *( uint8_t *)addr = *src1; return;
    case 2: *(uint16_t *)addr = *src1; return;
    default: assert(0);
  }
}

// control

static inline void rtl_j(DecodeExecState *s, vaddr_t target) {
  s->jmp_pc = target;
  s->is_jmp = true;
}

static inline void rtl_jr(DecodeExecState *s, rtlreg_t *target) {
  s->jmp_pc = *target;
  s->is_jmp = true;
}

static inline void rtl_jrelop(DecodeExecState *s, uint32_t relop,
    const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target) {
  bool is_jmp = interpret_relop(relop, *src1, *src2);
  if (is_jmp) rtl_j(s, target);
}
void rtl_exit(int state, vaddr_t halt_pc, uint32_t halt_ret);
void rtl_sfence();

extern const rtlreg_t rzero;
#define rz (&rzero)

/* RTL pseudo instructions */
static inline void rtl_li(DecodeExecState *s, rtlreg_t* dest, sword_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline void rtl_mv(DecodeExecState *s, rtlreg_t* dest, const rtlreg_t* src1) {
  if (dest != src1) rtl_add(s, dest, src1, rz);
}

static inline void rtl_not(DecodeExecState *s, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
//  TODO();
  rtl_xori(s, dest, src1, -1);
}

static inline void rtl_neg(DecodeExecState *s, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
//  TODO();
  rtl_sub(s, dest, rz, src1);
}

static inline void rtl_sext(DecodeExecState *s, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
//  TODO();

  const int word_size = sizeof(word_t);
  if (width == word_size) {
    rtl_mv(s, dest, src1);
  } else {
    assert(width == 1 || width == 2 || width == 4);
    rtl_shli(s, dest, src1, (word_size - width) * 8);
    rtl_sari(s, dest, dest, (word_size - width) * 8);
  }
}

static inline void rtl_zext(DecodeExecState *s, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
//  TODO();

  const int word_size = sizeof(word_t);
  if (width == word_size) {
    rtl_mv(s, dest, src1);
  } else {
    assert(width == 1 || width == 2 || width == 4);
    rtl_shli(s, dest, src1, (word_size - width) * 8);
    rtl_shri(s, dest, dest, (word_size - width) * 8);
  }
}

static inline void rtl_msb(DecodeExecState *s, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
//  TODO();
  rtl_shri(s, dest, src1, width * 8 - 1);
  if (width != 4) {
    rtl_andi(s, dest, dest, 0x1);
  }
}

#endif
