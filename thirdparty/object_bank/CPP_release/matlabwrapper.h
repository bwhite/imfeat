#include "mymex.h"
#include <string>

using namespace std;
mxArray * getMxNumericalArrayFrom1DDoublePtr(const double * ptr, int len); 
mxArray * getMxNumericalArrayFrom2DDoublePtr(const double ** ptr, const int * pDims); 
mxArray * getMxNumericalArrayFrom3DDoublePtr(double *** ptr, const int * pDims);
mxArray * getMxNumericalArrayFromInt(const int num);

// common functions
mxArray * resize(const mxArray *mxsrc, const double scale);
mxArray * padarray3D(mxArray * input, int * padsize, double val);

