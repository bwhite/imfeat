#include "MaxGetSpatialPyramid.h"
#include "MaxGetSpatialPyramid_single.h"
#include <float.h>
#define max(a, b) (((a) > (b)) ? (a) : (b))
using namespace std;

void MaxGetSpatialPyramid_single(float responseMap[], int nGrid , float feature[], int start, int height, int width){
    int i = 0;
    int j = 0;
    std::vector<int> x_start(nGrid);
    std::vector<int> y_start(nGrid);
    std::vector<int> x_end(nGrid);
    std::vector<int> y_end(nGrid);
    double maxRespGrid;
    int gridW = max(floor( (double) (width - (width%nGrid))/ nGrid ), 1);
    int gridH = max(floor( (double) (height - (height%nGrid))/ nGrid ), 1);

    // check boundaries
    for(i=0; i<nGrid; i++){
        x_start[i] = gridW*i; 
        x_end[i] = gridW - 1 + gridW*(i); 
        y_start[i] = gridH*i; 
        y_end[i] = gridH - 1 + gridH*(i);
    } /* end for */

    for(i=0; i<nGrid; i++){
        for(j=0; j<nGrid; j++){
            maxRespGrid = getMaxCurrGrid(responseMap, x_start[j], x_end[j], y_start[i], y_end[i], width);
            feature[(int)nGrid *i + j + start] = maxRespGrid;
        } /* end for */
    } /* end for */

}

double getMaxCurrGrid(float responseMap[], int xStart, int xEnd, int yStart, int yEnd, int width){
    int i,j,k,yCount;
    k = 0;
    yCount = 0;
    double maxVal = -DBL_MAX;
    // check boundaries
    for(i = yStart; i<= yEnd; i++){
        for(j = xStart; j<= xEnd; j++){
	    double curr = responseMap[j + (yCount+yStart)*width];
            if (maxVal < curr) maxVal = curr;
        }/* end for */
        yCount++;
    }/* end for */
    return maxVal;
}
