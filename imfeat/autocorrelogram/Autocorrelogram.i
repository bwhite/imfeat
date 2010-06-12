%module Autocorrelogram
%{
#include "Autocorrelogram.hpp"
%}
%include stl.i 
%template(DoubleVector) std::vector<double>;
%template(IntVector) std::vector<int>;
%include "Autocorrelogram.hpp"