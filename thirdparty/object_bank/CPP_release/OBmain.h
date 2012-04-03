#include "global.h"
#include "detect.h" //_with_responsemap.h"
#include "MaxGetSpatialPyramid_single.h"
#include "matlabwrapper.cpp"
#include "LSVMModel.h"
#include <jpeglib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>

#include "MaxGetSpatialPyramid.h"

using namespace std; 


int getdir(string dir, vector<string> &files);
int loadModelFiles(string dir, vector<CModel>& models);
mxArray * loadImg(const char *filename);
int extractOBFeature(mxArray * input, vector<CModel> &modelList, bool isRespMapReturned, string fileName, vector<float> & outputFeature, vector<mxArray *> & responsemap, int numComponents, int numLevels);

