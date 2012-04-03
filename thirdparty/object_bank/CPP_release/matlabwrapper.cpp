#include "matlabwrapper.h"
#include <iostream>
#include <algorithm>
// #define max(a, b) (a)>(b)?(a):(b)
// #define min(a, b) (a)>(b)?(b):(a)

mxArray * getMxNumericalArrayFrom1DDoublePtr(const double * ptr, int len)
{
  mxArray *mxarray = mxCreateNumericArray(1, &len, mxDOUBLE_CLASS, mxREAL);
  double *parray = (double *)mxGetPr(mxarray);
  for (int i = 0; i < len; i++)
    *(parray++) = *(ptr++);
  return mxarray;
}

mxArray * getMxNumericalArrayFrom2DDoublePtr(const double ** ptr, const int * pDims)
{
  mxArray *mxarray = mxCreateNumericArray(2, pDims, mxDOUBLE_CLASS, mxREAL);
  double *parray = (double *)mxGetPr(mxarray);
  for (int x = 0; x < pDims[1]; x++)
    for (int y = 0; y < pDims[0]; y++)
      *(parray++) = ptr[y][x]; 
  return mxarray;
}

/*mxArray * getMxNumericalArrayFromIplImage(const IplImage * ptr)
{
  int dims[3];
  dims[0] = ptr->height;
  dims[1] = ptr->width;
  dims[2] = 3;
  int swap_c[] = {2, 1, 0};
  mxArray *mxarray = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
  double *parray = (double *)mxGetPr(mxarray);
  unsigned char * pPixel;
  for (int c = 0; c < 3; c++)
    for (int x = 0; x < ptr->width; x++)
      for (int y = 0; y < ptr->height; y++)
      {
        pPixel = (unsigned char *)(ptr->imageData + y * ptr->widthStep + ptr->nChannels * x);
        if (ptr->nChannels == 1)
          *(parray++) = (*pPixel) / 255.0;
        else
          *(parray++) = pPixel[swap_c[c]] / 255.0;
      }
  return mxarray;
}
*/

mxArray * getMxNumericalArrayFrom3DDoublePtr(double *** ptr, const int * pDims)
{
  mxArray *mxarray = mxCreateNumericArray(3, pDims, mxDOUBLE_CLASS, mxREAL);
  double *parray = (double *)mxGetPr(mxarray);
  for (int c = 0; c < pDims[2]; c++)
    for (int x = 0; x < pDims[1]; x++)
      for (int y = 0; y < pDims[0]; y++) {
        *(parray++) = ptr[y][x][c]; 
	  }
  return mxarray;
}

/*mxArray * getMxNumericalArrayFromIplImage(const IplImage * ptr)
{
  int dims[3];
  dims[0] = ptr->height;
  dims[1] = ptr->width;
  dims[2] = 3;
  int swap_c[] = {2, 1, 0};
  mxArray *mxarray = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
  double *parray = (double *)mxGetPr(mxarray);
  unsigned char * pPixel;
  for (int c = 0; c < 3; c++)
    for (int x = 0; x < ptr->width; x++)
      for (int y = 0; y < ptr->height; y++)
      {
        pPixel = (unsigned char *)(ptr->imageData + y * ptr->widthStep + ptr->nChannels * x);
        if (ptr->nChannels == 1)
          *(parray++) = (*pPixel) / 255.0;
        else
          *(parray++) = pPixel[swap_c[c]] / 255.0;
      }
  return mxarray;
}*/

mxArray * getMxNumericalArrayFromInt(const int num)
{
  int dims[1] = {1};
  mxArray *mxarray = mxCreateNumericArray(1, dims, mxDOUBLE_CLASS, mxREAL);
  double * data = (double*)mxGetPr(mxarray);
  (*data) = num;
  return mxarray;
}

mxArray * padarray3D(mxArray * mxsrc, int * padsize, double val)
{
  if (mxGetNumberOfDimensions(mxsrc) != 3 || 
      mxGetClassID(mxsrc) != mxDOUBLE_CLASS)
    mexErrMsgTxt("Invalid input");  

  const int *sdims = mxGetDimensions(mxsrc);

  int ddims[3];
  ddims[0] = sdims[0] + 2 * padsize[0];
  ddims[1] = sdims[1] + 2 * padsize[1];
  ddims[2] = sdims[2] + 2 * padsize[2];
 
  mxArray * mxdst = mxCreateNumericArray(3, ddims, mxDOUBLE_CLASS, mxREAL);
  double *dst = (double *)mxGetPr(mxdst);
  for (int i = 0; i < ddims[0] * ddims[1] * ddims[2]; i++)
    dst[i] = val;  
  
  for (int s2 = padsize[2]; s2 < ddims[2] - padsize[2]; s2++) 
  {
	int src_s2 = s2 - padsize[2];	
    for (int s1 = padsize[1]; s1 < ddims[1] - padsize[1]; s1++)
	{
	  int src_s1 = s1 - padsize[1]; 
	  for (int s0 = padsize[0]; s0 < ddims[0] - padsize[0]; s0++)
	  {
		int src_s0 = s0 - padsize[0];
		mxsrc->get3D(src_s0, src_s1, src_s2, val);
		mxdst->set3D(s0, s1, s2, val);
	  }
	}
  }
  return mxdst;
}

