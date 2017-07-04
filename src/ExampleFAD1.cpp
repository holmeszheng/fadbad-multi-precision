#include <iostream>
#include "fadiff.h"

#define TERMS 2

using namespace std;
using namespace fadbad;

template <typename T, unsigned int N>
F<T, N> func(const F<T, N> *x_in, int n);
template <typename T, unsigned int N>
void show_result(F<T, N> &f_result, int n);
template <typename T, typename U, unsigned int N>
void show_norms(F<T, N> &f_result1, F<U, N> &f_result2, int n);
int main()
{
  // double type
  // variables initiation
  F<double, TERMS> f_double;           // Declare variables f
  F<double, TERMS> x_double[ TERMS ];  //  Declare 2 variables
  x_double[ 0 ] = 0.512;               // Initialize variable x
  x_double[ 1 ] = 2.141;               // Initialize variable y
  x_double[ 0 ].diff(0);               // Differentiate with respect to x (index 0 of 2)
  x_double[ 1 ].diff(1);               // Differentiate with respect to y (index 1 of 2)
  f_double = func(x_double, TERMS);    // Evaluate function and derivatives

  // output
  int output_prec = 15;
  cout.precision(output_prec);
  cout << "-----------------------------------------------\n";
  cout << "Computed in double precision, \noutput in " << output_prec << " digits" << endl;
  show_result(f_double, TERMS);

  // Settings for mpreal
  int prec = 128;
  /*Set the default working precision for the mpreal data type, the default working precision is 53
   * bit which is the same as double*/
  mpfr_set_default_prec(prec);

  // Stack-form template
  // variables initiation
  F<mpreal, TERMS> f_mpreal;           // Declare variables x,y,f
  F<mpreal, TERMS> x_mpreal[ TERMS ];  // Declare two variables
  x_mpreal[ 0 ] = 0.512;               // Initialize variable x
  x_mpreal[ 1 ] = 2.141;               // Initialize variable y
  x_mpreal[ 0 ].diff(0);               // Differentiate with respect to x (index 0 of 2)
  x_mpreal[ 1 ].diff(1);               // Differentiate with respect to y (index 1 of 2)
  f_mpreal = func(x_mpreal, TERMS);    // Evaluate function and derivatives

  // output
  cout << "-----------------------------------------------\n";
  cout << "Computed in MPFR precision " << prec << " digs" << endl;
  cout << "output in " << output_prec << " digits" << endl;
  show_result(f_mpreal, TERMS);

  // show_norms
  int norm_output_prec = 5;
  cout.precision(norm_output_prec);
  cout << "-----------------------------------------------\n";
  cout << "Errors between double and MPFR precision " << prec << "\n";
  show_norms(f_double, f_mpreal, TERMS);
  return 0;
}

template <typename T, unsigned int N>
F<T, N> func(const F<T, N> *x_in, int n)
{
  F<T, N> x_out;
  x_out = atan(x_in[ 0 ]) * x_in[ 1 ];
  return x_out;
}
template <typename T, unsigned int N>
void show_result(F<T, N> &f_result, int n)
{
  T  fval  = f_result.x();  // Value of function
  T *f_der = new T[ n ];
  for (int i = 0; i < n; i++)
  {
    f_der[ i ] = f_result.d(i);  // get Value for each derivative
  }

  // output
  cout << "f       = " << fval << endl;
  for (int i = 0; i < n; i++)
  {
    cout << "df/d" << i << "th = " << f_der[ i ] << endl;  // output each derivative
  }
  delete[] f_der;
}
template <typename T, typename U, unsigned int N>
void show_norms(F<T, N> &f_result1, F<U, N> &f_result2, int n)
{
  // max-norm;
  cout << "fval   : " << fabs(f_result1.x() - f_result2.x()) << endl;
  for (int i = 0; i < n; i++)
  {
    cout << "df/d" << i << "th: " << fabs(f_result1.d(i) - f_result2.d(i)) << endl;
  }
}
