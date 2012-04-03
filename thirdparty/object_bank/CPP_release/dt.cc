#include <math.h>
#include <sys/types.h>
#include "mymex.h"
#include "dt.h"
#include <iostream>

/*
 * Generalized distance transforms.
 * We use a simple nlog(n) divide and conquer algorithm instead of the
 * theoretically faster linear method, for no particular reason except
 * that this is a bit simpler and I wanted to test it out.
 *
 * The code is a bit convoluted because dt1d can operate either along
 * a row or column of an array.  
 */

static inline int square(int x) { return x*x; }

// dt helper function
void dt_helper(double *src, double *dst, double *ptr, int step, 
	       int s1, int s2, int d1, int d2, double a, double b) {
 if (d2 >= d1) {
   int d = (d1+d2) >> 1;
   int s = s1;
   for (int p = s1+1; p <= s2; p++)
     if (src[s*step] + a*square(d-s) + b*(d-s) > 
	 src[p*step] + a*square(d-p) + b*(d-p))
	s = p;
   dst[d*step] = src[s*step] + a*square(d-s) + b*(d-s);
   ptr[d*step] = s;
   dt_helper(src, dst, ptr, step, s1, s, d1, d-1, a, b);
   dt_helper(src, dst, ptr, step, s, s2, d+1, d2, a, b);
 }
}

// dt of 1d array
void dt1d(double *src, double *dst, double *ptr, int step, int n, 
	  double a, double b) {
  dt_helper(src, dst, ptr, step, 0, n-1, 0, n-1, a, b);
}

// matlab entry point
void dt(IN const mxArray * mxMatch, IN double ax, IN double bx, IN double ay, IN double by, OUT mxArray * & mxM) 
{
  const int *dims = mxGetDimensions(mxMatch);
  double *vals = (double *)mxGetPr(mxMatch);
    
  mxM = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
  double *M = (double *)mxGetPr(mxM);

  double *tmpM = (double *)mxCalloc(dims[0]*dims[1], sizeof(double));
  double *tmpIx = (double *)mxCalloc(dims[0]*dims[1], sizeof(double));
  double *tmpIy = (double *)mxCalloc(dims[0]*dims[1], sizeof(double));

  for (int x = 0; x < dims[1]; x++) dt1d(vals+x*dims[0], tmpM+x*dims[0], tmpIy+x*dims[0], 1, dims[0], ay, by);
  for (int y = 0; y < dims[0]; y++) dt1d(tmpM+y, M+y, tmpIx+y, dims[0], dims[1], ax, bx);

  delete[] tmpM;
  delete[] tmpIx;
  delete[] tmpIy;
}

