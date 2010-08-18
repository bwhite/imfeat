#ifndef SurfDetect_h
#define SurfDetect_h
#include "integral_image/IntegralImage.h"
#include "SurfPoint.h"
#include <list>
using namespace std;
class SurfDetect
{
public:
	SurfDetect(int height, int width, bool verbose=false, float featThresh=0.00010342091786758998f, int octaves=3, int intervals=4, int interpIters = 1, char * filename = 0);
	~SurfDetect();
	void compute(std::list<SurfPoint*> *points, IntegralImage *intim);
	void compute(std::list<SurfPoint*> *points, IntegralImage *intim, int k);
	static std::list<SurfPoint*> *allocPoints() {
		return new list<SurfPoint*>();
	}

	static void freePoints(std::list<SurfPoint*> *points) {
		freePoints(&points);
	}

	static void freePoints(std::list<SurfPoint*> **points) {
		for (list<SurfPoint*>::iterator pt = (*points)->begin(); pt != (*points)->end(); pt++) {
			delete *pt;
		}
		delete *points;
		*points=0;

	}
	int at(int x,int y) {
		return y*width+x;
	}

	float **getFeatureSpace();

	int getOctaves();

	int getIntervals();
	void zeroFeatureSpace();
	void zeroFeatureSpace(int i);
	void interpIter (int k, int i, int x, int y, int step, float *pos);
	void interpIter (int i, int x, int y, int step, float *pos);
	inline static void featureIter(IntegralImage *intim, int x, int y, int dc, int ds, int dl, int &dxx, int &dyy, int &dxy) {
		dxx=-(intim->getAreaNZ(x-ds,y-dl,x+ds,y+dl) << 1)// Center
		    +intim->getAreaNZ(x-ds-dc,y-dl,x-ds-1,y+dl) // Left
		    +intim->getAreaNZ(x+ds+1,y-dl,x+ds+dc,y+dl); // Right

		dyy=-(intim->getAreaNZ(x-dl,y-ds,x+dl,y+ds) << 1)// Center
		    +intim->getAreaNZ(x-dl,y-ds-dc,x+dl,y-ds-1)// Top
		    +intim->getAreaNZ(x-dl,y+ds+1,x+dl,y+ds+dc); // Bottom

		dxy=(intim->getAreaNZ(x-dc,y-dc,x-1,y-1) // TL
		     -intim->getAreaNZ(x+1,y-dc,x+dc,y-1) // TR
		     -intim->getAreaNZ(x-dc,y+1,x-1,y+dc) // BL
		     +intim->getAreaNZ(x+1,y+1,x+dc,y+dc)); // BR
	}
private:
	// Tunable settings
	int octaves;
	int intervals;
	float featThresh;

	const char* filename;
	bool verbose;
	int height;
	int width;
	int size;
	int interpIters;
	float **featureSpace;
	bool **featureSpaceSign;
	IntegralImage *intim;
	int *step;
	int *border;
	int *borderHeight;
	int *borderWidth;
	float **kernelScale;
	int **dc;
	int **ds;
	int **dl;
	int *intervalMap; // This is used in the "reuse hessian" optimization

	void computeFeatureSpace(int k);
	void nonMaximalSuppression(std::list<SurfPoint*> *points, int k);
	bool isPointValid(int x, int y, int scale);
	float getFeatureSpace(int k, int i, int x, int y);
	float getFeatureSpace(int i, int x, int y);
	bool getFeatureSpaceSign(int i, int x, int y);
};
#endif
