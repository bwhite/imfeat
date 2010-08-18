#include "SurfDetect.h"
#include <memory.h>
#include <list>
#include <cstdio>
#include <cmath>
#include <stdlib.h>
#include <cassert>
using namespace std;

// Define step behavior
//#define SD_WHOLE_IMAGE
#ifdef SD_WHOLE_IMAGE
#define STEP(k) 1
#else
#define STEP(k) (1 << ((k)+1))
#endif

SurfDetect::SurfDetect(int height, int width, bool verbose, float featThresh, int octaves, int intervals, int interpIters, char* filename)
{
	// Tunable settings
	this->octaves = octaves;
	this->intervals = intervals;
	this->featThresh = featThresh;
	this->interpIters = interpIters;
	if (filename != 0)
		this->filename = filename;
	else
		this->filename="logs/rocket_out.txt";
	this->verbose=verbose;
	this->height = height;
	this->width = width;
	size = height*width;

#ifdef DEBUG
	this->verbose=1;
#endif

	// This is used in the "reuse hessian" optimization
	intervalMap = new int[intervals];
	int numReuses = log2(intervals);
	for (int i = 0; i < intervals; i++) {
		intervalMap[i] = -1; // Initialize them to -1
	}
	int curVal = 2;
	for (int i=0; i < numReuses; i++) {
		intervalMap[i]=curVal-1;
		curVal = curVal << 1;
	}

	featureSpace = new float*[intervals];
	featureSpaceSign = new bool*[intervals];
	for (int i = 0; i < intervals; i++) {
		featureSpace[i]=new float[size];
		featureSpaceSign[i]=new bool[size];
	}

	// Make LUTs
	step = new int[octaves];
	border = new int[octaves];
	borderHeight = new int[octaves];
	borderWidth = new int[octaves];
	kernelScale = new float*[octaves];
	dc = new int*[octaves];
	ds = new int*[octaves];
	dl = new int*[octaves];
	for (int k = 0; k < octaves; k++) {
		step[k] = STEP(k);
		int maxKernelSize = 3+(intervals)*3*(1 << (k+1));
		border[k] = (maxKernelSize)/2+1 ; // The biggest kernel size+1 so that we can allow for 1 pixel motion without a check
		borderHeight[k]=height-border[k];
		borderWidth[k]=width-border[k];

		kernelScale[k] = new float[intervals];
		dc[k] = new int[intervals];
		ds[k] = new int[intervals];
		dl[k] = new int[intervals];
		for (int i = 0; i < intervals; i++) {
			int kernelSize = 3+(i+1)*3*(1 << (k+1)); //   9,15,21,27,33 for k=0

			float kernelScaleTemp = 1.0/(((kernelSize*kernelSize)*255.0));
			kernelScale[k][i] = kernelScaleTemp*kernelScaleTemp;
			dc[k][i] = kernelSize/3;//    3,5,7,9,11
			ds[k][i] = kernelSize/6;//    1,2,3,4,5
			dl[k][i] = kernelSize/4;//    2,4,6,8,10
			if (verbose)
				printf("o=%d i=%d kernelSize=%d border=%d scale=%.10f\n",k,i,kernelSize,border[k],kernelScale[k][i]);
		}
	}
}

void SurfDetect::zeroFeatureSpace()
{
	unsigned char val=255;
	for (int i = 0; i < intervals; i++)
		memset(featureSpace[i],val,size*sizeof(float));
}

void SurfDetect::zeroFeatureSpace(int i)
{
	unsigned char val=255;
	memset(featureSpace[i],val,size*sizeof(float));
}

SurfDetect::~SurfDetect()
{
	for (int i = 0; i < intervals; i++) {
		delete [] featureSpace[i];
		delete [] featureSpaceSign[i];
	}
	delete [] featureSpace;
	featureSpace = 0;
	delete [] featureSpaceSign;
	featureSpaceSign = 0;

	delete [] step;
	for (int k = 0; k < octaves; k++) {
		delete [] kernelScale[k];
		delete [] dc[k];
		delete [] ds[k];
		delete [] dl[k];
	}

	delete [] border;
	border = 0;
	delete [] borderHeight;
	borderHeight=0;
	delete [] borderWidth;
	borderWidth=0;
	delete [] kernelScale;
	kernelScale=0;
	delete [] dc;
	dc=0;
	delete [] ds;
	ds=0;
	delete [] dl;
	dl=0;
	delete [] intervalMap;
	intervalMap=0;
}

int SurfDetect::getOctaves()
{
	return octaves;
}

int SurfDetect::getIntervals()
{
	return intervals;
}

float **SurfDetect::getFeatureSpace()
{
	return featureSpace;
}

void SurfDetect::compute(list<SurfPoint*> *points, IntegralImage *intim)
{
	this->intim=intim;
	if (interpIters > 1)
		zeroFeatureSpace();
	for (int k = 0; k < octaves; k++) {
		// Compute feature space
		computeFeatureSpace(k);

		// Non-maximal suppression, Scale space interpolation, and point retrieval
		nonMaximalSuppression(points,k);
	}
}

// Assumes that during iterations you start at k=0, if not true you have to call zeroFeatureSpace() first
void SurfDetect::compute(list<SurfPoint*> *points, IntegralImage *intim, int k)
{
	if (k == 0)
		zeroFeatureSpace();
	this->intim=intim;
	// Compute feature space
	computeFeatureSpace(k);

	// Non-maximal suppression, Scale space interpolation, and point retrieval
	nonMaximalSuppression(points,k);
}

void SurfDetect::computeFeatureSpace(int k)
{
	int dxx,dyy,dxy;
	int step = this->step[k];
	for (int i = 0; i < intervals; i++) {
		if (k > 0) {
			// Reuse some of the old masks
			// NOTE: Assumes k starts at 0 and is incremented
			if (intervalMap[i] >= 0) {
				float *tmpSpace = featureSpace[i];
				featureSpace[i]=featureSpace[intervalMap[i]];
				featureSpace[intervalMap[i]] = tmpSpace;

				bool *tmpSpaceSign = featureSpaceSign[i];
				featureSpaceSign[i]=featureSpaceSign[intervalMap[i]];
				featureSpaceSign[intervalMap[i]] = tmpSpaceSign;
				continue;
			} else {
				// Erase the old space, necessary if we are doing interpolation where we overwrite the NaN values
				if (interpIters > 1)
					zeroFeatureSpace(i);
			}
		}

		int border=this->border[k];
		int borderHeight=this->borderHeight[k];
		int borderWidth=this->borderWidth[k];
		float kernelScale=this->kernelScale[k][i];
		int dc=this->dc[k][i];
		int ds=this->ds[k][i];
		int dl=this->dl[k][i];

		float *space = featureSpace[i];
		bool *spaceSign = featureSpaceSign[i];
		// Required: border, borderHeight, step, dc, ds, dl, kernelScale
		for (int y = border; y < borderHeight; y+=step) {
			for (int x = border; x < borderWidth; x+=step) {
				featureIter(intim,x,y,dc,ds,dl,dxx,dyy,dxy);

				space[at(x,y)]=kernelScale*((float)dxx*(float)dyy-.81*(float)dxy*(float)dxy); // Try as an int
				if (dxx+dyy < 0)
					spaceSign[at(x,y)]=false; // Negative
				else
					spaceSign[at(x,y)]=true; // Positive
			}
		}
	}
}

void SurfDetect::nonMaximalSuppression(list<SurfPoint*> *points, int k)
{
	FILE* fp = NULL;
	int *interpCnt = NULL;
	if (verbose) {
		fp=fopen(filename,"w");
		interpCnt = new int[interpIters];
		memset(interpCnt,(unsigned char)0,sizeof(int)*interpIters);
	}
	float pos[3];
	// Threshold and store features into a list
	int step=this->step[k];
	for (int i = 1; i < intervals-1; i++) {
		int scale = roundf(.4*((1 << (k+1))*(i+1)+1));
		int border = this->border[k];
		int border12S = scale*12;
		if (border12S > border) {
			border = border + step*((border12S/step)+1); // NOTE: This is integer division
		}
		int borderHeight=height-border;
		int borderWidth=width-border;
		float *spaceL = featureSpace[i-1];
		float *space = featureSpace[i];
		float *spaceH = featureSpace[i+1];
		bool *signSpace = featureSpaceSign[i];
		for (int y = border+step; y < borderHeight-step; y+=step) {
			for (int x = border+step; x < borderWidth-step; x+=step) {
				float val=space[at(x,y)];
#ifdef DEBUG
				if (isnan(val)) {
					printf("ERROR: Val is NaN\n");
				}
#endif
				if ( val > featThresh && // Test threshold
				     val > space[at(x+step,y+step)] && // Test current slice
				     val > space[at(x+step,y)] &&
				     val > space[at(x,y+step)] &&
				     val > space[at(x-step,y-step)] &&
				     val > space[at(x-step,y)] &&
				     val > space[at(x,y-step)] &&
				     val > space[at(x-step,y+step)] &&
				     val > space[at(x+step,y-step)] &&

				     val > spaceL[at(x,y)] && // Test lower slice
				     val > spaceL[at(x+step,y+step)] &&
				     val > spaceL[at(x+step,y)] &&
				     val > spaceL[at(x,y+step)] &&
				     val > spaceL[at(x-step,y-step)] &&
				     val > spaceL[at(x-step,y)] &&
				     val > spaceL[at(x,y-step)] &&
				     val > spaceL[at(x-step,y+step)] &&
				     val > spaceL[at(x+step,y-step)] &&

				     val > spaceH[at(x,y)] && // Test higher slice
				     val > spaceH[at(x+step,y+step)] &&
				     val > spaceH[at(x+step,y)] &&
				     val > spaceH[at(x,y+step)] &&
				     val > spaceH[at(x-step,y-step)] &&
				     val > spaceH[at(x-step,y)] &&
				     val > spaceH[at(x,y-step)] &&
				     val > spaceH[at(x-step,y+step)] &&
				     val > spaceH[at(x+step,y-step)]) {
					if (interpIters) {
						// Complex interpolation (Any # iters)
						float cur_x=x;
						float cur_y=y;
						float cur_i=i;
						//printf("Interp:%d[%g,%g,%g]\n",-1,cur_x,cur_y,cur_i);
						for (int interpNum = 0; interpNum < interpIters; interpNum++) {
							if (interpIters==1) {
								interpIter ((int)cur_i, (int)cur_x, (int)cur_y, this->step[k], pos);
							} else {
								interpIter (k,(int)cur_i, (int)cur_x, (int)cur_y, this->step[k], pos);
							}

							cur_x=roundf(pos[0]+cur_x);
							cur_y=roundf(pos[1]+cur_y);
							cur_i=pos[2]+cur_i;
							if (isnan(cur_x) || isnan(cur_y) || isnan(cur_i))
								break;

							//printf("Interp:%d[%g,%g,%g]\n",interpNum,cur_x,cur_y,cur_i);
							if (fabs(pos[0]) < .5 && fabs(pos[1]) < .5 && fabs(pos[2]) < .5) {
								int cur_scale = roundf(.4*((1 << (k+1))*(cur_i+1)+1));
								// This ensures that description will be able to process this point

								if (!isPointValid(cur_x,cur_y,cur_scale))
									break;

								// The point has been properly localized
								if (verbose)
									fprintf(fp,"%d %d %d %d %f\n",k,i,x,y,val);

								// We can truncate cur_i because it is guaranteed to have less than .5
								points->push_front(new SurfPoint(cur_x,cur_y,cur_scale,getFeatureSpaceSign(cur_i, cur_x,cur_y),val));
								if (verbose)
									interpCnt[interpNum]++;
								break;
							}

							// Check to see if the point can be computed for detection purposes (doesn't ensure description has enough area)
							cur_i=roundf(cur_i);
							if (cur_i < 1 || cur_i >= intervals-1)
								break;
							int bound=border+step;
							if (cur_x-bound < 0 || cur_x+bound > width)
								break;
							if (cur_y-bound < 0 || cur_y+bound > height)
								break;
						}
					} else {
						if (verbose)
							fprintf(fp,"%d %d %d %d %f\n",k,i,x,y,val);
						points->push_front(new SurfPoint(x,y,scale,signSpace[at(x,y)],val));
					}
				}
			}
		}

	}
	if (verbose) {
		printf("InterpIters:");
		for (int i =0; i < interpIters; i++)
			printf("%d ",interpCnt[i]);
		printf("\n");
		delete [] interpCnt;
		interpCnt=0;
		fclose(fp);
	}
}

inline float SurfDetect::getFeatureSpace(int k, int i, int x, int y)
{
	float val=featureSpace[i][at(x,y)];
	if (isnan(val)) {
		// We need to make the value at this point
		int dxx,dyy,dxy;
		featureIter(intim,x,y,dc[k][i],ds[k][i],dl[k][i],dxx,dyy,dxy);

		val=kernelScale[k][i]*((float)dxx*(float)dyy-.81*(float)dxy*(float)dxy); // Try as an int
		featureSpace[i][at(x,y)]=val;
		if (dxx+dyy < 0)
			featureSpaceSign[i][at(x,y)]=false; // Negative
		else
			featureSpaceSign[i][at(x,y)]=true; // Positive
	}
	return val;
}

inline float SurfDetect::getFeatureSpace(int i, int x, int y)
{
	float val=featureSpace[i][at(x,y)];
#ifdef DEBUG
	if (isnan(val)) {
		printf("ERROR: Val is NaN in getFeatureSpace!\n");
	}
#endif
	return val;
}

inline bool SurfDetect::getFeatureSpaceSign(int i, int x, int y)
{
#ifdef DEBUG
	if (isnan(featureSpace[i][at(x,y)])) {
		printf("ERROR: Val is NaN in getFeatureSpaceSign!\n");
	}
#endif
	return featureSpaceSign[i][at(x,y)];
}

bool SurfDetect::isPointValid(int x, int y, int scale)
{
	int bound=17*scale+2; // 17*s+2
	if (x-bound < 0 || x+bound > width || y-bound < 0 || y+bound > height)
		return false;
	return true;
}

void SurfDetect::interpIter (int k, int i, int x, int y, int step, float *pos)
{
	float dx,dy,ds,dxx,dyy,dss,dxy,dxs,dys,val,val2,det,ndeti;
	val = getFeatureSpace(k,i,x,y);
	val2 = val*2.0f;
	dx=(getFeatureSpace(k,i,x+step,y) - getFeatureSpace(k,i,x-step,y))/2.0f;
	dy=(getFeatureSpace(k,i,x,y+step) - getFeatureSpace(k,i,x,y-step))/2.0f;
	ds=(getFeatureSpace(k,i+1,x,y) - getFeatureSpace(k,i-1,x,y))/2.0f;

	dxx=getFeatureSpace(k,i,x+step,y) + getFeatureSpace(k,i,x-step,y)-val2;
	dyy=getFeatureSpace(k,i,x,y+step) + getFeatureSpace(k,i,x,y-step)-val2;
	dss=getFeatureSpace(k,i+1,x,y) + getFeatureSpace(k,i-1,x,y)-val2;

	dxy=(getFeatureSpace(k,i,x+step,y+step) - getFeatureSpace(k,i,x-step,y+step) -
	     getFeatureSpace(k,i,x+step,y-step) + getFeatureSpace(k,i,x-step,y-step))/4.0f;

	dxs=(getFeatureSpace(k,i+1,x+step,y) - getFeatureSpace(k,i+1,x-step,y) -
	     getFeatureSpace(k,i-1,x+step,y) + getFeatureSpace(k,i-1,x-step,y))/4.0f;

	dys=(getFeatureSpace(k,i+1,x,y+step) - getFeatureSpace(k,i+1,x,y-step) -
	     getFeatureSpace(k,i-1,x,y+step) + getFeatureSpace(k,i-1,x,y-step))/4.0f;

	det=dxx*(dyy*dss-dys*dys) - dxy*(dxy*dss-dxs*dys) + dxs*(dxy*dys-dxs*dyy);
	ndeti = -1.0f/det;
	pos[0] = ndeti*(dx*(dyy*dss-dys*dys) + dy*(dxs*dys-dss*dxy) + ds*(dxy*dys-dyy*dxs));//x
	pos[1] = ndeti*(dx*(dys*dxs-dss*dxy) + dy*(dxx*dss-dxs*dxs) + ds*(dxs*dxy-dys*dxx));//y
	pos[2] = ndeti*(dx*(dxy*dys-dxs*dyy) + dy*(dxy*dxs-dxx*dys) + ds*(dxx*dyy-dxy*dxy));//s
}

void SurfDetect::interpIter (int i, int x, int y, int step, float *pos)
{
	float dx,dy,ds,dxx,dyy,dss,dxy,dxs,dys,val,val2,det,ndeti;
	val = getFeatureSpace(i,x,y);
	val2 = val*2.0f;
	dx=(getFeatureSpace(i,x+step,y) - getFeatureSpace(i,x-step,y))/2.0f;
	dy=(getFeatureSpace(i,x,y+step) - getFeatureSpace(i,x,y-step))/2.0f;
	ds=(getFeatureSpace(i+1,x,y) - getFeatureSpace(i-1,x,y))/2.0f;

	dxx=getFeatureSpace(i,x+step,y) + getFeatureSpace(i,x-step,y)-val2;
	dyy=getFeatureSpace(i,x,y+step) + getFeatureSpace(i,x,y-step)-val2;
	dss=getFeatureSpace(i+1,x,y) + getFeatureSpace(i-1,x,y)-val2;

	dxy=(getFeatureSpace(i,x+step,y+step) - getFeatureSpace(i,x-step,y+step) -
	     getFeatureSpace(i,x+step,y-step) + getFeatureSpace(i,x-step,y-step))/4.0f;

	dxs=(getFeatureSpace(i+1,x+step,y) - getFeatureSpace(i+1,x-step,y) -
	     getFeatureSpace(i-1,x+step,y) + getFeatureSpace(i-1,x-step,y))/4.0f;

	dys=(getFeatureSpace(i+1,x,y+step) - getFeatureSpace(i+1,x,y-step) -
	     getFeatureSpace(i-1,x,y+step) + getFeatureSpace(i-1,x,y-step))/4.0f;

	det=dxx*(dyy*dss-dys*dys) - dxy*(dxy*dss-dxs*dys) + dxs*(dxy*dys-dxs*dyy);
	ndeti = -1.0f/det;
	pos[0] = ndeti*(dx*(dyy*dss-dys*dys) + dy*(dxs*dys-dss*dxy) + ds*(dxy*dys-dyy*dxs));//x
	pos[1] = ndeti*(dx*(dys*dxs-dss*dxy) + dy*(dxx*dss-dxs*dxs) + ds*(dxs*dxy-dys*dxx));//y
	pos[2] = ndeti*(dx*(dxy*dys-dxs*dyy) + dy*(dxy*dxs-dxx*dys) + ds*(dxx*dyy-dxy*dxy));//s
}

