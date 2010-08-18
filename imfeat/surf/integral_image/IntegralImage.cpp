#include "IntegralImage.h"

IntegralImage::IntegralImage(char *img, int height, int width)
{
	intim = new int[height*width];
	this->height = height;
	this->width = width;
	compute((unsigned char *)img);
}

IntegralImage::IntegralImage(unsigned char *img, int height, int width)
{
	intim = new int[height*width];
	this->height=height;
	this->width=width;
	compute(img);
}

IntegralImage::IntegralImage(int height, int width)
{
	intim = new int[height*width];
	this->height=height;
	this->width=width;
}

int IntegralImage::getHeight()
{
	return height;
}

int IntegralImage::getWidth()
{
	return width;
}

void IntegralImage::compute(unsigned char *img)
{
	// OPTIMIZATION: Do first row separate due to branching behavior
	for (int j=0; j <width; j++) {
		int i=0;
		// a b
		// c d
		int c=0;
		// C
		if (j != 0)
			c=intim[at(j-1,i)];
		intim[at(j,i)]=img[at(j,i)]+c;
	}

	for (int i=1; i <height; i++)
		for (int j=0; j <width; j++) {
			// a b
			// c d
			int a=0,b=0,c=0;
			// B
			b=intim[at(j,i-1)];
			// A and C
			if (j != 0) {
				a=intim[at(j-1,i-1)];
				c=intim[at(j-1,i)];
			}
			intim[at(j,i)]=img[at(j,i)]+c+b-a;
		}
}

int *IntegralImage::getIntegralImage()
{
	return intim;
}

IntegralImage::~IntegralImage()
{
	delete [] intim;
}
