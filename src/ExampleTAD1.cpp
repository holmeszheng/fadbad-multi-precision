#include <iostream>
#include "tadiff.h"

#define TERMS 2
#define ORDER 10

using namespace std;
using namespace fadbad;

template <typename U> T<U> func(const T<U> *x_in, int n);
template <typename U> void show_result(T<U> &f_result);
template <typename U, typename G>
void get_max_norm(T<U> &f_result1, T<G> &f_result2);

int main()
{
  // double type
  // variables initiation
  T<double> f_double;           // Declare variables f
  T<double> x_double[ TERMS ];  //  Declare 2 variables
  x_double[ 0 ][ 0 ] = 1.121;
  x_double[ 0 ][ 1 ] = 1.353;  // Taylor-expand wrt. x (dx/dx=1)
  x_double[ 0 ][ 2 ] = 5.21234;
  x_double[ 1 ][ 0 ] = 2.221;
  x_double[ 1 ][ 1 ] = 2.253;
  x_double[ 1 ][ 2 ] = -12.123;
  f_double           = func(x_double, TERMS);  // Evaluate function and record DAG
  f_double.eval(ORDER);                        // Taylor-expand f to degree ORDER
  // f[0]...f[ORDER] now contains the Taylor-coefficients.

  // output
  int output_prec = 15;
  cout.precision(output_prec);
  cout << "-----------------------------------------------\n";
  cout << "Computed in double precision, \noutput in " << output_prec << " digits" << endl;
  show_result(f_double);

  // Settings for mpreal
  int prec = 128;
  /*Set the default working precision for the mpreal data type, the default working precision is 53
   * bit which is the same as double*/
  mpfr_set_default_prec(prec);
  /*Using one of the 5 rounding mode parameter:
  MPFR_RNDN, MPFR_RNDZ, MPFR_RNDU, MPFR_RNDD, MPFR_RNDA
  to set the default rounding mode, the default rounding mode is MPFR_RNDN.*/
  mpfr_set_default_rounding_mode(MPFR_RNDN);

  // mpreal type
  // variables initiation
  T<mpreal> f_mpreal;           // Declare variables f
  T<mpreal> x_mpreal[ TERMS ];  //  Declare 2 variables
  x_mpreal[ 0 ][ 0 ] = 1.121;
  x_mpreal[ 0 ][ 1 ] = 1.353;  // Taylor-expand wrt. x (dx/dx=1)
  x_mpreal[ 0 ][ 2 ] = 5.21234;

  x_mpreal[ 1 ][ 0 ] = 2.221;
  x_mpreal[ 1 ][ 1 ] = 2.253;
  x_mpreal[ 1 ][ 2 ] = -12.123;
  f_mpreal           = func(x_mpreal, TERMS);  // Evaluate function and record DAG
  mpreal fval_mpreal;                          // Declare variables x,y,f
  fval_mpreal = f_mpreal[ 0 ];                 // Value of function
  f_mpreal.eval(ORDER);                        // Taylor-expand f to degree ORDER
  // f[0]...f[ORDER] now contains the Taylor-coefficients.

  // output
  cout.precision(output_prec);
  cout << "-----------------------------------------------\n";
  cout << "Computed in MPFR precision " << prec << " digs" << endl;
  cout << "output in " << output_prec << " digits" << endl;
  show_result(f_mpreal);

  // show max_norm
  int norm_output_prec = 5;
  cout.precision(norm_output_prec);
  cout << "-----------------------------------------------\n";
  cout << "max_norm between double and MPFR precision " << prec << "\n";
  get_max_norm(f_double, f_mpreal);

  return 0;
}
template <typename U>
T<U> func(const T<U> *x_in, int n)
{
  T<U> x_out;
  x_out = sin(x_in[ 0 ] + x_in[ 1 ] / 3.2 - cos(5.263));
  return x_out;
}
template <typename U>
void show_result(T<U> &f_result)
{
  U fval = f_result[ 0 ];
  cout << "f(x,y)=" << fval << endl;
  for (int i = 0; i <= ORDER; i++)
  {
    U c = f_result[ i ];  // The i'th taylor coefficient
    cout << "(1/k!)*(d^" << i << "f/dx^" << i << ")=" << c << endl;
  }
}
template <typename U, typename G>
void get_max_norm(T<U> &f_result1, T<G> &f_result2)
{
  // max-norm
  mpreal max_norm = 0;
  for (int i = 0; i <= ORDER; i++)
  {
    mpreal temp = 0;
    temp        = fabs(f_result1[ i ] - f_result2[ i ]);
    if (max_norm < temp)
    {
      max_norm = temp;
    }
  }
  cout << "max norm:\t" << max_norm << endl;
}
