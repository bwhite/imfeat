#include <math.h>
#include <assert.h>
#include <string.h>
#include "mymex.h"
#include "matlabwrapper.h"

/*
 * Fast image subsampling.
 * This is used to construct the feature pyramid.
 */

// struct used for caching interpolation values
struct alphainfo {
  int si, di;
  double alpha;
};

// copy src into dst using pre-computed interpolation values
void alphacopy(double *src, double *dst, struct alphainfo *ofs, int n) {
  struct alphainfo *end = ofs + n;
  while (ofs != end) {
    dst[ofs->di] += ofs->alpha * src[ofs->si];
    ofs++;
  }
}

// resize along each column
// result is transposed, so we can apply it twice for a complete resize
void resize1dtran(double *src, int sheight, double *dst, int dheight, 
		  int width, int chan) {
  double scale = (double)dheight/(double)sheight;
  double invscale = (double)sheight/(double)dheight;
  
  // we cache the interpolation values since they can be 
  // shared among different columns
  int len = (int)ceil(dheight*invscale) + 2*dheight;
  alphainfo ofs[len];
  int k = 0;
  for (int dy = 0; dy < dheight; dy++) {
    double fsy1 = dy * invscale;
    double fsy2 = fsy1 + invscale;
    int sy1 = (int)ceil(fsy1);
    int sy2 = (int)floor(fsy2);       

    if (sy1 - fsy1 > 1e-3) {
      assert(k < len);
      assert(sy1 >= 0);
      ofs[k].di = dy*width;
      ofs[k].si = sy1-1;
      ofs[k++].alpha = (sy1 - fsy1) * scale;
    }

    for (int sy = sy1; sy < sy2; sy++) {
      assert(k < len);
      assert(sy < sheight);
      ofs[k].di = dy*width;
      ofs[k].si = sy;
      ofs[k++].alpha = scale;
    }

    if (fsy2 - sy2 > 1e-3) {
      assert(k < len);
      assert(sy2 < sheight);
      ofs[k].di = dy*width;
      ofs[k].si = sy2;
      ofs[k++].alpha = (fsy2 - sy2) * scale;
    }
  }

  // resize each column of each color channel
  bzero(dst, chan*width*dheight*sizeof(double));
  for (int c = 0; c < chan; c++) {
    for (int x = 0; x < width; x++) {
      double *s = src + c*width*sheight + x*sheight;
      double *d = dst + c*width*dheight + x;
      alphacopy(s, d, ofs, k);
    }
  }
}

// Perform a basic 'pixel' enlarging resample.
void upSample_NN(double *src, int sheight, int swidth, double *dst, int dheight, int dwidth, int chan)
{
    double scaleWidth =  (double)dwidth / (double)swidth;
    double scaleHeight = (double)dheight / (double)sheight;

    for(int cy = 0; cy < dheight; cy++)
    {
        for(int cx = 0; cx < dwidth; cx++)
        {
            for (int channel = 0; channel < chan; channel++) {
                int pixel = cy + cx * dheight + channel * dheight * dwidth;
                int nearestMatch =  (int)(cy / scaleHeight) + 
                    (int)(cx / scaleWidth) * sheight + channel * sheight * swidth; 
                if (nearestMatch >= swidth * sheight * chan) {
                    printf("(int)(cy / scaleHeight)=%d, (int)(cx / scaleWidth)=%d, channel=%d\nsheight=%d, swidth=%d\n nearestMatch=%d, swidth*sheight*chan=%d", (int)(cy / scaleHeight), (int)(cx / scaleWidth), channel, sheight, swidth, nearestMatch, swidth * sheight * chan);
                }
                assert(nearestMatch < swidth * sheight * chan);
                dst[pixel] =  src[nearestMatch];
            }
        }
    }
}

// main function
// takes a double color image and a scaling factor
// returns resized image
mxArray *resize(const mxArray *mxsrc, const double scale) {
  double *src = (double *)mxGetPr(mxsrc);
  const int *sdims = mxGetDimensions(mxsrc);
  if (mxGetNumberOfDimensions(mxsrc) != 3 || 
      mxGetClassID(mxsrc) != mxDOUBLE_CLASS)
    mexErrMsgTxt("Invalid input");  

  int ddims[3];
  ddims[0] = (int)round(sdims[0]*scale);
  ddims[1] = (int)round(sdims[1]*scale);
  ddims[2] = sdims[2];
  mxArray *mxdst = mxCreateNumericArray(3, ddims, mxDOUBLE_CLASS, mxREAL);
  double *dst = (double *)mxGetPr(mxdst);

  if (scale > 1) {
      upSample_NN(src, sdims[0], sdims[1], dst, ddims[0], ddims[1], sdims[2]); 
  }
  else { 
      double *tmp = (double *)mxCalloc(ddims[0]*sdims[1]*sdims[2], sizeof(double));
      resize1dtran(src, sdims[0], tmp, ddims[0], sdims[1], sdims[2]);
      resize1dtran(tmp, sdims[1], dst, ddims[1], ddims[0], sdims[2]);
      mxFree(tmp);
  }

  return mxdst;
}
