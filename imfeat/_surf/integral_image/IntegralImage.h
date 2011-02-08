#ifndef IntegralImage_h
#define IntegralImage_h
#ifdef DEBUG
#include <cstdio>
#include <cstdlib>
#endif
class IntegralImage
{
public:
	IntegralImage(char *image, int height, int width);
	IntegralImage(unsigned char *image, int height, int width);
	IntegralImage(int height, int width);
	void compute(unsigned char *img);
	int at(int x,int y) {
		return y*width+x;
	}
	int getHeight();
	int getWidth();
	int* getIntegralImage();
	// xa<=x<=xb, ya<=y<=yb
	inline int getArea(int xa,int ya,int xb, int yb) {
		int a=0,b=0,c=0,d=0;
		if (xa != 0 && ya != 0)
			a=intim[at(xa-1,ya-1)];
		if (ya != 0)
			b=intim[at(xb,ya-1)];
		c=intim[at(xb,yb)];
		if (xa != 0)
			d=intim[at(xa-1,yb)];
		return c+a-b-d;
	}

	// 0<xa<=x<=xb, 0<ya<=y<=yb
	inline int getAreaNZ(int xa,int ya,int xb, int yb) {
#ifdef DEBUG
		if (xa < 1 || ya < 1 || xb >= width || yb >= height) {
			char err[100];
			sprintf(err,"Intim: Access out of bounds (%d,%d) to (%d,%d) x=[1,%d] y=[1,%d]\n",xa,ya,xb,yb,getWidth(),getHeight());
			throw(err);
		}
#endif
		return intim[at(xb,yb)]+intim[at(xa-1,ya-1)]-intim[at(xb,ya-1)]-intim[at(xa-1,yb)];
	}

	~IntegralImage();

private:
	int *intim;
	int height;
	int width;
};
#endif
