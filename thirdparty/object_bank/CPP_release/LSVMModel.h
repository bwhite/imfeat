#pragma once

#include "parserUtil.h"
#include <iostream>
#include <cctype>

using namespace std;
typedef vector<double> Tlowerbounds;

struct CRootFilter_w 
{
    double *** w;
    vector<int> vecDims;
};

class CRootFilter
{
public:
    int size[2];
    int blocklabel;
    CRootFilter_w w;
public:
    void ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType);
};

typedef CRootFilter_w CPartFilter_w;

class CPartFilter
{
public:
    int fake;
    int partner;
    int blocklabel;
    CPartFilter_w w;
public:
    void ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType);
};

class COffset
{
public:
    double w;
    int blocklabel;
public:
    void ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType);
};

class CPart
{
public:
    int partindex;
    int defindex;
public:
    void ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType);
};

class CComponent
{
public:
    int rootindex;
    int offsetindex;
    vector<CPart*> parts;
    int dim;
    int numblocks;
    vector<double> x1;
    vector<double> y1;
    vector<double> x2;
    vector<double> y2;

public:
    void ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType);
    int Parse_parts(MyStringBuffer * sb, char * & token, int & iTokenType);
};

class CDef
{
public:
    int anchor[2];
    double * w;
    int blocklabel;
public:
    void ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType);
};

class CModel
{
public:
    string cls_id;
    int sbin;
    vector<int> blocksizes;
    vector<double> regmult;
    vector<double> learnmult;
    int interval;
    int numcomponents;
    int numblocks;
    int maxsize[2];
    int minsize[2];
    double thresh;
    double negfrompos_overlap;
    vector<CRootFilter*> rootfilters;
    vector<CPartFilter*> partfilters;
    vector<COffset*> offsets;
    vector<Tlowerbounds*> lowerbounds;
    vector<CDef*> defs;
    vector<CComponent*> components;

public:
    CModel(string szModel)
    {
        int retval = ParseFromText(szModel);
        if (retval < 0)
        {
            cout << "Error parsing model file!!!";
            cout << "Error parsing model file!!!";
        }
    }

    int Parse_lowerbounds(MyStringBuffer * sb, char * & token, int & iTokenType);
    int Parse_offsets(MyStringBuffer * sb, char * & token, int & iTokenType);
    int Parse_rootfilters(MyStringBuffer * sb, char * & token, int & iTokenType);
    int Parse_partfilters(MyStringBuffer * sb, char * & token, int & iTokenType);
    int Parse_components(MyStringBuffer * sb, char * & token, int & iTokenType);
    int Parse_defs(MyStringBuffer * sb, char * & token, int & iTokenType);

    int ParseFromText(string szModel);
};
