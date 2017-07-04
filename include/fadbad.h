// Copyright (C) 1996-2007 Ole Stauning & Claus Bendtsen (fadbad@uning.dk)
// All rights reserved.

// This code is provided "as is", without any warranty of any kind,
// either expressed or implied, including but not limited to, any implied
// warranty of merchantibility or fitness for any purpose. In no event
// will any party who distributed the code be liable for damages or for
// any claim(s) by any other party, including but not limited to, any
// lost profits, lost monies, lost data or data rendered inaccurate,
// losses sustained by third parties, or any other special, incidental or
// consequential damages arising out of the use or inability to use the
// program, even if the possibility of such damages has been advised
// against. The entire risk as to the quality, the performance, and the
// fitness of the program for any particular purpose lies with the party
// using the code.

// This code, and any derivative of this code, may not be used in a
// commercial package without the prior explicit written permission of
// the authors. Verbatim copies of this code may be made and distributed
// in any medium, provided that this copyright notice is not removed or
// altered in any way. No fees may be charged for distribution of the
// codes, other than a fee to cover the cost of the media and a
// reasonable handling fee.

// ***************************************************************
// ANY USE OF THIS CODE CONSTITUTES ACCEPTANCE OF THE TERMS OF THE
//                         COPYRIGHT NOTICE
// ***************************************************************

#ifndef _FADBAD_H
#define _FADBAD_H

#include <math.h>

// For test purpose
#include <iostream>
using namespace std;

// mpreal.h include mpfr.h, including gmp.h
#include "mpreal.h"
using namespace mpfr;

namespace fadbad
{
#define PI 3.14159265358979323846
// NOTE:
// The following template allows the user to change the operations that
// are used in FADBAD++ for computing the derivatives. This is useful
// for example for specializing with non-standard types such as interval
// arithmetic types.
template <typename T>
struct Op  // YOU MIGHT NEED TO SPECIALIZE THIS TEMPLATE:
{
  typedef T   Base;
  static Base myInteger(const int i) { return Base(i); }
  static Base                     myZero() { return myInteger(0); }
  static Base                     myOne() { return myInteger(1); }
  static Base                     myTwo() { return myInteger(2); }
  static Base                     myPI() { return PI; }
  static T myPos(const T &x) { return +x; }
  static T myNeg(const T &x) { return -x; }
  template <typename U>
  static T &myCadd(T &x, const U &y)
  {
    return x += y;
  }
  template <typename U>
  static T &myCsub(T &x, const U &y)
  {
    return x -= y;
  }
  template <typename U>
  static T &myCmul(T &x, const U &y)
  {
    return x *= y;
  }
  template <typename U>
  static T &myCdiv(T &x, const U &y)
  {
    return x /= y;
  }
  static T myInv(const T &x) { return myOne() / x; }
  static T mySqr(const T &x) { return x * x; }
  template <typename X, typename Y>
  static T myPow(const X &x, const Y &y)
  {
    return ::pow(x, y);
  }
  static T mySqrt(const T &x) { return ::sqrt(x); }
  static T myLog(const T &x) { return ::log(x); }
  static T myExp(const T &x) { return ::exp(x); }
  static T mySin(const T &x) { return ::sin(x); }
  static T myCos(const T &x) { return ::cos(x); }
  static T myTan(const T &x) { return ::tan(x); }
  static T myAsin(const T &x) { return ::asin(x); }
  static T myAcos(const T &x) { return ::acos(x); }
  static T myAtan(const T &x) { return ::atan(x); }
  static bool myEq(const T &x, const T &y) { return x == y; }
  static bool myNe(const T &x, const T &y) { return x != y; }
  static bool myLt(const T &x, const T &y) { return x < y; }
  static bool myLe(const T &x, const T &y) { return x <= y; }
  static bool myGt(const T &x, const T &y) { return x > y; }
  static bool myGe(const T &x, const T &y) { return x >= y; }
};

#define DEFAULT_PREC (mpfr_get_default_prec())
#define DEFAULT_RNDM (mpfr_get_default_rounding_mode())
template <>
struct Op<mpreal>  //  SPECIALIZED TEMPLATE FOR mpreal class:
{
  typedef mpreal Base;
  static Base myInteger(const int i) { return Base(i); }
  static Base                     myZero() { return myInteger(0); }
  static Base                     myOne() { return myInteger(1); }
  static Base                     myTwo() { return myInteger(2); }
  static Base                     myPI() { return PI; }
  // static mpreal myPos(const mpreal& x) { return +x; }
  static void mpreal_pos(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM) { rop = x; }
  // static mpreal myNeg(const mpreal& x) { return -x; }
  static void mpreal_neg(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    mpfr_neg(rop.mpfr_ptr(), x.mpfr_srcptr(), rnd);
  }
  template <typename U>
  static mpreal &myCadd(mpreal &x, const U &y)
  {
    return x += y;
  }
  template <typename U>
  static mpreal &myCsub(mpreal &x, const U &y)
  {
    return x -= y;
  }
  template <typename U>
  static mpreal &myCmul(mpreal &x, const U &y)
  {
    return x *= y;
  }
  template <typename U>
  static mpreal &myCdiv(mpreal &x, const U &y)
  {
    return x /= y;
  }

  // mpreal_add
  static void mpreal_add(mpreal &rop, const mpreal &op1, const double &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_add_d(rop.mpfr_ptr(), op1.mpfr_ptr(), op2, rnd);
  }
  static void mpreal_add(mpreal &rop, const double &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_add_d(rop.mpfr_ptr(), op2.mpfr_ptr(), op1, rnd);
  }
  static void mpreal_add(mpreal &rop, const mpreal &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_add(rop.mpfr_ptr(), op1.mpfr_ptr(), op2.mpfr_ptr(), rnd);
  }
  // mpreal_sub
  static void mpreal_sub(mpreal &rop, const mpreal &op1, const double &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_sub_d(rop.mpfr_ptr(), op1.mpfr_ptr(), op2, rnd);
  }
  static void mpreal_sub(mpreal &rop, const double &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_d_sub(rop.mpfr_ptr(), op1, op2.mpfr_ptr(), rnd);
  }
  static void mpreal_sub(mpreal &rop, const mpreal &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_sub(rop.mpfr_ptr(), op1.mpfr_ptr(), op2.mpfr_ptr(), rnd);
  }
  // mpreal_mul
  static void mpreal_mul(mpreal &rop, const mpreal &op1, const double &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_mul_d(rop.mpfr_ptr(), op1.mpfr_ptr(), op2, rnd);
  }
  static void mpreal_mul(mpreal &rop, const double &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_mul_d(rop.mpfr_ptr(), op2.mpfr_ptr(), op1, rnd);
  }
  static void mpreal_mul(mpreal &rop, const mpreal &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_mul(rop.mpfr_ptr(), op1.mpfr_ptr(), op2.mpfr_ptr(), rnd);
  }
  // mpreal_div
  static void mpreal_div(mpreal &rop, const mpreal &op1, const double &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_div_d(rop.mpfr_ptr(), op1.mpfr_ptr(), op2, rnd);
  }
  static void mpreal_div(mpreal &rop, const double &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_d_div(rop.mpfr_ptr(), op1, op2.mpfr_ptr(), rnd);
  }
  static void mpreal_div(mpreal &rop, const mpreal &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_div(rop.mpfr_ptr(), op1.mpfr_ptr(), op2.mpfr_ptr(), rnd);
  }

  // template <typename X, typename Y>
  // static mpreal myPow(const X& x, const Y& y) { return ::pow(x,y); }
  // mpreal_pow
  static void mpreal_pow(mpreal &rop, const mpreal &op1, const double &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_pow(rop.mpfr_ptr(), op1.mpfr_ptr(), mpreal(op2).mpfr_ptr(), rnd);
  }
  static void mpreal_pow(mpreal &rop, const double &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_pow(rop.mpfr_ptr(), mpreal(op1).mpfr_ptr(), op2.mpfr_ptr(), rnd);
  }
  static void mpreal_pow(mpreal &rop, const mpreal &op1, const mpreal &op2,
                         mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_pow(rop.mpfr_ptr(), op1.mpfr_ptr(), op2.mpfr_ptr(), rnd);
  }

  // static mpreal myInv(const mpreal& x) { return myOne()/x; }
  static void mpreal_inv(mpreal &rop, const mpreal &x) { Op<mpreal>::mpreal_div(rop, 1.0, x); }
  // static mpreal mySqr(const mpreal& x) { return x*x; }
  static void mpreal_sqr(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_sqr(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }

  //static mpreal mySqrt(const mpreal& x) { return ::sqrt(x); }
  static void mpreal_sqrt(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_sqrt(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  //static mpreal myLog(const mpreal& x) { return ::log(x); }
  static void mpreal_log(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_log(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  static void mpreal_log2(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_log2(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  static void mpreal_log10(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_log10(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  //static mpreal myExp(const mpreal& x) { return ::exp(x); }
  static void mpreal_exp(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_exp(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  static void mpreal_exp2(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_exp2(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  static void mpreal_exp10(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_exp10(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  //static mpreal mySin(const mpreal& x) { return ::sin(x); }
  static void mpreal_sin(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_sin(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  //static mpreal myCos(const mpreal& x) { return ::cos(x); }
  static void mpreal_cos(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_cos(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  //static mpreal myTan(const mpreal& x) { return ::tan(x); }
  static void mpreal_tan(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_tan(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  //static mpreal myAsin(const mpreal& x) { return ::asin(x); }
  static void mpreal_asin(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_asin(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  //static mpreal myAcos(const mpreal& x) { return ::acos(x); }
  static void mpreal_acos(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_acos(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }
  //static mpreal myAtan(const mpreal& x) { return ::atan(x); }
  static void mpreal_atan(mpreal &rop, const mpreal &x, mpfr_rnd_t rnd = DEFAULT_RNDM)
  {
    if (rop.get_prec() != DEFAULT_PREC)
      rop.set_prec(DEFAULT_PREC, DEFAULT_RNDM);
    mpfr_atan(rop.mpfr_ptr(), x.mpfr_ptr(), rnd);
  }

  static bool myEq(const mpreal &x, const mpreal &y) { return x == y; }
  static bool myNe(const mpreal &x, const mpreal &y) { return x != y; }
  static bool myLt(const mpreal &x, const mpreal &y) { return x < y; }
  static bool myLe(const mpreal &x, const mpreal &y) { return x <= y; }
  static bool myGt(const mpreal &x, const mpreal &y) { return x > y; }
  static bool myGe(const mpreal &x, const mpreal &y) { return x >= y; }
};

static mpreal TEMP_RESULT  = 0.0;
static mpreal TEMP_RESULT1 = 0.0;
}  // namespace fadbad

// Name for backward AD type:
#define BTypeName B

// Name for forward AD type:
#define FTypeName F

// Name for taylor AD type:
#define TTypeName T

// Should always be inline:
#define INLINE0 inline

// Methods with only one line:
#define INLINE1 inline

// Methods with more than one line:
#define INLINE2 inline

#ifdef __SUNPRO_CC
// FOR SOME REASON SOME INLINES CAUSES
// UNRESOLVED SMBOLS ON SUN.
#undef INLINE0
#undef INLINE1
#undef INLINE2
#define INLINE0
#define INLINE1
#define INLINE2
#endif

// Define this if you want assertions, etc..
#ifdef _DEBUG

#include <iostream>
#include <sstream>

inline void ReportError(const char *errmsg) { std::cout << errmsg << std::endl; }
#define USER_ASSERT(check, msg)                                                          \
  if (!(check))                                                                          \
  {                                                                                      \
    std::ostringstream ost;                                                              \
    ost << "User assertion failed: \"" << msg << "\", at line " << __LINE__ << ", file " \
        << __FILE__ << std::endl;                                                        \
    ReportError(ost.str().c_str());                                                      \
  }
#define INTERNAL_ASSERT(check, msg)                                                           \
  if (!(check))                                                                               \
  {                                                                                           \
    std::ostringstream ost;                                                                   \
    ost << "Internal error: \"" << msg << "\", at line " << __LINE__ << ", file " << __FILE__ \
        << std::endl;                                                                         \
    ReportError(ost.str().c_str());                                                           \
  }
#define ASSERT(check)                                                                   \
  if (!(check))                                                                         \
  {                                                                                     \
    std::ostringstream ost;                                                             \
    ost << "Internal error at line " << __LINE__ << ", file " << __FILE__ << std::endl; \
    ReportError(ost.str().c_str());                                                     \
  }
#ifdef _TRACE
#define DEBUG(code) code;
#else
#define DEBUG(code)
#endif

#else

#define USER_ASSERT(check, msg)
#define INTERNAL_ASSERT(check, msg)
#define ASSERT(check)
#define DEBUG(code)

#endif

#endif
