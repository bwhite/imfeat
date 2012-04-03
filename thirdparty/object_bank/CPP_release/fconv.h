#pragma once

#include "LSVMModel.h"
#include "mymex.h"
using namespace std;

void fconv(const mxArray * mxA, const vector<CRootFilter_w> & cellB, int start, int end, vector<mxArray *> &cellC);
