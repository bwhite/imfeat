#include <list>
#include <map>
#include "SurfPoint.h"
#include "integral_image/IntegralImage.h"
using namespace std;
class SurfDescribe
{
public:
	SurfDescribe();
	~SurfDescribe();
	list<SurfPoint*> *compute(list<SurfPoint*> *points, IntegralImage *intim);
private:
	void computeOrientation(SurfPoint *pt);
	void computeDescriptor(SurfPoint *pt);
	int getHaarX(int x, int y, int sHalf);
	int getHaarY(int x, int y, int sHalf);
	IntegralImage *intim;
	float *angleLUT;//
	bool *ijMagLUT; // i*i+j*j < 36
	float *gLUT2p5;
	float *gLUT3p3;
};
