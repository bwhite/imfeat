#pragma once

#include "mymex.h"
#include "LSVMModel.h"

#define IN
#define OUT

using namespace std;

const double g_inf = 1e20;

/*void detect(IN const mxArray * input, IN const CModel &model, OUT vector<mxArray *> & vecResponsemap);*/

void detect_postfeatpyr(IN const vector<mxArray *> &feat, IN const vector<double> &scales, IN const CModel &model, OUT vector<mxArray *> & vecResponsemap,int numComponents);

int * getIntBinaryTuple(IN int s1, IN int s2);

void featpyramid(IN const mxArray * input, IN const int sbin, IN const int interval, OUT vector<mxArray*> &feat, OUT vector<double> &scale);

int WriteToDisk(vector<mxArray *> & vecMatrix, string szFileName);
