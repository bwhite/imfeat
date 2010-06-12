%module Histogram
%{
#include "Histogram.hpp"
%}
%include stl.i 
%template(IntVector) std::vector<int>;
%include "Histogram.hpp"