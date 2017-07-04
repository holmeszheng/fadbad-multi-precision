#include <iostream>
#include "tadiff.h"

#define ORDER 10

using namespace std;
using namespace fadbad;

template <typename U>
class TODE
{
 public:
  T<U> x;                  // Independent variables
  T<U> xp;                 // Dependent variables
  TODE() { xp = cos(x); }  // record DAG at construction
};
template <typename U, typename G>
void get_max_norm(TODE<U> &ode1, TODE<G> &ode2);
template <typename U>
void show_result(TODE<U> &ode)
{
  for (int i = 0; i < ORDER; i++)
  {
    ode.xp.eval(i);                                // Evaluate i'th Taylor coefficient
    ode.x[ i + 1 ] = ode.xp[ i ] / double(i + 1);  // Use dx/dt=ode(x).
  }
  // ode.x[0]...ode.x[10] now contains the Taylor-coefficients
  // of the solution of the ODE.

  // Print out the Taylor coefficients for the solution
  // of the ODE:
  for (int i = 0; i <= ORDER; i++)
  {
    cout << "x[" << i << "]=" << ode.x[ i ] << endl;
  }
}
int main()
{
  // ODE_double
  // variables initiation
  TODE<double> ode_double;  // Construct ODE:
  ode_double.x[ 0 ] = 1;    // Set point of expansion:

  // output
  int output_prec = 15;
  cout.precision(output_prec);
  cout << "-----------------------------------------------\n";
  cout << "Computed in double precision, \noutput in " << output_prec << " digits" << endl;
  show_result(ode_double);

  // Settings for mpreal
  int prec = 512;
  /*Set the default working precision for the mpreal data type, the default working precision is 53
   * bit which is the same as double*/
  mpfr_set_default_prec(prec);
  /*Using one of the 5 rounding mode parameter:
  MPFR_RNDN, MPFR_RNDZ, MPFR_RNDU, MPFR_RNDD, MPFR_RNDA
  to set the default rounding mode, the default rounding mode is MPFR_RNDN.*/
  mpfr_set_default_rounding_mode(MPFR_RNDN);

  // ODE_mpreal
  // variables initiation
  TODE<mpreal> ode_mpreal;  // Construct ODE:
  ode_mpreal.x[ 0 ] = 1;    // Set point of expansion:

  // output
  cout.precision(output_prec);
  cout << "-----------------------------------------------\n";
  cout << "Computed in MPFR precision " << prec << " digs" << endl;
  cout << "output in " << output_prec << " digits" << endl;
  show_result(ode_mpreal);

  // show max_norm
  int norm_output_prec = 5;
  cout.precision(norm_output_prec);
  cout << "-----------------------------------------------\n";
  cout << "max_norm between double and MPFR precision " << prec << "\n";
  get_max_norm(ode_double, ode_mpreal);

  return 0;
}

template <typename U, typename G>
void get_max_norm(TODE<U> &ode1, TODE<G> &ode2)
{
  mpreal max_norm = 0;
  for (int i = 0; i <= ORDER; i++)
  {
    mpreal temp = 0;
    temp        = fabs(ode1.x[ i ] - ode2.x[ i ]);
    if (max_norm < temp)
    {
      max_norm = temp;
    }
  }
  cout << "max norm:\t" << max_norm << endl;
}
