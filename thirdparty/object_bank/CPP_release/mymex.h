#pragma once

#include <vector>
#include <string>
#include <iostream>

using namespace std;

const int mxDOUBLE_CLASS = 1;
const int mxREAL = 1;
typedef int mwSize;

typedef class mxArray_tag {
public:
  char * data;
  int NDim;
  int * Dims;
  int classID;
  int _nelem;

public:
  mxArray_tag() {
	data = NULL;
	Dims = NULL;
  }
  ~mxArray_tag();

public:
  mxArray_tag & operator+=(const mxArray_tag & rhs);
  mxArray_tag & operator+=(const double & rhs);
  const mxArray_tag operator+(const mxArray_tag & rhs);
  const mxArray_tag operator+(const double & rhs);
  friend mxArray_tag operator-(const mxArray_tag & op);

public:
  double get(vector<int> &subscript, double & retval);
  double get2D(int row, int col, double &retval);
  double get3D(int subidx1, int subidx2, int subidx3, double &retval);
  double * getPtr3D(int subidx1, int subidx2, int subidx3);
  void set(vector<int> subscript, double val);
  void set2D(int row, int col, double val);
  void set3D(int subidx1, int subidx2, int subidx3, double val);
  mxArray_tag * clone();
  void negative();
} mxArray;

double mxGetScalar(const mxArray * mxarray);
void mxFree(mxArray ** mxarray);
void mxFree(double * array);
mxArray * mxCreateNumericArray(const int ndim, const int * dims, const int type, const int fake);
void * mxGetPr(const mxArray * mxarray);
void * mxCalloc(int nelem, int typesize);
int * mxGetDimensions(const mxArray * mxarray);
int mxGetNumberOfDimensions(const mxArray * mxarray);
int mxGetClassID(const mxArray * mxarray);
void mexErrMsgTxt(const char * msg);
int size(const mxArray * mxarray, int k);
int WriteToDisk3D(vector<mxArray *> & vecMatrix, string szFileName);

