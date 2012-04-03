#include "mymex.h"
#include "detect.h"
#include <math.h>
#include "dt.h"
#include "matlabwrapper.h"
#include "myfeatures.h"
#include "fconv.h"
#include <stdio.h>
#include <iostream>
#include <algorithm>

// #define min(x, y) ((x)>(y)?y:x)

// return a int array with two elements
int * getIntBinaryTuple(int s1, int s2)
{
  int * tuple;
  tuple = new int[2];
  tuple[0] = s1;
  tuple[1] = s2;
  return tuple;
}

// return a int array with three elements
int * getIntTripleTuple(int s1, int s2, int s3)
{
  int * tuple;
  tuple = new int[3];
  tuple[0] = s1;
  tuple[1] = s2;
  tuple[2] = s3;
  return tuple;
}

/*
 * Compute feature pyramid.
 *
 * INPUT:
 * sbin is the size of a HOG cell - it should be even.
 * interval is the number of scales in an octave of the pyramid.
 *
 * OUTPUT:
 * feat[i] is the i-th level of the feature pyramid.
 * scale[i] is the scaling factor used for the i-th level.
 * feat[i+interval] is computed at exactly half the resolution of feat[i].
 * first octave halucinates higher resolution data.
 *
 * Attention:
 * the valid indice of feat and scale start from 1, following the Matlab convention
 */

void featpyramid(IN const mxArray * input, IN const int sbin, IN const int interval, OUT vector<mxArray*> &feat, OUT vector<double> &scale)
{
  double sc = pow(2.0, 1.0 / interval);
  int h = size(input, 0), w = size(input, 1);
  int max_scale = 1+floor(log(min(h, w) / (5.0 * sbin)) / log(sc)); 
  feat.resize(max_scale + interval, NULL); 
  scale.resize(max_scale + interval, NULL);

  mxArray * scaled;
  mxArray * scaled_new;
  mxArray * mxsbin;
  for (int i = 0; i < interval; i+=5) 
  {
	scaled = resize(input, pow(1.0 / sc, i));
	// "first" 2x interval
	mxsbin = getMxNumericalArrayFromInt(sbin/2);
	
	feat[i] = features(scaled, mxsbin);
	mxFree(&mxsbin);
	scale[i] = 2.0 / pow(sc, i);
	// "second" 2x interval
	mxsbin = getMxNumericalArrayFromInt(sbin);
	feat[i+interval] = features(scaled, mxsbin);

	mxFree(&mxsbin);
	scale[i+interval] = 1.0 / pow(sc, i);
	// remaining intervals
	for (int j = i + interval; j <= max_scale; j += interval)
	{
	  
	  scaled_new = resize(scaled, 0.5);
	  mxFree(&scaled);
	  scaled = scaled_new;
	  mxsbin = getMxNumericalArrayFromInt(sbin);
	  feat[j+interval] = features(scaled, mxsbin);
	  mxFree(&mxsbin);
	  scale[j+interval] = 0.5 * scale[j];
	}
	mxFree(&scaled);
  }

}

/*
 * THIS IS THE NEW VERSION THAT DOES THE DETECTION AFTER THE FEATPYRAMID HAS BEEN CALLED
 */
void detect_postfeatpyr(IN const vector<mxArray *> &feat, IN const vector<double> &scales, IN const CModel &model, OUT vector<mxArray *> & vecResponsemap, int numComponents)
{
 
  int i;
  vector<CRootFilter_w> rootfilters;
  rootfilters.resize(model.rootfilters.size()+1);

  for(i = 1; i <= model.rootfilters.size(); i++)
	rootfilters[i] = model.rootfilters[i-1]->w; 
  
  vector<int> ridx, oidx;
  ridx.resize(numComponents+1); //model.numcomponents+1
  oidx.resize(numComponents+1);//model.numcomponents+1
 
  						  
  for (int c = 1; c <= numComponents; c++)
  {
	ridx[c] = model.components[c-1]->rootindex;
	oidx[c] = model.components[c-1]->offsetindex;
  }
  
  int padx = ceil((double)model.maxsize[1]/2+1);
  int pady = ceil((double)model.maxsize[0]/2+1);

  mxArray * featr;
  vector<mxArray *> rootmatch;
  mxArray * score;
  
  for (int level = model.interval; level < feat.size()-1; level+=5)
  {

	if (size(feat[level], 0) + 2*pady < model.maxsize[0] || 
		size(feat[level], 1) + 2*padx < model.maxsize[1])
	  continue;

	featr = padarray3D(feat[level], getIntTripleTuple(pady, padx, 0), 0);
	
	fconv(featr, rootfilters, 1, rootfilters.size()-1, rootmatch);


	for (int c = 1; c <= numComponents; c++){
	
	  score = rootmatch[ridx[c]]->clone();
	  (*score) += model.offsets[oidx[c]-1]->w;

	  vecResponsemap.push_back(score);
	
	} 
	
	mxFree(&featr);

	for (int i = 1; i < rootmatch.size(); i++) 
	  mxFree(&rootmatch[i]);
	
	
  } 

}
