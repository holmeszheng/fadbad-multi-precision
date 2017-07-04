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

#ifndef _FADIFF_H
#define _FADIFF_H

#include "fadbad.h"

namespace fadbad
{
template <typename T, unsigned int N = 0>
class FTypeName  // STACK-BASED     General Template
{
  T    m_val;
  T    m_diff[ N ];
  bool m_depend;

 public:
  typedef T UnderlyingType;
  FTypeName() : m_depend(false) {}
  FTypeName(const FTypeName<T, N>& val) : m_val(val.m_val), m_depend(val.m_depend)
  {
    if (m_depend)
      for (unsigned int i = 0; i < N; ++i)
        m_diff[ i ]       = val.m_diff[ i ];
  }
  template <class U> /*explicit*/ FTypeName(const U& val) : m_val(val), m_depend(false) {}
  template <class U>
  FTypeName<T, N>& operator=(const U& val)
  {
    m_val    = val;
    m_depend = false;
    return *this;
  }
  FTypeName<T, N>& operator=(const FTypeName<T, N>& val)
  {
    if (this == &val)
      return *this;
    m_val    = val.m_val;
    m_depend = val.m_depend;
    if (m_depend)
      for (unsigned int i = 0; i < N; ++i)
        m_diff[ i ]       = val.m_diff[ i ];
    return *this;
  }
  unsigned int size() const { return m_depend ? N : 0; }
  const T& operator[](const unsigned int i) const
  {
    USER_ASSERT(i < N && m_depend, "Index " << i << " out of bounds [0," << N << "]")
    return m_diff[ i ];
  }
  T& operator[](const unsigned int i)
  {
    USER_ASSERT(i < N && m_depend, "Index " << i << " out of bounds [0," << N << "]")
    return m_diff[ i ];
  }
  const T& val() const { return m_val; }
  T&       x() { return m_val; }
  const T& deriv(const unsigned int i) const
  {
    USER_ASSERT(i < N, "Index " << i << " out of bounds [0," << N << "]")
    if (m_depend)
      return m_diff[ i ];
    static T zero;
    zero = Op<T>::myZero();
    return zero;
  }
  T& d(const unsigned int i)
  {
    USER_ASSERT(i < N, "Index " << i << " out of bounds [0," << N << "]")
    if (m_depend)
      return m_diff[ i ];
    static T zero;
    zero = Op<T>::myZero();
    return zero;
  }

  T& diff(unsigned int idx)
  {
    USER_ASSERT(idx < N, "index out of bounds: " << idx);
    unsigned int i;
    for (i        = 0; i < idx; ++i)
      m_diff[ i ] = Op<T>::myZero();
    m_diff[ i++ ] = Op<T>::myOne();
    for (; i < N; ++i)
      m_diff[ i ] = Op<T>::myZero();
    m_depend      = true;
    return m_diff[ idx ];
  }
  bool depend() const { return m_depend; }
  void setDepend(const FTypeName<T, N>&) { m_depend = true; }
  void setDepend(const FTypeName<T, N>&, const FTypeName<T, N>&) { m_depend = true; }
  FTypeName<T, N>& operator+=(const FTypeName<T, N>& val);
  FTypeName<T, N>& operator-=(const FTypeName<T, N>& val);
  FTypeName<T, N>& operator*=(const FTypeName<T, N>& val);
  FTypeName<T, N>& operator/=(const FTypeName<T, N>& val);
  template <typename V>
  FTypeName<T, N>& operator+=(const V& val);
  template <typename V>
  FTypeName<T, N>& operator-=(const V& val);
  template <typename V>
  FTypeName<T, N>& operator*=(const V& val);
  template <typename V>
  FTypeName<T, N>& operator/=(const V& val);
};

template <typename T>
class FTypeName<T, 0>  // HEAP-BASED     Template specialized
{
  T            m_val;
  unsigned int m_size;
  T*           m_diff;

 public:
  typedef T UnderlyingType;
  // Constructors
  FTypeName() : m_val(), m_size(0), m_diff(0) {}
  FTypeName(const FTypeName<T>& val)
      : m_val(val.m_val), m_size(val.m_size), m_diff(m_size == 0 ? 0 : new T[ m_size ])
  {
    for (unsigned int i = 0; i < m_size; ++i)
      m_diff[ i ]       = val.m_diff[ i ];
  }
  template <class U> /*explicit*/ FTypeName(const U& val) : m_val(val), m_size(0), m_diff(0) {}
  // Destructor
  ~FTypeName() { delete[] m_diff; }
  // Operator = reloading
  template <class U>
  FTypeName<T>& operator=(const U& val)
  {
    m_val  = val;
    m_size = 0;
    delete[] m_diff;
    m_diff = 0;
    return *this;
  }
  FTypeName<T>& operator=(const FTypeName<T>& val)
  {
    if (this == &val)
      return *this;
    m_val = val.m_val;
    if (val.m_size > 0)
    {
      if (m_size == 0)
      {
        m_size = val.m_size;
        m_diff = new T[ m_size ];
      }
      // Operator = must guarantee that both have the same size
      USER_ASSERT(m_size == val.m_size, "derivative vectors not of same size");
      for (unsigned int i = 0; i < val.m_size; ++i)
        m_diff[ i ]       = val.m_diff[ i ];
    }
    else if (m_size > 0)
    {
      for (unsigned int i = 0; i < m_size; ++i)
        m_diff[ i ]       = 0;
    }
    return *this;
  }

  // size() method
  unsigned int size() const { return m_size; }
  // Operator [] reloading  get m_diff[i]
  // rvalue
  const T& operator[](const unsigned int i) const
  {
    USER_ASSERT(i < m_size, "index out of bounds: " << i);
    return m_diff[ i ];
  }
  // lvalue
  T& operator[](const unsigned int i)
  {
    USER_ASSERT(i < m_size, "index out of bounds: " << i);
    return m_diff[ i ];
  }

  // get m_val
  // rvalue
  const T& val() const { return m_val; }
  // lvalue
  T& x() { return m_val; }
  // get m_diff[i]
  // rvalue
  const T& deriv(const unsigned int i) const
  {
    if (i < m_size)
      return m_diff[ i ];
    static T zero;
    zero = Op<T>::myZero();
    return zero;
  }
  // lvalue
  T& d(const unsigned int i)
  {
    if (i < m_size)
      return m_diff[ i ];
    static T zero;
    zero = Op<T>::myZero();
    return zero;
  }

  // Set m_diff[idx] = 1 and return as lvalue
  T& diff(unsigned int idx, unsigned int N)
  {
    USER_ASSERT(idx < N, "Index " << idx << " out of bounds [0," << N << "]")
    if (m_size == 0)
    {
      m_size = N;
      m_diff = new T[ m_size ];
    }
    else
    {
      USER_ASSERT(m_size == N, "derivative vectors not of same size " << m_size << "," << N);
    }
    unsigned int i;
    for (i = 0; i < idx; ++i)
      m_diff[ i ] = Op<T>::myZero();  // first convert 0 to the same type T
    m_diff[ i++ ] = Op<T>::myOne();
    for (; i < N; ++i)
      m_diff[ i ] = Op<T>::myZero();
    return m_diff[ idx ];
  }

  // depend functions
  /*If dimensions of gradients of the two variables are the same, it means two variables depend.*/
  bool depend() const { return m_size != 0; }
  void setDepend(const FTypeName<T>& val)
  {
    INTERNAL_ASSERT(val.m_size > 0, "input is not a dependent variable")
    if (m_size == 0)
    {
      m_size = val.m_size;
      m_diff = new T[ m_size ];
    }
    else
    {
      USER_ASSERT(m_size == val.m_size, "derivative vectors not of same size " << m_size << ","
                                                                               << val.m_size);
    }
  }
  void setDepend(const FTypeName<T>& val1, const FTypeName<T>& val2)
  {
    USER_ASSERT(val1.m_size == val2.m_size, "derivative vectors not of same size "
                                                << val1.m_size << "," << val2.m_size);
    INTERNAL_ASSERT(val1.m_size > 0, "lhs-input is not a dependent variable")
    INTERNAL_ASSERT(val2.m_size > 0, "rhs-input is not a dependent variable")
    if (m_size == 0)
    {
      m_size = val1.m_size;
      m_diff = new T[ m_size ];
    }
    else
    {
      USER_ASSERT(m_size == val1.m_size, "derivative vectors not of same size " << m_size << ","
                                                                                << val1.m_size);
    }
  }

  FTypeName<T>& operator+=(const FTypeName<T>& val);
  FTypeName<T>& operator-=(const FTypeName<T>& val);
  FTypeName<T>& operator*=(const FTypeName<T>& val);
  FTypeName<T>& operator/=(const FTypeName<T>& val);
  template <typename V>
  FTypeName<T>& operator+=(const V& val);
  template <typename V>
  FTypeName<T>& operator-=(const V& val);
  template <typename V>
  FTypeName<T>& operator*=(const V& val);
  template <typename V>
  FTypeName<T>& operator/=(const V& val);
};

//-------------------------------------------Class FTypeName<T, N> member
//functions-------------------------------------------------------
// Parameter FTypeName<T, N>
template <typename T, unsigned int N>
FTypeName<T, N>& FTypeName<T, N>::operator+=(const FTypeName<T, N>& val)
{
  Op<T>::myCadd(m_val, val.m_val);
  if (!val.depend())
    return *this;
  if (this->depend())
  {
    for (unsigned int i = 0; i < N; ++i)
      Op<T>::myCadd(m_diff[ i ], val[ i ]);
  }
  else
  {
    this->setDepend(val);
    for (unsigned int i = 0; i < N; ++i)
      m_diff[ i ]       = val[ i ];
  }
  return *this;
}

template <typename T, unsigned int N>
FTypeName<T, N>& FTypeName<T, N>::operator-=(const FTypeName<T, N>& val)
{
  Op<T>::myCsub(m_val, val.m_val);
  if (!val.depend())
    return *this;
  if (this->depend())
  {
    for (unsigned int i = 0; i < N; ++i)
      Op<T>::myCsub(m_diff[ i ], val[ i ]);
  }
  else
  {
    this->setDepend(val);
    for (unsigned int i = 0; i < N; ++i)
      m_diff[ i ]       = Op<T>::myNeg(val[ i ]);
  }
  return *this;
}

template <typename T, unsigned int N>
FTypeName<T, N>& FTypeName<T, N>::operator*=(const FTypeName<T, N>& val)
{
  if (this->depend() && val.depend())
  {
    for (unsigned int i = 0; i < N; ++i)
      m_diff[ i ]       = m_diff[ i ] * val.m_val + val.m_diff[ i ] * m_val;
  }
  else if (this->depend())
  {
    for (unsigned int i = 0; i < N; ++i)
      Op<T>::myCmul(m_diff[ i ], val.m_val);
  }
  else  // (val.depend())
  {
    this->setDepend(val);
    for (unsigned int i = 0; i < N; ++i)
      m_diff[ i ]       = val.m_diff[ i ] * m_val;
  }
  Op<T>::myCmul(m_val, val.m_val);
  return *this;
}

template <typename T, unsigned int N>
FTypeName<T, N>& FTypeName<T, N>::operator/=(const FTypeName<T, N>& val)
{
  Op<T>::myCdiv(m_val, val.m_val);
  if (this->depend() && val.depend())
  {
    for (unsigned int i = 0; i < N; ++i)
      m_diff[ i ]       = (m_diff[ i ] - m_val * val.m_diff[ i ]) / val.m_val;
  }
  else if (this->depend())
  {
    for (unsigned int i = 0; i < N; ++i)
      Op<T>::myCdiv(m_diff[ i ], val.m_val);
  }
  else  // (val.depend())
  {
    this->setDepend(val);
    for (unsigned int i = 0; i < N; ++i)
      m_diff[ i ]       = Op<T>::myNeg(m_val * val.m_diff[ i ] / val.m_val);
  }
  return *this;
}

//------------------------------------------Class FTypeName<T, N> member
//functions----------------------------------------------------
// Parameter V type
template <typename T, unsigned int N>
template <typename V>
FTypeName<T, N>& FTypeName<T, N>::operator+=(const V& val)
{
  Op<T>::myCadd(m_val, val);
  return *this;
}
template <typename T, unsigned int N>
template <typename V>
FTypeName<T, N>& FTypeName<T, N>::operator-=(const V& val)
{
  Op<T>::myCsub(m_val, val);
  return *this;
}

template <typename T, unsigned int N>
template <typename V>
FTypeName<T, N>& FTypeName<T, N>::operator*=(const V& val)
{
  Op<T>::myCmul(m_val, val);
  if (!this->depend())
    return *this;
  for (unsigned int i = 0; i < N; ++i)
    Op<T>::myCmul(m_diff[ i ], val);
  return *this;
}

template <typename T, unsigned int N>
template <typename V>
FTypeName<T, N>& FTypeName<T, N>::operator/=(const V& val)
{
  Op<T>::myCdiv(m_val, val);
  if (!this->depend())
    return *this;
  for (unsigned int i = 0; i < N; ++i)
    Op<T>::myCdiv(m_diff[ i ], val);
  return *this;
}

//----------------------------------------------Class FTypeName<T,0> member
//functions----------------------------------------------------
// Parameter FTypeName<T, 0>
template <typename T>
FTypeName<T, 0>& FTypeName<T, 0>::operator+=(const FTypeName<T, 0>& val)
{
  Op<T>::myCadd(m_val, val.m_val);
  if (!val.depend())
    return *this;
  if (this->depend())
  {
    for (unsigned int i = 0; i < this->size(); ++i)
      Op<T>::myCadd(m_diff[ i ], val[ i ]);
  }
  else
  {
    this->setDepend(val);
    for (unsigned int i = 0; i < this->size(); ++i)
      m_diff[ i ]       = val[ i ];
  }
  return *this;
}
template <typename T>
FTypeName<T, 0>& FTypeName<T, 0>::operator-=(const FTypeName<T, 0>& val)
{
  Op<T>::myCsub(m_val, val.m_val);
  if (!val.depend())
    return *this;
  if (this->depend())
  {
    for (unsigned int i = 0; i < this->size(); ++i)
      Op<T>::myCsub(m_diff[ i ], val[ i ]);
  }
  else
  {
    this->setDepend(val);
    for (unsigned int i = 0; i < this->size(); ++i)
      m_diff[ i ]       = Op<T>::myNeg(val[ i ]);
  }
  return *this;
}
template <typename T>
FTypeName<T, 0>& FTypeName<T, 0>::operator*=(const FTypeName<T, 0>& val)
{
  if (this->depend() && val.depend())
  {
    for (unsigned int i = 0; i < this->size(); ++i)
      m_diff[ i ]       = m_diff[ i ] * val.m_val + val.m_diff[ i ] * m_val;
  }
  else if (this->depend())
  {
    for (unsigned int i = 0; i < this->size(); ++i)
      Op<T>::myCmul(m_diff[ i ], val.m_val);
  }
  else  // (val.depend())
  {
    this->setDepend(val);
    for (unsigned int i = 0; i < this->size(); ++i)
      m_diff[ i ]       = val.m_diff[ i ] * m_val;
  }
  Op<T>::myCmul(m_val, val.m_val);
  return *this;
}
template <typename T>
FTypeName<T, 0>& FTypeName<T, 0>::operator/=(const FTypeName<T, 0>& val)
{
  Op<T>::myCdiv(m_val, val.m_val);
  if (this->depend() && val.depend())
  {
    for (unsigned int i = 0; i < this->size(); ++i)
      m_diff[ i ]       = (m_diff[ i ] - m_val * val.m_diff[ i ]) / val.m_val;
  }
  else if (this->depend())
  {
    for (unsigned int i = 0; i < this->size(); ++i)
      Op<T>::myCdiv(m_diff[ i ], val.m_val);
  }
  else  // (val.depend())
  {
    this->setDepend(val);
    for (unsigned int i = 0; i < this->size(); ++i)
      m_diff[ i ]       = Op<T>::myNeg(m_val * val.m_diff[ i ] / val.m_val);
  }
  return *this;
}

//----------------------------------------------Class FTypeName<T,0> member
//functions-----------------------------------------------------
// Parameter V type
template <typename T>
template <typename V>
FTypeName<T, 0>& FTypeName<T, 0>::operator+=(const V& val)
{
  Op<T>::myCadd(m_val, val);
  return *this;
}
template <typename T>
template <typename V>
FTypeName<T, 0>& FTypeName<T, 0>::operator-=(const V& val)
{
  Op<T>::myCsub(m_val, val);
  return *this;
}
template <typename T>
template <typename V>
FTypeName<T, 0>& FTypeName<T, 0>::operator*=(const V& val)
{
  Op<T>::myCmul(m_val, val);
  if (!this->depend())
    return *this;
  for (unsigned int i = 0; i < this->size(); ++i)
    Op<T>::myCmul(m_diff[ i ], val);
  return *this;
}
template <typename T>
template <typename V>
FTypeName<T, 0>& FTypeName<T, 0>::operator/=(const V& val)
{
  Op<T>::myCdiv(m_val, val);
  if (!this->depend())
    return *this;
  for (unsigned int i = 0; i < this->size(); ++i)
    Op<T>::myCdiv(m_diff[ i ], val);
  return *this;
}

//---------------------------------------------------Non-member
//functions---------------------------------------------------------------
//-----------------------------------------------------Compare
//operations-----------------------------------------------------------------
// Function reloadings
// Two FTypeName<T, N> variables
template <typename T, unsigned int N>
bool operator==(const FTypeName<T, N>& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myEq(val1.val(), val2.val());
}
template <typename T, unsigned int N>
bool operator!=(const FTypeName<T, N>& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myNe(val1.val(), val2.val());
}
template <typename T, unsigned int N>
bool operator<(const FTypeName<T, N>& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myLt(val1.val(), val2.val());
}
template <typename T, unsigned int N>
bool operator<=(const FTypeName<T, N>& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myLe(val1.val(), val2.val());
}
template <typename T, unsigned int N>
bool operator>(const FTypeName<T, N>& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myGt(val1.val(), val2.val());
}
template <typename T, unsigned int N>
bool operator>=(const FTypeName<T, N>& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myGe(val1.val(), val2.val());
}
// One FTypeName<T, N> and One U type variable
template <typename T, unsigned int N, typename U>
bool operator==(const FTypeName<T, N>& val1, const U& val2)
{
  return Op<T>::myEq(val1.val(), val2);
}
template <typename T, unsigned int N, typename U>
bool operator==(const U& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myEq(val1, val2.val());
}
template <typename T, unsigned int N, typename U>
bool operator!=(const FTypeName<T, N>& val1, const U& val2)
{
  return Op<T>::myNe(val1.val(), val2);
}
template <typename T, unsigned int N, typename U>
bool operator!=(const U& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myNe(val1, val2.val());
}
template <typename T, unsigned int N, typename U>
bool operator<(const FTypeName<T, N>& val1, const U& val2)
{
  return Op<T>::myLt(val1.val(), val2);
}
template <typename T, unsigned int N, typename U>
bool operator<(const U& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myLt(val1, val2.val());
}
template <typename T, unsigned int N, typename U>
bool operator<=(const FTypeName<T, N>& val1, const U& val2)
{
  return Op<T>::myLe(val1.val(), val2);
}
template <typename T, unsigned int N, typename U>
bool operator<=(const U& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myLe(val1, val2.val());
}
template <typename T, unsigned int N, typename U>
bool operator>(const FTypeName<T, N>& val1, const U& val2)
{
  return Op<T>::myGt(val1.val(), val2);
}
template <typename T, unsigned int N, typename U>
bool operator>(const U& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myGt(val1, val2.val());
}
template <typename T, unsigned int N, typename U>
bool operator>=(const FTypeName<T, N>& val1, const U& val2)
{
  return Op<T>::myGe(val1.val(), val2);
}
template <typename T, unsigned int N, typename U>
bool operator>=(const U& val1, const FTypeName<T, N>& val2)
{
  return Op<T>::myGe(val1, val2.val());
}

//-----------------------------------------------------add1, add2, add3, Operator +
//---------------------------------------------------------------
// One FTypeName<T, N> and One U type variable
// function add1 for  FTypeName<T, N>    stack
template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T, N> add1(const U& a, const FTypeName<T, N>& b)
{
  FTypeName<T, N> c(a + b.val());
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = b[ i ];
  return c;
}
template <class U, unsigned int N>
INLINE2 FTypeName<mpreal, N> add1(const U& a, const FTypeName<mpreal, N>& b)
{
  Op<mpreal>::mpreal_add(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = b[ i ];
  return c;
}
// function reloading  add1     heap
// When variable is FTypeName<T, 0>, this is more concrete
template <typename T, class U>
INLINE2 FTypeName<T, 0> add1(const U& a, const FTypeName<T, 0>& b)
{
  FTypeName<T, 0> c(a + b.val());
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = b[ i ];
  return c;
}

template <class U>
INLINE2 FTypeName<mpreal, 0> add1(const U& a, const FTypeName<mpreal, 0>& b)
{
  Op<mpreal>::mpreal_add(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = b[ i ];
  return c;
}

// function add2 for  FTypeName<T, N>    stack
template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T, N> add2(const FTypeName<T, N>& a, const U& b)
{
  FTypeName<T, N> c(a.val() + b);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ];
  return c;
}
template <class U, unsigned int N>
INLINE2 FTypeName<mpreal, N> add2(const FTypeName<mpreal, N>& a, const U& b)
{
  Op<mpreal>::mpreal_add(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);  // when a.size>0     assign this to c.size and allocate m_diff
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ];  // sum gradient
  return c;
}
// function reloading  add2     heap
// When variable is FTypeName<T, 0>, this is more concrete
template <typename T, class U>
INLINE2 FTypeName<T, 0> add2(const FTypeName<T, 0>& a, const U& b)
{
  FTypeName<T, 0> c(a.val() + b);  // construct c.m_value(sum), size(0)
  if (!a.depend())
    return c;
  c.setDepend(a);  // when a.size>0     assign this to c.size and allocate m_diff
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ];  // sum gradient
  return c;
}

template <class U>
INLINE2 FTypeName<mpreal, 0> add2(const FTypeName<mpreal, 0>& a, const U& b)
{
  Op<mpreal>::mpreal_add(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);  // when a.size>0     assign this to c.size and allocate m_diff
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ];  // sum gradient
  return c;
}

// operator+ reloading
// One U type variable and One FTypeName<T, N>
// general function     Don't need reloading function for FTypeName<T, 0>.
// when calling add1, according to the type of a and b, it calls different add1.
template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> operator+(const U& a, const FTypeName<T, N>& b)
{
  return add1(a, b);
}
// One FTypeName<T, N> and One U type variable
// general function     Don't need reloading function for FTypeName<T, 0>.
// when calling add2, according to the type of a and b, it calls different add2.
template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> operator+(const FTypeName<T, N>& a, const U& b)
{
  return add2(a, b);
}

// add3      don't make FTypeName<T,N> and FTypeName<T,0> miscellaneous
// Two  FTypeName<T, N> variables
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> add3(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  FTypeName<T, N> c(a.val() + b.val());
  c.setDepend(a, b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] + b[ i ];
  return c;
}
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> add3(const FTypeName<mpreal, N>& a, const FTypeName<mpreal, N>& b)
{
  Op<mpreal>::mpreal_add(TEMP_RESULT, a.val(), b.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  c.setDepend(a, b);  // c is dependent on a and b, btw a and b's dimensions are the same and
                      // greater than 0 and assign this to c
  for (unsigned int i = 0; i < N; ++i)
  {
    Op<mpreal>::mpreal_add(c[ i ], a[ i ], b[ i ]);
  }
  return c;
}
// Function add3 reloading
// Two FTypeName<T, 0> variables
template <typename T>
INLINE2 FTypeName<T, 0> add3(const FTypeName<T, 0>& a, const FTypeName<T, 0>& b)
{
  FTypeName<T, 0> c(a.val() + b.val());
  c.setDepend(a, b);  // c is dependent on a and b, btw a and b's dimensions are the same and
                      // greater than 0 and assign this to c
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] + b[ i ];
  return c;
}

INLINE2 FTypeName<mpreal, 0> add3(const FTypeName<mpreal, 0>& a, const FTypeName<mpreal, 0>& b)
{
  Op<mpreal>::mpreal_add(TEMP_RESULT, a.val(), b.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  c.setDepend(a, b);  // c is dependent on a and b, btw a and b's dimensions are the same and
                      // greater than 0 and assign this to c
  for (unsigned int i = 0; i < c.size(); ++i)
  {
    Op<mpreal>::mpreal_add(c[ i ], a[ i ], b[ i ]);
  }
  return c;
}

// operator+ reloading
// two FTypeName<T, N> variables
// Don't need reloading function for FTypeName<T, 0>.
// From the template parameter, we can see that the FTypeName<T, N> won't be miscellaneous.
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> operator+(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  switch ((a.depend() ? 1 : 0) | (b.depend() ? 2 : 0))
  {
    case 0:
      return FTypeName<T, N>(a.val() + b.val());
    case 1:
      return add2(a, b.val());  // related to 2 templates, a has gradient, b doesn't
    case 2:
      return add1(a.val(), b);  // related to 2 templates, b has gradient, a doesn't
  }
  return add3(a, b);  // case 3 :  related to 2 templates, when a and b all have gradients
}

//-----------------------------------------------------sub1, sub2, sub3, Operator -
//--------------------------------------------------------------
// one U variable and One FTypeName<T,N> variable
// function sub1 for  FTypeName<T, N>    stack
template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T, N> sub1(const U& a, const FTypeName<T, N>& b)
{
  FTypeName<T, N> c(a - b.val());
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = Op<T>::myNeg(b[ i ]);
  return c;
}
template <class U, unsigned int N>
INLINE2 FTypeName<mpreal, N> sub1(const U& a, const FTypeName<mpreal, N>& b)
{
  Op<mpreal>::mpreal_sub(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_neg(c[ i ], b[ i ]);
  return c;
}
// function reloading sub1       heap
// When variable is FTypeName<T, 0>, this is more concrete
template <typename T, class U>
INLINE2 FTypeName<T, 0> sub1(const U& a, const FTypeName<T, 0>& b)
{
  FTypeName<T, 0> c(a - b.val());
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = Op<T>::myNeg(b[ i ]);
  return c;
}
template <class U>
INLINE2 FTypeName<mpreal, 0> sub1(const U& a, const FTypeName<mpreal, 0>& b)
{
  Op<mpreal>::mpreal_sub(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_neg(c[ i ], b[ i ]);
  return c;
}
// One FTypeName<T,N> variable and one U variable
// function sub2 for  FTypeName<T, N>    stack
template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T, N> sub2(const FTypeName<T, N>& a, const U& b)
{
  FTypeName<T, N> c(a.val() - b);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ];
  return c;
}
template <class U, unsigned int N>
INLINE2 FTypeName<mpreal, N> sub2(const FTypeName<mpreal, N>& a, const U& b)
{
  Op<mpreal>::mpreal_sub(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ];
  return c;
}
// function reloading  sub2     heap
// When variable is FTypeName<T, 0>, this is more concrete
template <typename T, class U>
INLINE2 FTypeName<T, 0> sub2(const FTypeName<T, 0>& a, const U& b)
{
  FTypeName<T, 0> c(a.val() - b);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ];
  return c;
}

template <class U>
INLINE2 FTypeName<mpreal, 0> sub2(const FTypeName<mpreal, 0>& a, const U& b)
{
  Op<mpreal>::mpreal_sub(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ];
  return c;
}

// Operator - reloading
// One U type variable and One FTypeName<T, N> variable
// general function     Don't need reloading function for FTypeName<T, 0>.
// when calling sub1, according to the type of a and b, it calls different sub1.
template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> operator-(const U& a, const FTypeName<T, N>& b)
{
  return sub1(a, b);
}
// One FTypeName<T, N> variable and One U type variable
// general function     Don't need reloading function for FTypeName<T, 0>.
// when calling sub2, according to the type of a and b, it calls different sub2.
template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> operator-(const FTypeName<T, N>& a, const U& b)
{
  return sub2(a, b);
}

// sub3      According to the parameters of the template, the FTypeName<T,N> won't be miscellaneous
// Two  FTypeName<T, N> variables
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> sub3(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  FTypeName<T, N> c(a.val() - b.val());
  c.setDepend(a, b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] - b[ i ];
  return c;
}
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> sub3(const FTypeName<mpreal, N>& a, const FTypeName<mpreal, N>& b)
{
  Op<mpreal>::mpreal_sub(TEMP_RESULT, a.val(), b.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  c.setDepend(a, b);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_sub(c[ i ], a[ i ], b[ i ]);
  return c;
}
// Function sub3 reloading
// Two FTypeName<T, 0> variables
template <typename T>
INLINE2 FTypeName<T, 0> sub3(const FTypeName<T, 0>& a, const FTypeName<T, 0>& b)
{
  FTypeName<T, 0> c(a.val() - b.val());
  c.setDepend(a, b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] - b[ i ];
  return c;
}

INLINE2 FTypeName<mpreal, 0> sub3(const FTypeName<mpreal, 0>& a, const FTypeName<mpreal, 0>& b)
{
  Op<mpreal>::mpreal_sub(TEMP_RESULT, a.val(), b.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  c.setDepend(a, b);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_sub(c[ i ], a[ i ], b[ i ]);
  return c;
}
// Operator - reloading
// two FTypeName<T, N> variables
// Don't need reloading function for FTypeName<T, 0>.
// From the template parameter, we can see that the FTypeName<T, N> won't be miscellaneous.
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> operator-(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  switch ((a.depend() ? 1 : 0) | (b.depend() ? 2 : 0))
  {
    case 0:
      return FTypeName<T, N>(a.val() - b.val());
    case 1:
      return sub2(a, b.val());  // related to 2 templates, a has gradient, b doesn't
    case 2:
      return sub1(a.val(), b);  // related to 2 templates, b has gradient, a doesn't
  }
  return sub3(a,
              b);  // case 3 :  related to 2 templates, when a and b all have gradients, implicitly
}

//-----------------------------------------------------mul1, mul2, mul3, Operator *
//--------------------------------------------------------------
// mul1
template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T, N> mul1(const U& a, const FTypeName<T, N>& b)
{
  FTypeName<T, N> c(a * b.val());
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = b[ i ] * a;
  return c;
}
// reloaded for mpreal
template <class U, unsigned int N>
INLINE2 FTypeName<mpreal, N> mul1(const U& a, const FTypeName<mpreal, N>& b)
{
  Op<mpreal>::mpreal_mul(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], b[ i ], a);
  return c;
}

template <typename T, class U>
INLINE2 FTypeName<T, 0> mul1(const U& a, const FTypeName<T, 0>& b)
{
  FTypeName<T, 0> c(a * b.val());
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = b[ i ] * a;
  return c;
}

// reloaded for mpreal
template <class U>
INLINE2 FTypeName<mpreal, 0> mul1(const U& a, const FTypeName<mpreal, 0>& b)
{
  Op<mpreal>::mpreal_mul(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], b[ i ], a);
  return c;
}

// mul2
template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T, N> mul2(const FTypeName<T, N>& a, const U& b)
{
  FTypeName<T, N> c(a.val() * b);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * b;
  return c;
}
// reloaded for mpreal
template <class U, unsigned int N>
INLINE2 FTypeName<mpreal, N> mul2(const FTypeName<mpreal, N>& a, const U& b)
{
  Op<mpreal>::mpreal_mul(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], b);
  return c;
}

template <typename T, class U>
INLINE2 FTypeName<T, 0> mul2(const FTypeName<T, 0>& a, const U& b)
{
  FTypeName<T, 0> c(a.val() * b);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * b;
  return c;
}
// reloaded for mpreal
template <class U>
INLINE2 FTypeName<mpreal, 0> mul2(const FTypeName<mpreal, 0>& a, const U& b)
{
  Op<mpreal>::mpreal_mul(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], b);
  return c;
}

// operator *
template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> operator*(const U& a, const FTypeName<T, N>& b)
{
  return mul1(a, b);
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> operator*(const FTypeName<T, N>& a, const U& b)
{
  return mul2(a, b);
}

// mul3
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> mul3(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  const T& aval(a.val());
  const T& bval(b.val());
  FTypeName<T, N> c(aval * bval);
  c.setDepend(a, b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * bval + b[ i ] * aval;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> mul3(const FTypeName<mpreal, N>& a, const FTypeName<mpreal, N>& b)
{
  const mpreal& aval(a.val());
  const mpreal& bval(b.val());
  Op<mpreal>::mpreal_mul(TEMP_RESULT, aval, bval);
  FTypeName<mpreal, N> c(TEMP_RESULT);
  c.setDepend(a, b);
  for (unsigned int i = 0; i < N; ++i)
  {
    Op<mpreal>::mpreal_mul(TEMP_RESULT, a[ i ], bval);
    Op<mpreal>::mpreal_mul(TEMP_RESULT1, b[ i ], aval);
    Op<mpreal>::mpreal_add(c[ i ], TEMP_RESULT, TEMP_RESULT1);
  }
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> mul3(const FTypeName<T, 0>& a, const FTypeName<T, 0>& b)
{
  const T& aval(a.val());
  const T& bval(b.val());
  FTypeName<T, 0> c(aval * bval);
  c.setDepend(a, b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * bval + b[ i ] * aval;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> mul3(const FTypeName<mpreal, 0>& a, const FTypeName<mpreal, 0>& b)
{
  const mpreal& aval(a.val());
  const mpreal& bval(b.val());
  Op<mpreal>::mpreal_mul(TEMP_RESULT, aval, bval);
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  c.setDepend(a, b);
  for (unsigned int i = 0; i < c.size(); ++i)
  {
    Op<mpreal>::mpreal_mul(TEMP_RESULT, a[ i ], bval);
    Op<mpreal>::mpreal_mul(TEMP_RESULT1, b[ i ], aval);
    Op<mpreal>::mpreal_add(c[ i ], TEMP_RESULT, TEMP_RESULT1);
  }
  return c;
}

// operator*
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> operator*(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  switch ((a.depend() ? 1 : 0) | (b.depend() ? 2 : 0))
  {
    case 0:
      return FTypeName<T, N>(a.val() * b.val());
    case 1:
      return mul2(a, b.val());
    case 2:
      return mul1(a.val(), b);
  }
  return mul3(a, b);
}

//----------------------------------------------------div1, div2, div3, Operator /
//-------------------------------------------------------------------
// div1
template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T, N> div1(const U& a, const FTypeName<T, N>& b)
{
  FTypeName<T, N> c(a / b.val());
  if (!b.depend())
    return c;
  T tmp(Op<T>::myNeg(c.val() / b.val()));
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = tmp * b[ i ];
  return c;
}
// reloaded for mpreal
template <class U, unsigned int N>
INLINE2 FTypeName<mpreal, N> div1(const U& a, const FTypeName<mpreal, N>& b)
{
  Op<mpreal>::mpreal_div(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  Op<mpreal>::mpreal_div(TEMP_RESULT, c.val(), b.val());
  Op<mpreal>::mpreal_neg(TEMP_RESULT1, TEMP_RESULT);
  mpreal tmp(TEMP_RESULT1);
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], tmp, b[ i ]);
  return c;
}

template <typename T, class U>
INLINE2 FTypeName<T, 0> div1(const U& a, const FTypeName<T, 0>& b)
{
  FTypeName<T, 0> c(a / b.val());
  if (!b.depend())
    return c;
  T tmp(Op<T>::myNeg(c.val() / b.val()));
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = tmp * b[ i ];
  return c;
}
// reloaded for mpreal
template <class U>
INLINE2 FTypeName<mpreal, 0> div1(const U& a, const FTypeName<mpreal, 0>& b)
{
  Op<mpreal>::mpreal_div(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  Op<mpreal>::mpreal_div(TEMP_RESULT, c.val(), b.val());
  Op<mpreal>::mpreal_neg(TEMP_RESULT1, TEMP_RESULT);
  mpreal tmp(TEMP_RESULT1);
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], tmp, b[ i ]);
  return c;
}

// div2
template <typename T, class U, unsigned int N>
INLINE2 FTypeName<T, N> div2(const FTypeName<T, N>& a, const U& b)
{
  FTypeName<T, N> c(a.val() / b);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = (a[ i ]) / b;
  return c;
}
// reloaded for mpreal
template <class U, unsigned int N>
INLINE2 FTypeName<mpreal, N> div2(const FTypeName<mpreal, N>& a, const U& b)
{
  Op<mpreal>::mpreal_div(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_div(c[ i ], a[ i ], b);
  return c;
}

template <typename T, class U>
INLINE2 FTypeName<T, 0> div2(const FTypeName<T, 0>& a, const U& b)
{
  FTypeName<T, 0> c(a.val() / b);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = (a[ i ]) / b;
  return c;
}
// reloaded for mpreal
template <class U>
INLINE2 FTypeName<mpreal, 0> div2(const FTypeName<mpreal, 0>& a, const U& b)
{
  Op<mpreal>::mpreal_div(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_div(c[ i ], a[ i ], b);
  return c;
}

// operator/
template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> operator/(const U& a, const FTypeName<T, N>& b)
{
  return div1(a, b);
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> operator/(const FTypeName<T, N>& a, const U& b)
{
  return div2(a, b);
}

// div3
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> div3(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  const T& bval(b.val());
  FTypeName<T, N> c(a.val() / bval);
  c.setDepend(a, b);
  const T& cval(c.val());
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = (a[ i ] - cval * b[ i ]) / bval;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> div3(const FTypeName<mpreal, N>& a, const FTypeName<mpreal, N>& b)
{
  const mpreal& bval(b.val());
  Op<mpreal>::mpreal_div(TEMP_RESULT, a.val(), bval);
  FTypeName<mpreal, N> c(TEMP_RESULT);
  c.setDepend(a, b);
  const mpreal& cval(c.val());
  for (unsigned int i = 0; i < N; ++i)
  {
    Op<mpreal>::mpreal_mul(TEMP_RESULT, cval, b[ i ]);
    Op<mpreal>::mpreal_sub(TEMP_RESULT1, a[ i ], TEMP_RESULT);
    Op<mpreal>::mpreal_div(c[ i ], TEMP_RESULT1, bval);
  }
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> div3(const FTypeName<T, 0>& a, const FTypeName<T, 0>& b)
{
  const T& bval(b.val());
  FTypeName<T, 0> c(a.val() / bval);
  c.setDepend(a, b);
  const T& cval(c.val());
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = (a[ i ] - cval * b[ i ]) / bval;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> div3(const FTypeName<mpreal, 0>& a, const FTypeName<mpreal, 0>& b)
{
  const mpreal& bval(b.val());
  Op<mpreal>::mpreal_div(TEMP_RESULT, a.val(), bval);
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  c.setDepend(a, b);
  const mpreal& cval(c.val());
  for (unsigned int i = 0; i < c.size(); ++i)
  {
    Op<mpreal>::mpreal_mul(TEMP_RESULT, cval, b[ i ]);
    Op<mpreal>::mpreal_sub(TEMP_RESULT1, a[ i ], TEMP_RESULT);
    Op<mpreal>::mpreal_div(c[ i ], TEMP_RESULT1, bval);
  }
  return c;
}

// operator /
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> operator/(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  switch ((a.depend() ? 1 : 0) | (b.depend() ? 2 : 0))
  {
    case 0:
      return FTypeName<T, N>(a.val() / b.val());
    case 1:
      return div2(a, b.val());
    case 2:
      return div1(a.val(), b);
  }
  return div3(a, b);
}

//-------------------------------------------------------pow1, pow2, pow3, pow
//------------------------------------------------------------------
// pow1
template <typename T, typename U, unsigned int N>
INLINE2 FTypeName<T, N> pow1(const U& a, const FTypeName<T, N>& b)
{
  FTypeName<T, N> c(Op<T>::myPow(a, b.val()));
  if (!b.depend())
    return c;
  T tmp(c.val() * Op<T>::myLog(a));
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = tmp * b[ i ];
  return c;
}
// reloaded for mpreal
template <typename U, unsigned int N>
INLINE2 FTypeName<mpreal, N> pow1(const U& a, const FTypeName<mpreal, N>& b)
{
  Op<mpreal>::mpreal_pow(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  Op<mpreal>::mpreal_log(TEMP_RESULT, a);
  Op<mpreal>::mpreal_mul(TEMP_RESULT1, c.val(), TEMP_RESULT);
  mpreal tmp(TEMP_RESULT1);
  c.setDepend(b);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], tmp, b[ i ]);
  return c;
}

template <typename T, typename U>
INLINE2 FTypeName<T, 0> pow1(const U& a, const FTypeName<T, 0>& b)
{
  FTypeName<T, 0> c(Op<T>::myPow(a, b.val()));
  if (!b.depend())
    return c;
  T tmp(c.val() * Op<T>::myLog(a));
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = tmp * b[ i ];
  return c;
}
// reloaded for mpreal
template <typename U>
INLINE2 FTypeName<mpreal, 0> pow1(const U& a, const FTypeName<mpreal, 0>& b)
{
  Op<mpreal>::mpreal_pow(TEMP_RESULT, a, b.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!b.depend())
    return c;
  Op<mpreal>::mpreal_log(TEMP_RESULT, a);
  Op<mpreal>::mpreal_mul(TEMP_RESULT1, c.val(), TEMP_RESULT);
  mpreal tmp(TEMP_RESULT1);
  c.setDepend(b);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], tmp, b[ i ]);
  return c;
}
// pow2
template <typename T, typename U, unsigned int N>
INLINE2 FTypeName<T, N> pow2(const FTypeName<T, N>& a, const U& b)
{
  FTypeName<T, N> c(Op<T>::myPow(a.val(), b));
  if (!a.depend())
    return c;
  T tmp(b * Op<T>::myPow(a.val(), b - Op<T>::myOne()));
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = tmp * a[ i ];
  return c;
}
// reloaded for mpreal
template <typename U, unsigned int N>
INLINE2 FTypeName<mpreal, N> pow2(const FTypeName<mpreal, N>& a, const U& b)
{
  Op<mpreal>::mpreal_pow(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sub(TEMP_RESULT, b, Op<mpreal>::myOne());
  Op<mpreal>::mpreal_pow(TEMP_RESULT1, a.val(), TEMP_RESULT);
  Op<mpreal>::mpreal_mul(TEMP_RESULT, b, TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], tmp, a[ i ]);
  return c;
}

template <typename T, typename U>
INLINE2 FTypeName<T, 0> pow2(const FTypeName<T, 0>& a, const U& b)
{
  FTypeName<T, 0> c(Op<T>::myPow(a.val(), b));
  if (!a.depend())
    return c;
  T tmp(b * Op<T>::myPow(a.val(), b - Op<T>::myOne()));
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = tmp * a[ i ];
  return c;
}
// reloaded for mpreal
template <typename U>
INLINE2 FTypeName<mpreal, 0> pow2(const FTypeName<mpreal, 0>& a, const U& b)
{
  Op<mpreal>::mpreal_pow(TEMP_RESULT, a.val(), b);
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sub(TEMP_RESULT, b, Op<mpreal>::myOne());
  Op<mpreal>::mpreal_pow(TEMP_RESULT1, a.val(), TEMP_RESULT);
  Op<mpreal>::mpreal_mul(TEMP_RESULT, b, TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], tmp, a[ i ]);
  return c;
}

// pow
template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> pow(const U& a, const FTypeName<T, N>& b)
{
  return pow1(a, b);
}

template <typename T, unsigned int N, typename U>
INLINE2 FTypeName<T, N> pow(const FTypeName<T, N>& a, const U& b)
{
  return pow2(a, b);
}

// pow3
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> pow3(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  FTypeName<T, N> c(Op<T>::myPow(a.val(), b.val()));
  T tmp(b.val() * Op<T>::myPow(a.val(), b.val() - Op<T>::myOne())),
      tmp1(c.val() * Op<T>::myLog(a.val()));
  c.setDepend(a, b);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = tmp * a[ i ] + tmp1 * b[ i ];
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> pow3(const FTypeName<mpreal, N>& a, const FTypeName<mpreal, N>& b)
{
  Op<mpreal>::mpreal_pow(TEMP_RESULT, a.val(), b.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  Op<mpreal>::mpreal_sub(TEMP_RESULT, b.val(), Op<mpreal>::myOne());
  Op<mpreal>::mpreal_pow(TEMP_RESULT1, a.val(), TEMP_RESULT);
  Op<mpreal>::mpreal_mul(TEMP_RESULT, b.val(), TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  Op<mpreal>::mpreal_log(TEMP_RESULT, a.val());
  Op<mpreal>::mpreal_mul(TEMP_RESULT1, c.val(), TEMP_RESULT);
  mpreal tmp1(TEMP_RESULT1);
  c.setDepend(a, b);
  for (unsigned int i = 0; i < N; ++i)
  {
    Op<mpreal>::mpreal_mul(TEMP_RESULT, tmp, a[ i ]);
    Op<mpreal>::mpreal_mul(TEMP_RESULT1, tmp1, b[ i ]);
    Op<mpreal>::mpreal_add(c[ i ], TEMP_RESULT, TEMP_RESULT1);
  }
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> pow3(const FTypeName<T, 0>& a, const FTypeName<T, 0>& b)
{
  FTypeName<T, 0> c(Op<T>::myPow(a.val(), b.val()));
  T tmp(b.val() * Op<T>::myPow(a.val(), b.val() - Op<T>::myOne())),
      tmp1(c.val() * Op<T>::myLog(a.val()));
  c.setDepend(a, b);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = tmp * a[ i ] + tmp1 * b[ i ];
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> pow3(const FTypeName<mpreal, 0>& a, const FTypeName<mpreal, 0>& b)
{
  Op<mpreal>::mpreal_pow(TEMP_RESULT, a.val(), b.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  Op<mpreal>::mpreal_sub(TEMP_RESULT, b.val(), Op<mpreal>::myOne());
  Op<mpreal>::mpreal_pow(TEMP_RESULT1, a.val(), TEMP_RESULT);
  Op<mpreal>::mpreal_mul(TEMP_RESULT, b.val(), TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  Op<mpreal>::mpreal_log(TEMP_RESULT, a.val());
  Op<mpreal>::mpreal_mul(TEMP_RESULT1, c.val(), TEMP_RESULT);
  mpreal tmp1(TEMP_RESULT1);
  c.setDepend(a, b);
  for (unsigned int i = 0; i < c.size(); ++i)
  {
    Op<mpreal>::mpreal_mul(TEMP_RESULT, tmp, a[ i ]);
    Op<mpreal>::mpreal_mul(TEMP_RESULT1, tmp1, b[ i ]);
    Op<mpreal>::mpreal_add(c[ i ], TEMP_RESULT, TEMP_RESULT1);
  }
  return c;
}

// pow
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> pow(const FTypeName<T, N>& a, const FTypeName<T, N>& b)
{
  switch ((a.depend() ? 1 : 0) | (b.depend() ? 2 : 0))
  {
    case 0:
      return FTypeName<T, N>(Op<T>::myPow(a.val(), b.val()));
    case 1:
      return pow2(a, b.val());
    case 2:
      return pow1(a.val(), b);
  }
  return pow3(a, b);
}
// reloaded pow for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> pow(const FTypeName<mpreal, N>& a, const FTypeName<mpreal, N>& b)
{
  switch ((a.depend() ? 1 : 0) | (b.depend() ? 2 : 0))
  {
    case 0:
      Op<mpreal>::mpreal_pow(TEMP_RESULT, a.val(), b.val());
      return FTypeName<mpreal, N>(TEMP_RESULT);
    case 1:
      return pow2(a, b.val());
    case 2:
      return pow1(a.val(), b);
  }
  return pow3(a, b);
}
//---------------------------------------------------------- Unary Operators
//-------------------------------------------------------------------------
//+
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> operator+(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(a.val());
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ];
  return c;
}
template <typename T>
INLINE2 FTypeName<T, 0> operator+(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(a.val());
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ];
  return c;
}

//-
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> operator-(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::myNeg(a.val()));
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = Op<T>::myNeg(a[ i ]);
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> operator-(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_neg(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_neg(c[ i ], a[ i ]);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> operator-(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::myNeg(a.val()));
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = Op<T>::myNeg(a[ i ]);
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> operator-(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_neg(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_neg(c[ i ], a[ i ]);
  return c;
}

// sqr
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> sqr(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::mySqr(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myTwo() * a.val());
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> sqr(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_sqr(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_mul(TEMP_RESULT, Op<mpreal>::myTwo(), a.val());
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> sqr(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::mySqr(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myTwo() * a.val());
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> sqr(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_sqr(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_mul(TEMP_RESULT, Op<mpreal>::myTwo(), a.val());
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

// exp
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> exp(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::myExp(a.val()));
  if (!a.depend())
    return c;
  c.setDepend(a);
  const T& cval(c.val());
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * cval;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> exp(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_exp(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  const mpreal& cval(c.val());
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], cval);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> exp(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::myExp(a.val()));
  if (!a.depend())
    return c;
  c.setDepend(a);
  const T& cval(c.val());
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * cval;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> exp(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_exp(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  const mpreal& cval(c.val());
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], cval);
  return c;
}

// log
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> log(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::myLog(a.val()));
  if (!a.depend())
    return c;
  c.setDepend(a);
  const T& aval(a.val());
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] / aval;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> log(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_log(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  const mpreal& aval(a.val());
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_div(c[ i ], a[ i ], aval);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> log(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::myLog(a.val()));
  if (!a.depend())
    return c;
  c.setDepend(a);
  const T& aval(a.val());
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] / aval;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> log(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_log(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  c.setDepend(a);
  const mpreal& aval(a.val());
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_div(c[ i ], a[ i ], aval);
  return c;
}

// sqrt
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> sqrt(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::mySqrt(a.val()));
  if (!a.depend())
    return c;
  T tmp(c.val() * Op<T>::myTwo());
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] / tmp;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> sqrt(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_sqrt(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_mul(TEMP_RESULT, c.val(), Op<mpreal>::myTwo());
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_div(c[ i ], a[ i ], tmp);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> sqrt(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::mySqrt(a.val()));
  if (!a.depend())
    return c;
  T tmp(c.val() * Op<T>::myTwo());
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] / tmp;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> sqrt(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_sqrt(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_mul(TEMP_RESULT, c.val(), Op<mpreal>::myTwo());
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_div(c[ i ], a[ i ], tmp);
  return c;
}

// sin
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> sin(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::mySin(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myCos(a.val()));
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> sin(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_sin(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_cos(TEMP_RESULT, a.val());
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> sin(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::mySin(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myCos(a.val()));
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> sin(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_sin(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_cos(TEMP_RESULT, a.val());
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

// cos
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> cos(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::myCos(a.val()));
  if (!a.depend())
    return c;
  T tmp(-Op<T>::mySin(a.val()));
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> cos(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_cos(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sin(TEMP_RESULT, a.val());
  Op<mpreal>::mpreal_neg(TEMP_RESULT1, TEMP_RESULT);
  mpreal tmp(TEMP_RESULT1);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> cos(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::myCos(a.val()));
  if (!a.depend())
    return c;
  T tmp(-Op<T>::mySin(a.val()));
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> cos(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_cos(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sin(TEMP_RESULT, a.val());
  Op<mpreal>::mpreal_neg(TEMP_RESULT1, TEMP_RESULT);
  mpreal tmp(TEMP_RESULT1);
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

// tan
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> tan(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::myTan(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myOne() + Op<T>::mySqr(c.val()));
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> tan(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_tan(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sqr(TEMP_RESULT, c.val());
  Op<mpreal>::mpreal_add(TEMP_RESULT1, Op<mpreal>::myOne(), TEMP_RESULT);
  mpreal tmp(TEMP_RESULT1);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> tan(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::myTan(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myOne() + Op<T>::mySqr(c.val()));
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> tan(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_tan(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sqr(TEMP_RESULT, c.val());
  Op<mpreal>::mpreal_add(TEMP_RESULT1, Op<mpreal>::myOne(), TEMP_RESULT);
  mpreal tmp(TEMP_RESULT1);
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

// asin
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> asin(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::myAsin(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myInv(Op<T>::mySqrt(Op<T>::myOne() - Op<T>::mySqr(a.val()))));
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> asin(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_asin(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sqr(TEMP_RESULT1, a.val());
  Op<mpreal>::mpreal_sub(TEMP_RESULT, Op<mpreal>::myOne(), TEMP_RESULT1);
  Op<mpreal>::mpreal_sqrt(TEMP_RESULT1, TEMP_RESULT);
  Op<mpreal>::mpreal_inv(TEMP_RESULT, TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> asin(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::myAsin(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myInv(Op<T>::mySqrt(Op<T>::myOne() - Op<T>::mySqr(a.val()))));
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> asin(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_asin(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sqr(TEMP_RESULT1, a.val());
  Op<mpreal>::mpreal_sub(TEMP_RESULT, Op<mpreal>::myOne(), TEMP_RESULT1);
  Op<mpreal>::mpreal_sqrt(TEMP_RESULT1, TEMP_RESULT);
  Op<mpreal>::mpreal_inv(TEMP_RESULT, TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  cout << "good!";
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

// acos
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> acos(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::myAcos(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myNeg(Op<T>::myInv(Op<T>::mySqrt(Op<T>::myOne() - Op<T>::mySqr(a.val())))));
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> acos(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_acos(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sqr(TEMP_RESULT, a.val());
  Op<mpreal>::mpreal_sub(TEMP_RESULT1, Op<mpreal>::myOne(), TEMP_RESULT);
  Op<mpreal>::mpreal_sqrt(TEMP_RESULT, TEMP_RESULT1);
  Op<mpreal>::mpreal_inv(TEMP_RESULT1, TEMP_RESULT);
  Op<mpreal>::mpreal_neg(TEMP_RESULT, TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> acos(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::myAcos(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myNeg(Op<T>::myInv(Op<T>::mySqrt(Op<T>::myOne() - Op<T>::mySqr(a.val())))));
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> acos(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_acos(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sqr(TEMP_RESULT, a.val());
  Op<mpreal>::mpreal_sub(TEMP_RESULT1, Op<mpreal>::myOne(), TEMP_RESULT);
  Op<mpreal>::mpreal_sqrt(TEMP_RESULT, TEMP_RESULT1);
  Op<mpreal>::mpreal_inv(TEMP_RESULT1, TEMP_RESULT);
  Op<mpreal>::mpreal_neg(TEMP_RESULT, TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

// atan
template <typename T, unsigned int N>
INLINE2 FTypeName<T, N> atan(const FTypeName<T, N>& a)
{
  FTypeName<T, N> c(Op<T>::myAtan(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myInv(Op<T>::myOne() + Op<T>::mySqr(a.val())));
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
template <unsigned int N>
INLINE2 FTypeName<mpreal, N> atan(const FTypeName<mpreal, N>& a)
{
  Op<mpreal>::mpreal_atan(TEMP_RESULT, a.val());
  FTypeName<mpreal, N> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sqr(TEMP_RESULT, a.val());
  Op<mpreal>::mpreal_add(TEMP_RESULT1, Op<mpreal>::myOne(), TEMP_RESULT);
  Op<mpreal>::mpreal_inv(TEMP_RESULT, TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < N; ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

template <typename T>
INLINE2 FTypeName<T, 0> atan(const FTypeName<T, 0>& a)
{
  FTypeName<T, 0> c(Op<T>::myAtan(a.val()));
  if (!a.depend())
    return c;
  T tmp(Op<T>::myInv(Op<T>::myOne() + Op<T>::mySqr(a.val())));
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    c[ i ]            = a[ i ] * tmp;
  return c;
}
// reloaded for mpreal
INLINE2 FTypeName<mpreal, 0> atan(const FTypeName<mpreal, 0>& a)
{
  Op<mpreal>::mpreal_atan(TEMP_RESULT, a.val());
  FTypeName<mpreal, 0> c(TEMP_RESULT);
  if (!a.depend())
    return c;
  Op<mpreal>::mpreal_sqr(TEMP_RESULT, a.val());
  Op<mpreal>::mpreal_add(TEMP_RESULT1, Op<mpreal>::myOne(), TEMP_RESULT);
  Op<mpreal>::mpreal_inv(TEMP_RESULT, TEMP_RESULT1);
  mpreal tmp(TEMP_RESULT);
  c.setDepend(a);
  for (unsigned int i = 0; i < c.size(); ++i)
    Op<mpreal>::mpreal_mul(c[ i ], a[ i ], tmp);
  return c;
}

template <typename U, unsigned int N>
struct Op<FTypeName<U, N>>
{
  typedef FTypeName<U, N> T;
  typedef FTypeName<U, N> Underlying;
  typedef typename Op<U>::Base Base;
  static Base myInteger(const int i) { return Base(i); }
  static Base                     myZero() { return myInteger(0); }
  static Base                     myOne() { return myInteger(1); }
  static Base                     myTwo() { return myInteger(2); }
  static Base                     myPI() { return Op<Base>::myPI(); }
  static T myPos(const T& x) { return +x; }
  static T myNeg(const T& x) { return -x; }
  template <typename V>
  static T& myCadd(T& x, const V& y)
  {
    return x += y;
  }
  template <typename V>
  static T& myCsub(T& x, const V& y)
  {
    return x -= y;
  }
  template <typename V>
  static T& myCmul(T& x, const V& y)
  {
    return x *= y;
  }
  template <typename V>
  static T& myCdiv(T& x, const V& y)
  {
    return x /= y;
  }
  static T myInv(const T& x) { return myOne() / x; }
  static T mySqr(const T& x) { return fadbad::sqr(x); }
  template <typename X, typename Y>
  static T myPow(const X& x, const Y& y)
  {
    return fadbad::pow(x, y);
  }
  static T mySqrt(const T& x) { return fadbad::sqrt(x); }
  static T myLog(const T& x) { return fadbad::log(x); }
  static T myExp(const T& x) { return fadbad::exp(x); }
  static T mySin(const T& x) { return fadbad::sin(x); }
  static T myCos(const T& x) { return fadbad::cos(x); }
  static T myTan(const T& x) { return fadbad::tan(x); }
  static T myAsin(const T& x) { return fadbad::asin(x); }
  static T myAcos(const T& x) { return fadbad::acos(x); }
  static T myAtan(const T& x) { return fadbad::atan(x); }
  static bool myEq(const T& x, const T& y) { return x == y; }
  static bool myNe(const T& x, const T& y) { return x != y; }
  static bool myLt(const T& x, const T& y) { return x < y; }
  static bool myLe(const T& x, const T& y) { return x <= y; }
  static bool myGt(const T& x, const T& y) { return x > y; }
  static bool myGe(const T& x, const T& y) { return x >= y; }
};

}  // namespace fadbad

#endif
