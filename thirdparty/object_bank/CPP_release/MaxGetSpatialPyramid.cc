#include "MaxGetSpatialPyramid.h"
#include "MaxGetSpatialPyramid_single.h"

void MaxGetSpatialPyramid(float responseMap[], int nLevel, float feature[], int height, int width){
	int k = 0;
	double nGrid = 0;

	// Make sure boundaries are correct!!!
	for(int i=1; i<=nLevel; i++){
		// Number of grids in 1d
		nGrid = pow( (double) 2,i-1);
		MaxGetSpatialPyramid_single(responseMap, nGrid, feature, k, height, width);
		k = k + (int) pow( (double) 4 ,i-1);
	}/* end for */

}

