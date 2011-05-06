#include "SurfDescribe.h"
#include <stdlib.h>
#include <cmath>
#include <cstdio>
SurfDescribe::SurfDescribe()
{
	// Build gLUT2p5
	gLUT2p5 = new float[169];
	ijMagLUT = new bool[169];
	int isqrjsqrsum;
	float invDenom2p5 = 1.0f/(-12.5f);
	float invDenom3p3 = 1.0f/(-21.78);
	int cnt = 0;
	for (int i = -6; i <= 6; i++)
		for (int j = -6; j <= 6; j++) {
			isqrjsqrsum = i*i+j*j;
			gLUT2p5[cnt]=exp(isqrjsqrsum*invDenom2p5);
			ijMagLUT[cnt]=isqrjsqrsum < 36;
			cnt++;
		}
	gLUT3p3 = new float[25];
	for (int i = 0; i < 25; i++) {
		gLUT3p3[i]=exp(isqrjsqrsum*invDenom3p3);
	}
}

SurfDescribe::~SurfDescribe()
{
	delete [] gLUT2p5;
	gLUT2p5 = 0;
	delete [] gLUT3p3;
	gLUT3p3 = 0;
	delete [] ijMagLUT;
	ijMagLUT = 0;
}

list<SurfPoint*> *SurfDescribe::compute(list<SurfPoint*> *points, IntegralImage *intim)
{
	this->intim = intim;
	for (list<SurfPoint*>::iterator pt = points->begin(); pt != points->end(); pt++) {
		computeOrientation(*pt);
		computeDescriptor(*pt);
	}
	return points;
}

void SurfDescribe::computeOrientation(SurfPoint *pt)
{
	// Make a 6s radius grid of sampling step s
	int x = pt->x;
	int y = pt->y;
	int s = pt->scale;
	int sT6 = s*6;
	int sT2 = s << 1;
	float *xd = new float[169];
	float *yd = new float[169];
	float *angle = new float[169];
	float gaussDenom = 1.0/(2*M_PI*s*s);
	float gauss;
	int cnt = 0;

	// Compute haar responses over a grid
	for (float i = -sT6; i <= sT6; i+=s)
		for (float j = -sT6; j <= sT6; j+=s) {
			if (ijMagLUT[cnt]) {
				gauss=gLUT2p5[cnt]*gaussDenom;
				xd[cnt] = gauss*getHaarX(x+j,y+i,sT2);
				yd[cnt] = gauss*getHaarY(x+j,y+i,sT2);
				angle[cnt] = atan2(yd[cnt],xd[cnt]);
			}
			cnt++;
		}

	// Compute the dominant orientation from the haar responses
	// TODO Compute the dominant K angles
	float angstep = .1*M_PI;
	int xSum,ySum,mag,maxMag = 0;
	float maxAng = 0.0f;
	float piover3 = M_PI/3.0f;
	float angH=-M_PI+piover3;

	// [-pi,pi-pi/3
	for (float angL=-M_PI; angL < M_PI-piover3; angL+= angstep) {
		angH+=angstep;
		xSum = 0;
		ySum = 0;

		for (int i = 0; i < 169; i++) {
			if (ijMagLUT[i] && angH>angle[i] && angle[i]>=angL) {
				xSum += xd[i];
				ySum += yd[i];
			}
		}

		// If the response magnitude is greater than our previous best
		mag = xSum*xSum+ySum*ySum;
		if (mag > maxMag) {
			maxMag = mag;
			maxAng = atan2(ySum,xSum);
		}
	}

	// [pi-pi/3,pi)
	float angL=-M_PI;
	for (float angH=M_PI-piover3; angH < M_PI; angH+= angstep) {
		angL+=angstep;
		xSum = 0;
		ySum = 0;
		for (int i = 0; i < 169; i++) {
			if (ijMagLUT[i] && (angle[i]>=angH || angle[i]<angL)) {
				xSum += xd[i];
				ySum += yd[i];
			}
		}

		// If the response magnitude is greater than our previous best
		mag = xSum*xSum+ySum*ySum;
		if (mag > maxMag) {
			maxMag = mag;
			maxAng = atan2(ySum,xSum);
		}
	}
	pt->orientation = maxAng;

	delete [] xd;
	delete [] yd;
	delete [] angle;
}

// TODO The Haar wavelets can be combined into just 6 integral image operations instead of 8
int SurfDescribe::getHaarX(int x, int y, int sHalf)
{
	return intim->getAreaNZ(x,y-sHalf,x+sHalf,y+sHalf) - intim->getAreaNZ(x-sHalf,y-sHalf,x,y+sHalf);
}

int SurfDescribe::getHaarY(int x, int y, int sHalf)
{
	return intim->getAreaNZ(x-sHalf,y,x+sHalf,y+sHalf) - intim->getAreaNZ(x-sHalf,y-sHalf,x+sHalf,y);
}

void SurfDescribe::computeDescriptor(SurfPoint *pt)
{
	// Compute the feature region using a 20s window
	int s = pt->scale;
	int sT2 = s << 1;
	int sT10 = 10*s;
	int sT5 = 5*s;
	float oCos = cos(pt->orientation);
	float oSin = sin(pt->orientation);
	int warpX, warpY;
	float dx,dy,dxWarp, dyWarp;
	int cnt;
	/* float xSum, ySum, xSumAbs, ySumAbs; */
	float xSumNeg, ySumNeg, xSumAbsNeg, ySumAbsNeg;
	float xSumPos, ySumPos, xSumAbsPos, ySumAbsPos;
	float gaussDenom = 1.0/(2*M_PI*s*s);
	float gauss;

	// Allocate feature memory
	if (!pt->features64)
		pt->features64 = new float[64];
	if (!pt->features128)
		pt->features128 = new float[128];

	float *features64 = pt->features64;
	float *features128 = pt->features128;
	int featCnt = 0;
	int featSignCnt = 0;

	// 4x4 Cells
	for (float i = -sT10; i < sT10; i+=sT5) {// Y cell
		for (float j = -sT10; j < sT10; j+=sT5) { // X cell
			/*      xSum = 0;
			ySum = 0;
			xSumAbs = 0;
			ySumAbs = 0;*/

			xSumNeg = 0;
			ySumNeg = 0;
			xSumAbsNeg = 0;
			ySumAbsNeg = 0;
			xSumPos = 0;
			ySumPos = 0;
			xSumAbsPos = 0;
			ySumAbsPos = 0;
			cnt=0;

			// 5x5 Samples per cell
			for (float l = i; l < sT5+i; l+=s) { // Relative y coord
				for (float m = j; m < sT5+j; m+=s) { // Relative x coord
					gauss=gLUT3p3[cnt]*gaussDenom;
					// Compute location
					warpY = roundf(pt->y+oCos*l+oSin*m);
					warpX = roundf(pt->x+oCos*m-oSin*l);
					dy = gauss*getHaarY(warpX,warpY,sT2);
					dx = gauss*getHaarX(warpX,warpY,sT2);
					dyWarp = -oCos*dy+oSin*dx;
					dxWarp = oCos*dx+oSin*dy;

					//ySum += dyWarp;
					//ySumAbs += abs(dyWarp);

					if (dyWarp>=0) {
						ySumPos += dyWarp;
						ySumAbsPos += abs(dyWarp);
					} else {
						ySumNeg += dyWarp;
						ySumAbsNeg += abs(dyWarp);
					}

					//xSum += dxWarp;
					//xSumAbs += abs(dxWarp);
					if (dxWarp>=0) {
						xSumPos += dxWarp;
						xSumAbsPos += abs(dxWarp);
					} else {
						xSumNeg += dxWarp;
						xSumAbsNeg += abs(dxWarp);
					}
					cnt++;
				}
			}

			features64[featCnt] = ySumNeg+ySumPos;
			features64[featCnt+1] = ySumAbsNeg+ySumAbsPos;
			features64[featCnt+2] = xSumNeg+xSumPos;
			features64[featCnt+3] = xSumAbsNeg+xSumAbsPos;

			features128[featSignCnt] = ySumPos;
			features128[featSignCnt+1] = ySumAbsPos;
			features128[featSignCnt+2] = xSumPos;
			features128[featSignCnt+3] = xSumAbsPos;

			// Negatives
			features128[featSignCnt+4] = ySumNeg;
			features128[featSignCnt+5] = ySumAbsNeg;
			features128[featSignCnt+6] = xSumNeg;
			features128[featSignCnt+7] = xSumAbsNeg;

			featSignCnt+=8;
			featCnt+=4;
		}
	}
}
