#include "mymex.h"
#include <string.h>
#include <stdio.h>
#include <cstdlib>

double mxGetScalar(const mxArray * mxarray)
{
  return ((double *)mxarray->data)[0];
}

void mxFree(mxArray ** mxarray)
{
  if (*mxarray) {
    delete *mxarray;
    *mxarray = NULL;
  } else {
	cout <<"Duplicated delete";
	cout <<"TODO(haosu): the pointer should be NULL";
  }
}

void mxFree(double * array)
{
  delete [] array;
}

mxArray *mxCreateNumericArray(const int ndim, const int * dims, const int classID, const int fake)
{
  mxArray * mxarray;
  mxarray = new mxArray();
  mxarray->NDim = ndim;
  if (0 == ndim) {
	cout <<"TODO(haosu): ndim=0! fix that";
  }
  mxarray->Dims = new int[ndim];
  memcpy(mxarray->Dims, dims, sizeof(int) * ndim);
  mxarray->_nelem = 1;
  for (int i = 0; i < ndim; i++) {
    mxarray->_nelem *= dims[i];
  }
  mxarray->classID = classID;
  if (classID == mxDOUBLE_CLASS) {
    mxarray->data = new char[mxarray->_nelem * sizeof(double)];
    memset(mxarray->data, 0, mxarray->_nelem * sizeof(double)); 
  } else {
      if (mxarray->data) {
      }
      mxarray->data = NULL;
  }
  return mxarray;
}

void * mxGetPr(const mxArray * mxarray)
{
    return mxarray->data;
}

void *mxCalloc(int nelem, int typesize)
{
    char * data;
    data = new char[nelem * typesize];
    memset(data, 0, nelem * typesize);
    return (void*)data;
}

int * mxGetDimensions(const mxArray * mxarray)
{
    return mxarray->Dims;
}

int mxGetNumberOfDimensions(const mxArray * mxarray)
{
    return mxarray->NDim;
}

int mxGetClassID(const mxArray * mxarray)
{
    return mxarray->classID;
}

void mexErrMsgTxt(const char * msg)
{
    cout << msg << endl;
    exit(-1);
}

mxArray_tag::~mxArray_tag() {
    if (this->data != NULL) {
        delete [] this->data;
        this->data = NULL;
    }
    if (this->Dims != NULL) {
        delete [] this->Dims;
        this->Dims = NULL;
    }
}

mxArray_tag * mxArray_tag::clone() {
    mxArray_tag * result;
    result = mxCreateNumericArray(this->NDim, this->Dims, mxDOUBLE_CLASS, mxREAL);

    memcpy(result->data, this->data, sizeof(double) * this->_nelem);
    return result;
}

mxArray_tag & mxArray_tag::operator+=(const mxArray_tag & rhs)
{
    if (classID == mxDOUBLE_CLASS && rhs.classID == mxDOUBLE_CLASS)
    {
        double * ptrLHS = (double*)this->data;
        double * ptrRHS = (double*)rhs.data;
        for (int i = 0; i < this->_nelem; i++)
        {
            *(ptrLHS++) += *(ptrRHS++);
        }
    }
    return *this;
}

const mxArray_tag mxArray_tag::operator+(const mxArray_tag & rhs)
{
    mxArray_tag * result;
    result = mxCreateNumericArray(this->NDim, this->Dims, mxDOUBLE_CLASS, mxREAL);
    memcpy(result->data, this->data, sizeof(double) * this->_nelem);
    (*result) += rhs;
    return (*result);
}

mxArray_tag & mxArray_tag::operator+=(const double & rhs)
{
    if (classID == mxDOUBLE_CLASS)
    {
        double * ptrLHS = (double*)this->data;
        for (int i = 0; i < this->_nelem; i++)
            *(ptrLHS++) += rhs;
    }
    return *this;
}

const mxArray_tag mxArray_tag::operator+(const double & rhs)
{
    mxArray_tag result = *this;
    result += rhs;
    return result;
}

mxArray_tag operator-(const mxArray_tag & op)
{
    mxArray_tag * res = NULL;
    if (op.classID == mxDOUBLE_CLASS)
    {
        res = mxCreateNumericArray(op.NDim, op.Dims, mxDOUBLE_CLASS, mxREAL);
        double * ptrOpL = (double*)res->data;
        double * ptrOpR = (double*)op.data;
        for (int i = 0; i < op._nelem; i++) *(ptrOpL++) = -*(ptrOpR++);
    }
    return (*res);
}

void mxArray_tag::negative() 
{
    if (this->classID == mxDOUBLE_CLASS)
    {
        double * ptr = (double*)this->data;
        for (int i = 0; i < this->_nelem; i++)
        {
            *ptr = - *ptr;
            ptr++;
        }
    }
}

double mxArray_tag::get(vector<int> &subscript, double & retval)
{ 
    if (subscript.size() != this->NDim)
        return -1;
    if (this->classID != mxDOUBLE_CLASS)
        return -2;
    unsigned int offset = 0;
    unsigned int block = 1;
    for (int j = 0; j < subscript.size(); j++)
    {
        offset += subscript[j] * block;
        block *= this->Dims[j];
    }
    retval = (((double*)this->data))[offset];
    return retval;
}

double mxArray_tag::get2D(int row, int col, double & retval)
{ 
    retval = (((double*)this->data))[col * Dims[0] + row];
    return retval;
}

void mxArray_tag::set2D(int row, int col, double val)
{
    (((double*)this->data))[col*Dims[0]+row] = val;
}

double mxArray_tag::get3D(int subidx1, int subidx2, int subidx3, double &retval)
{ 
    retval = (((double*)this->data))[subidx1 + subidx2 * Dims[0] + subidx3 * Dims[0] * Dims[1]];
    return retval;
}

void mxArray_tag::set3D(int subidx1, int subidx2, int subidx3, double val)
{
    (((double*)this->data))[subidx1 + subidx2 * Dims[0] + subidx3 * Dims[0] * Dims[1]] = val;
}

double * mxArray_tag::getPtr3D(int subidx1, int subidx2, int subidx3)
{
    return (((double*)this->data)) + subidx1 + subidx2 * Dims[0] + subidx3 * Dims[0] * Dims[1];
}

int size(const mxArray * mxarray, int k) 
{ 
    return mxarray->Dims[k]; 
}

int WriteToDisk3D(vector<mxArray *> & vecMatrix, string szFileName)
{
    int vecSize = vecMatrix.size();
    cout << "output: " << szFileName;
    FILE * fp = fopen(szFileName.c_str(), "w");
    if (fp == NULL) {
        cout << "Cannot create output file!";
    } else {
        fprintf(fp, "%d\n", vecSize);
        for (int i = 0; i < vecSize; i++)
        {
            if (vecMatrix[i]->NDim != 3)
                cout << "matrix dimension incorrect" << endl;
            int nrow = vecMatrix[i]->Dims[0], ncol = vecMatrix[i]->Dims[1];
            fprintf(fp, "%d %d\n", nrow, ncol);
            double val;
            for (int x = 0; x < ncol; x++)
            {
                for (int y = 0; y < nrow; y++)
                    fprintf(fp, "%lf ", vecMatrix[i]->get3D(y, x, 0, val));
                fprintf(fp, "\n");
            }
        }
        fclose(fp);
    }
    return 1;
}

