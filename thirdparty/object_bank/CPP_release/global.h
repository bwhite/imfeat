#include<stdio.h>
#include<string>
#include<iostream>
#include<sys/types.h>
#include<dirent.h>
#include<errno.h>
#include<vector>

using namespace std;

class Model {
	public:
		int sbins;
		int maxSize[2];
		int numcomponents;
		float interval;
		float rootfilter [10][10];
		vector <float> offsets;
		vector <int> rootIdx;
		vector <int> offsetIdx;
};
