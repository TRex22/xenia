/*
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/cpu/ppc/ppc_emit-private.h"

#include "xenia/base/assert.h"
#include "xenia/cpu/ppc/ppc_context.h"
#include "xenia/cpu/ppc/ppc_hir_builder.h"

namespace xe {
namespace cpu {
namespace ppc {

// TODO(benvanik): remove when enums redefined.
using namespace xe::cpu::hir;

using xe::cpu::hir::RoundMode;
using xe::cpu::hir::Value;

// Good source of information:
// http://mamedev.org/source/src/emu/cpu/powerpc/ppc_ops.c
// The correctness of that code is not reflected here yet -_-

// Enable rounding numbers to single precision as required.
// This adds a bunch of work per operation and I'm not sure it's required.
#define ROUND_TO_SINGLE

// Floating-point arithmetic (A-8)

int InstrEmit_faddx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA) + (frB)
  Value* v = f.Add(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRB));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_faddsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA) + (frB)
  Value* v = f.Add(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRB));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fdivx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- frA / frB
  Value* v = f.Div(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRB));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fdivsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- frA / frB
  Value* v = f.Div(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRB));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fmulx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA) x (frC)
  Value* v = f.Mul(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fmulsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA) x (frC)
  Value* v = f.Mul(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fresx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- 1.0 / (frB)
  Value* v = f.Convert(f.Div(f.LoadConstantFloat32(1.0f),
                             f.Convert(f.LoadFPR(i.A.FRB), FLOAT32_TYPE)),
                       FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_frsqrtex(PPCHIRBuilder& f, const InstrData& i) {
  // Double precision:
  // frD <- 1/sqrt(frB)
  Value* v = f.RSqrt(f.LoadFPR(i.A.FRB));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fsubx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA) - (frB)
  Value* v = f.Sub(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRB));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fsubsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA) - (frB)
  Value* v = f.Sub(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRB));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fselx(PPCHIRBuilder& f, const InstrData& i) {
  // if (frA) >= 0.0
  // then frD <- (frC)
  // else frD <- (frB)
  Value* ge = f.CompareSGE(f.LoadFPR(i.A.FRA), f.LoadZeroFloat64());
  Value* v = f.Select(ge, f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fsqrtx(PPCHIRBuilder& f, const InstrData& i) {
  // Double precision:
  // frD <- sqrt(frB)
  Value* v = f.Sqrt(f.LoadFPR(i.A.FRB));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fsqrtsx(PPCHIRBuilder& f, const InstrData& i) {
  // Single precision:
  // frD <- sqrt(frB)
  Value* v = f.Sqrt(f.LoadFPR(i.A.FRB));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

// Floating-point multiply-add (A-9)

int InstrEmit_fmaddx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA x frC) + frB
  Value* v =
      f.MulAdd(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fmaddsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA x frC) + frB
  Value* v =
      f.MulAdd(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fmsubx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA x frC) - frB
  Value* v =
      f.MulSub(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fmsubsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frA x frC) - frB
  Value* v =
      f.MulSub(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fnmaddx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- -([frA x frC] + frB)
  Value* v = f.Neg(
      f.MulAdd(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB)));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fnmaddsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- -([frA x frC] + frB)
  Value* v = f.Neg(
      f.MulAdd(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB)));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fnmsubx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- -([frA x frC] - frB)
  Value* v = f.Neg(
      f.MulSub(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB)));
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fnmsubsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- -([frA x frC] - frB)
  Value* v = f.Neg(
      f.MulSub(f.LoadFPR(i.A.FRA), f.LoadFPR(i.A.FRC), f.LoadFPR(i.A.FRB)));
  v = f.Convert(f.Convert(v, FLOAT32_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.A.FRT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

// Floating-point rounding and conversion (A-10)

int InstrEmit_fcfidx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- signed_int64_to_double( frB )
  Value* v = f.Convert(f.Cast(f.LoadFPR(i.X.RB), INT64_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.X.RT, v);
  f.UpdateFPSCR(v, i.A.Rc);
  return 0;
}

int InstrEmit_fctidx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- double_to_signed_int64( frB )
  // TODO(benvanik): pull from FPSCR[RN]
  RoundMode round_mode = ROUND_TO_ZERO;
  Value* v = f.Convert(f.LoadFPR(i.X.RB), INT64_TYPE, round_mode);
  v = f.Cast(v, FLOAT64_TYPE);
  f.StoreFPR(i.X.RT, v);
  f.UpdateFPSCR(v, i.X.Rc);
  return 0;
}

int InstrEmit_fctidzx(PPCHIRBuilder& f, const InstrData& i) {
  // TODO(benvanik): assuming round to zero is always set, is that ok?
  return InstrEmit_fctidx(f, i);
}

int InstrEmit_fctiwx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- double_to_signed_int32( frB )
  // TODO(benvanik): pull from FPSCR[RN]
  RoundMode round_mode = ROUND_TO_ZERO;
  Value* v = f.Convert(f.LoadFPR(i.X.RB), INT32_TYPE, round_mode);
  v = f.Cast(f.ZeroExtend(v, INT64_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.X.RT, v);
  f.UpdateFPSCR(v, i.X.Rc);
  return 0;
}

int InstrEmit_fctiwzx(PPCHIRBuilder& f, const InstrData& i) {
  // TODO(benvanik): assuming round to zero is always set, is that ok?
  return InstrEmit_fctiwx(f, i);
}

int InstrEmit_frspx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- Round_single(frB)
  // TODO(benvanik): pull from FPSCR[RN]
  RoundMode round_mode = ROUND_TO_ZERO;
  Value* v = f.Convert(f.LoadFPR(i.X.RB), FLOAT32_TYPE, round_mode);
  v = f.Convert(v, FLOAT64_TYPE);
  f.StoreFPR(i.X.RT, v);
  f.UpdateFPSCR(v, i.X.Rc);
  return 0;
}

// Floating-point compare (A-11)

int InstrEmit_fcmpx_(PPCHIRBuilder& f, const InstrData& i, bool ordered) {
  // if (FRA) is a NaN or (FRB) is a NaN then
  //   c <- 0b0001
  // else if (FRA) < (FRB) then
  //   c <- 0b1000
  // else if (FRA) > (FRB) then
  //   c <- 0b0100
  // else {
  //   c <- 0b0010
  // }
  // FPCC <- c
  // CR[4*BF:4*BF+3] <- c
  // if (FRA) is an SNaN or (FRB) is an SNaN then
  //   VXSNAN <- 1

  // TODO(benvanik): update FPCC for mffsx/etc
  // TODO(benvanik): update VXSNAN
  const uint32_t crf = i.X.RT >> 2;
  f.UpdateCR(crf, f.LoadFPR(i.X.RA), f.LoadFPR(i.X.RB));
  return 0;
}
int InstrEmit_fcmpo(PPCHIRBuilder& f, const InstrData& i) {
  return InstrEmit_fcmpx_(f, i, true);
}
int InstrEmit_fcmpu(PPCHIRBuilder& f, const InstrData& i) {
  return InstrEmit_fcmpx_(f, i, false);
}

// Floating-point status and control register (A

int InstrEmit_mcrfs(PPCHIRBuilder& f, const InstrData& i) {
  XEINSTRNOTIMPLEMENTED();
  return 1;
}

int InstrEmit_mffsx(PPCHIRBuilder& f, const InstrData& i) {
  if (i.X.Rc) {
    XEINSTRNOTIMPLEMENTED();
    return 1;
  }
  Value* v = f.Cast(f.ZeroExtend(f.LoadFPSCR(), INT64_TYPE), FLOAT64_TYPE);
  f.StoreFPR(i.X.RT, v);
  return 0;
}

int InstrEmit_mtfsb0x(PPCHIRBuilder& f, const InstrData& i) {
  XEINSTRNOTIMPLEMENTED();
  return 1;
}

int InstrEmit_mtfsb1x(PPCHIRBuilder& f, const InstrData& i) {
  XEINSTRNOTIMPLEMENTED();
  return 1;
}

int InstrEmit_mtfsfx(PPCHIRBuilder& f, const InstrData& i) {
  if (i.XFL.L) {
    // Move/shift.
    XEINSTRNOTIMPLEMENTED();
    return 1;
  } else {
    // Directly store.
    // TODO(benvanik): use w/field mask to select bits.
    // i.XFL.W;
    // i.XFL.FM;
    f.StoreFPSCR(
        f.Truncate(f.Cast(f.LoadFPR(i.XFL.RB), INT64_TYPE), INT32_TYPE));
  }
  if (i.XFL.Rc) {
    f.CopyFPSCRToCR1();
  }
  return 0;
}

int InstrEmit_mtfsfix(PPCHIRBuilder& f, const InstrData& i) {
  XEINSTRNOTIMPLEMENTED();
  return 1;
}

// Floating-point move (A-21)

int InstrEmit_fabsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- abs(frB)
  Value* v = f.Abs(f.LoadFPR(i.X.RB));
  f.StoreFPR(i.X.RT, v);
  f.UpdateFPSCR(v, i.X.Rc);
  return 0;
}

int InstrEmit_fmrx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- (frB)
  Value* v = f.LoadFPR(i.X.RB);
  f.StoreFPR(i.X.RT, v);
  f.UpdateFPSCR(v, i.X.Rc);
  return 0;
}

int InstrEmit_fnabsx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- !abs(frB)
  Value* v = f.Neg(f.Abs(f.LoadFPR(i.X.RB)));
  f.StoreFPR(i.X.RT, v);
  f.UpdateFPSCR(v, i.X.Rc);
  return 0;
}

int InstrEmit_fnegx(PPCHIRBuilder& f, const InstrData& i) {
  // frD <- ¬ frB[0] || frB[1-63]
  Value* v = f.Neg(f.LoadFPR(i.X.RB));
  f.StoreFPR(i.X.RT, v);
  f.UpdateFPSCR(v, i.X.Rc);
  return 0;
}

void RegisterEmitCategoryFPU() {
  XEREGISTERINSTR(faddx);
  XEREGISTERINSTR(faddsx);
  XEREGISTERINSTR(fdivx);
  XEREGISTERINSTR(fdivsx);
  XEREGISTERINSTR(fmulx);
  XEREGISTERINSTR(fmulsx);
  XEREGISTERINSTR(fresx);
  XEREGISTERINSTR(frsqrtex);
  XEREGISTERINSTR(fsubx);
  XEREGISTERINSTR(fsubsx);
  XEREGISTERINSTR(fselx);
  XEREGISTERINSTR(fsqrtx);
  XEREGISTERINSTR(fsqrtsx);
  XEREGISTERINSTR(fmaddx);
  XEREGISTERINSTR(fmaddsx);
  XEREGISTERINSTR(fmsubx);
  XEREGISTERINSTR(fmsubsx);
  XEREGISTERINSTR(fnmaddx);
  XEREGISTERINSTR(fnmaddsx);
  XEREGISTERINSTR(fnmsubx);
  XEREGISTERINSTR(fnmsubsx);
  XEREGISTERINSTR(fcfidx);
  XEREGISTERINSTR(fctidx);
  XEREGISTERINSTR(fctidzx);
  XEREGISTERINSTR(fctiwx);
  XEREGISTERINSTR(fctiwzx);
  XEREGISTERINSTR(frspx);
  XEREGISTERINSTR(fcmpo);
  XEREGISTERINSTR(fcmpu);
  XEREGISTERINSTR(mcrfs);
  XEREGISTERINSTR(mffsx);
  XEREGISTERINSTR(mtfsb0x);
  XEREGISTERINSTR(mtfsb1x);
  XEREGISTERINSTR(mtfsfx);
  XEREGISTERINSTR(mtfsfix);
  XEREGISTERINSTR(fabsx);
  XEREGISTERINSTR(fmrx);
  XEREGISTERINSTR(fnabsx);
  XEREGISTERINSTR(fnegx);
}

}  // namespace ppc
}  // namespace cpu
}  // namespace xe
