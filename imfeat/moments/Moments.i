%module Moments
%{
#include "Moments.hpp"
%}
%include stl.i 
%template(DoubleVector) std::vector<double>;
%include "Moments.hpp"