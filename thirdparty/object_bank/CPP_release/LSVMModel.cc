#include "LSVMModel.h"
#include <stdio.h>
#include <assert.h>
#include "parserUtil.h"
#include <string.h>
#define AssertStringEqual(x1, x2) do {assert(strcmp(x1, x2));} while(0);
#define AssertNumEqual(x1, x2) do {assert(x1 == x2);} while(0);

void COffset::ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);    
    AssertNumEqual(iTokenType, '<');
    vector<string> vecStructFileds;
    ParseFields(vecStructFileds, sb, token, iTokenType);
    AssertNumEqual(iTokenType, '>');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '<');

    do{
        token = NextToken(sb, iTokenType);
        AssertNumEqual(iTokenType, g_text);
        if (strcmp(token, "w") == 0)
        {
            this->w = ReadNumber<double>(sb, token, iTokenType);
        }
        else if (strcmp(token, "blocklabel") == 0)
        {
            this->blocklabel = ReadNumber<int>(sb, token, iTokenType);
        }
    } while(iTokenType == ','); 
    AssertNumEqual(iTokenType, '>');
    token = NextToken(sb, iTokenType);
}

int CModel::Parse_offsets(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    vector<int> vecDims;
    ParseCellDim(vecDims, sb, token, iTokenType);

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '{');
    AssertNumEqual(vecDims.size(), 2);
    AssertNumEqual(vecDims[0], 1);

    int nelem = vecDims[0] * vecDims[1]; 

    do
    {
        COffset * offset = new COffset();
        offset->ParseFromFileStream(sb, token, iTokenType);
        this->offsets.push_back(offset);
        nelem--;   
    } while (iTokenType == ',');

    AssertNumEqual(nelem, 0);
    AssertNumEqual(iTokenType, '}');
    token = NextToken(sb, iTokenType);
    return 0;
}

void CRootFilter::ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);    
    AssertNumEqual(iTokenType, '<');
    vector<string> vecStructFileds;
    ParseFields(vecStructFileds, sb, token, iTokenType);
    AssertNumEqual(iTokenType, '>');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '<');

    do{
        token = NextToken(sb, iTokenType);
        AssertNumEqual(iTokenType, g_text);
        if (strcmp(token, "size") == 0)
        {
            int nelem;
            int * size = Read1DArray<int>(nelem, sb, token, iTokenType);
            memcpy(this->size, size, sizeof(this->size));
        }
        else if (strcmp(token, "w") == 0)
        {
            int nelem;
            double * res = Read1DArray<double>(nelem, sb, token, iTokenType, &this->w.vecDims);
            AssertNumEqual(this->w.vecDims.size(), 3);
            double *** w = new double**[this->w.vecDims[0]];
            int i, j, k;
            for (i = 0; i < this->w.vecDims[0]; i++)
            {
                w[i] = new double*[this->w.vecDims[1]];
                for (j = 0; j < this->w.vecDims[1]; j++)
                    w[i][j] = new double[this->w.vecDims[2]];
            }
            int p = 0;
            for (k = 0; k < this->w.vecDims[2]; k++)
                for (j = 0; j < this->w.vecDims[1]; j++)
                    for (i = 0; i < this->w.vecDims[0]; i++)                    
                        w[i][j][k] = res[p++];

            this->w.w = w;
        }
        else if (strcmp(token, "blocklabel") == 0)
        {
            this->blocklabel = ReadNumber<int>(sb, token, iTokenType);
        }
    } while (iTokenType == ',');

    AssertNumEqual(iTokenType, '>');
    token = NextToken(sb, iTokenType);
}

int CModel::Parse_rootfilters(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    vector<int> vecDims;
    ParseCellDim(vecDims, sb, token, iTokenType);

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '{');
    AssertNumEqual(vecDims.size(), 2);
    AssertNumEqual(vecDims[0], 1);

    int nelem = vecDims[0] * vecDims[1]; 

    do
    {
        CRootFilter * rootfilter = new CRootFilter();
        rootfilter->ParseFromFileStream(sb, token, iTokenType);
        this->rootfilters.push_back(rootfilter);
        nelem--;   
    } while (iTokenType == ',');

    AssertNumEqual(nelem, 0);
    AssertNumEqual(iTokenType, '}');
    token = NextToken(sb, iTokenType);
    return 0;
} 

void CPartFilter::ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);    
    AssertNumEqual(iTokenType, '<');
    vector<string> vecStructFileds;
    ParseFields(vecStructFileds, sb, token, iTokenType);
    AssertNumEqual(iTokenType, '>');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '<');

    do{
        token = NextToken(sb, iTokenType);
        AssertNumEqual(iTokenType, g_text);
        if (strcmp(token, "fake") == 0)
        {
            this->fake = ReadNumber<int>(sb, token, iTokenType);
        }
        else if (strcmp(token, "partner") == 0)
        {
            this->partner = ReadNumber<int>(sb, token, iTokenType);
        }
        else if (strcmp(token, "blocklabel") == 0)
        {
            this->blocklabel = ReadNumber<int>(sb, token, iTokenType);
        }
        else if (strcmp(token, "w") == 0)
        {
            int nelem;
            double * res = Read1DArray<double>(nelem, sb, token, iTokenType, &this->w.vecDims);
            AssertNumEqual(this->w.vecDims.size(), 3);
            this->w.w = new double**[this->w.vecDims[0]];
            int i, j, k;
            for (i = 0; i < this->w.vecDims[0]; i++)
            {
                this->w.w[i] = new double*[this->w.vecDims[1]];
                for (j = 0; j < this->w.vecDims[1]; j++)
                    this->w.w[i][j] = new double[this->w.vecDims[2]];
            }
            int p = 0;
            for (k = 0; k < this->w.vecDims[2]; k++)            
                for (j = 0; j < this->w.vecDims[1]; j++)
                    for (i = 0; i < this->w.vecDims[0]; i++)        
                        this->w.w[i][j][k] = res[p++];
        }
    } while (iTokenType == ',');

    AssertNumEqual(iTokenType, '>');
    token = NextToken(sb, iTokenType);
}

int CModel::Parse_partfilters(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    vector<int> vecDims;
    ParseCellDim(vecDims, sb, token, iTokenType);

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '{');
    AssertNumEqual(vecDims.size(), 2);

    int nelem = vecDims[0] * vecDims[1]; 

    do
    {
        if (nelem > 0) {
            CPartFilter * partfilter = new CPartFilter();
            partfilter->ParseFromFileStream(sb, token, iTokenType);
            this->partfilters.push_back(partfilter);
            nelem--;   
        }
        else { 
            token = NextToken(sb, iTokenType);    
        } 
    } while (iTokenType == ',');

    AssertNumEqual(nelem, 0);
    AssertNumEqual(iTokenType, '}');
    token = NextToken(sb, iTokenType);
    return 0;
}

void CDef::ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);    
    AssertNumEqual(iTokenType, '<');
    vector<string> vecStructFileds;
    ParseFields(vecStructFileds, sb, token, iTokenType);
    AssertNumEqual(iTokenType, '>');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '<');

    do{
        token = NextToken(sb, iTokenType);
        AssertNumEqual(iTokenType, g_text);
        if (strcmp(token, "anchor") == 0)
        {
            int nelem;
            int * anchor = Read1DArray<int>(nelem, sb, token, iTokenType);
            memcpy(this->anchor, anchor, sizeof(this->anchor));
        }
        else if (strcmp(token, "w") == 0)
        {
            int nelem;
            this->w = Read1DArray<double>(nelem, sb, token, iTokenType);
        }
        else if (strcmp(token, "blocklabel") == 0)
        {
            this->blocklabel = ReadNumber<int>(sb, token, iTokenType);
        }
    } while (iTokenType == ',');

    AssertNumEqual(iTokenType, '>');
    token = NextToken(sb, iTokenType);
}

int CModel::Parse_defs(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    vector<int> vecDims;
    ParseCellDim(vecDims, sb, token, iTokenType);

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '{');
    AssertNumEqual(vecDims.size(), 2);

    int nelem = vecDims[0] * vecDims[1]; 

    do
    {
        if (nelem > 0) {
            CDef * def = new CDef();
            def->ParseFromFileStream(sb, token, iTokenType);
            this->defs.push_back(def);
            nelem--;   
        }
        else { 
            token = NextToken(sb, iTokenType);    
        }

    } while (iTokenType == ',');

    AssertNumEqual(nelem, 0);
    AssertNumEqual(iTokenType, '}');
    token = NextToken(sb, iTokenType);
    return 0;
}

void CPart::ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);    
    AssertNumEqual(iTokenType, '<');
    vector<string> vecStructFileds;
    ParseFields(vecStructFileds, sb, token, iTokenType);
    AssertNumEqual(iTokenType, '>');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '<');

    do{
        token = NextToken(sb, iTokenType);
        AssertNumEqual(iTokenType, g_text);
        if (strcmp(token, "partindex") == 0)
        {
            this->partindex = ReadNumber<int>(sb, token, iTokenType);
        }
        else if (strcmp(token, "defindex") == 0)
        {
            this->defindex = ReadNumber<int>(sb, token, iTokenType);
        }
    } while (iTokenType == ',');

    AssertNumEqual(iTokenType, '>');
    token = NextToken(sb, iTokenType);
}

int CComponent::Parse_parts(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    vector<int> vecDims;
    ParseCellDim(vecDims, sb, token, iTokenType);

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '{');
    AssertNumEqual(vecDims.size(), 2);

    int nelem = vecDims[0] * vecDims[1]; 

    do
    {
        if (nelem > 0) {
            CPart * part = new CPart();
            part->ParseFromFileStream(sb, token, iTokenType);
            this->parts.push_back(part);
            nelem--;   
        }
        else { 
            token = NextToken(sb, iTokenType);    
        }
    } while (iTokenType == ',');

    AssertNumEqual(nelem, 0);
    AssertNumEqual(iTokenType, '}');
    token = NextToken(sb, iTokenType);
    return 0;
}

void CComponent::ParseFromFileStream(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);    
    AssertNumEqual(iTokenType, '<');
    vector<string> vecStructFileds;
    ParseFields(vecStructFileds, sb, token, iTokenType);
    AssertNumEqual(iTokenType, '>');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '<');

    int nelem;
    do{
        token = NextToken(sb, iTokenType);
        AssertNumEqual(iTokenType, g_text);
        if (strcmp(token, "rootindex") == 0)
        {
            this->rootindex = ReadNumber<int>(sb, token, iTokenType);
        }
        else if (strcmp(token, "offsetindex") == 0)
        {
            this->offsetindex = ReadNumber<int>(sb, token, iTokenType);
        }
        else if (strcmp(token, "parts") == 0)
        {
            Parse_parts(sb, token, iTokenType);
        }
        else if (strcmp(token, "dim") == 0)
        {
            this->dim = ReadNumber<int>(sb, token, iTokenType);
        }
        else if (strcmp(token, "numblocks") == 0)
        {
            this->numblocks = ReadNumber<int>(sb, token, iTokenType);
        }
        else if (strcmp(token, "x1") == 0)
        {
            double * x1 = Read1DArray<double>(nelem, sb, token, iTokenType);
            push_vector<double>(this->x1, x1, nelem);
        }
        else if (strcmp(token, "y1") == 0)
        {
            double * y1 = Read1DArray<double>(nelem, sb, token, iTokenType);
            push_vector<double>(this->y1, y1, nelem);
        }
        else if (strcmp(token, "x2") == 0)
        {
            double * x2 = Read1DArray<double>(nelem, sb, token, iTokenType);
            push_vector<double>(this->x2, x2, nelem);
        }
        else if (strcmp(token, "y2") == 0)
        {
            double * y2 = Read1DArray<double>(nelem, sb, token, iTokenType);
            push_vector<double>(this->y2, y2, nelem);
        }
    } while (iTokenType == ',');

    AssertNumEqual(iTokenType, '>');
    token = NextToken(sb, iTokenType);
}

int CModel::Parse_components(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    vector<int> vecDims;
    ParseCellDim(vecDims, sb, token, iTokenType);

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '{');
    AssertNumEqual(vecDims.size(), 2);
    AssertNumEqual(vecDims[0], 1);

    int nelem = vecDims[0] * vecDims[1]; 

    do
    {
        CComponent * component = new CComponent();
        component->ParseFromFileStream(sb, token, iTokenType);
        this->components.push_back(component);
        nelem--;   
    } while (iTokenType == ',');

    AssertNumEqual(nelem, 0);
    AssertNumEqual(iTokenType, '}');
    token = NextToken(sb, iTokenType);
    return 0;
}

int CModel::Parse_lowerbounds(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    vector<int> vecDims;
    ParseCellDim(vecDims, sb, token, iTokenType);

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '{');
    AssertNumEqual(vecDims.size(), 2);
    AssertNumEqual(vecDims[0], 1);

    int nelem = vecDims[0] * vecDims[1]; 

    do
    {
        int nmem;
        double * res = Read1DArray<double>(nmem, sb, token, iTokenType);
        Tlowerbounds * lowerbound = new Tlowerbounds;
        push_vector(*lowerbound, res, nmem);
        this->lowerbounds.push_back(lowerbound);
        nelem--;   
    } while (iTokenType == ',');

    AssertNumEqual(nelem, 0);
    AssertNumEqual(iTokenType, '}');
    token = NextToken(sb, iTokenType);
    return 0;
}


int CModel::ParseFromText(string szModel)
{
    FILE * fp;
    if ((fp  = fopen(szModel.c_str(), "r")) != NULL)
    { 
        string file_content = "";
        char c;
        while((c=fgetc(fp)) != EOF) {
            file_content.append(&c, 1);
        }
        string * pfc = &file_content;
        MyStringBuffer * sb = new MyStringBuffer(pfc);

        char * token;
        int iTokenType;
        token = NextToken(sb, iTokenType);

        if (iTokenType == g_text && strcmp(token, "model") == 0)
        {
            token = NextToken(sb, iTokenType);
            AssertNumEqual(iTokenType, '<');

            vector<string> vecStructFileds;
            ParseFields(vecStructFileds, sb, token, iTokenType);
            AssertNumEqual(iTokenType, '>');

            token = NextToken(sb, iTokenType);
            AssertNumEqual(iTokenType, '=');

            token = NextToken(sb, iTokenType);
            AssertNumEqual(iTokenType, '<');

            do{
                token = NextToken(sb, iTokenType);
                AssertNumEqual(iTokenType, g_text);
                if (strcmp(token, "sbin") == 0)
                {
                    this->sbin = ReadNumber<int>(sb, token, iTokenType);
                }
                else if (strcmp(token, "rootfilters") == 0)
                {
                    Parse_rootfilters(sb, token, iTokenType);
                }
                else if (strcmp(token, "offsets") == 0)
                {
                    Parse_offsets(sb, token, iTokenType);
                }
                else if (strcmp(token, "blocksizes") == 0)
                {
                    int nelem;
                    int * blocksizes = Read1DArray<int>(nelem, sb, token, iTokenType);
                    push_vector<int>(this->blocksizes, blocksizes, nelem);
                }
                else if (strcmp(token, "regmult") == 0)
                {
                    int nelem;
                    double * regmult = Read1DArray<double>(nelem, sb, token, iTokenType);
                    push_vector<double>(this->regmult, regmult, nelem);
                }
                else if (strcmp(token, "learnmult") == 0)
                {
                    int nelem;
                    double * learnmult = Read1DArray<double>(nelem, sb, token, iTokenType);
                    push_vector<double>(this->learnmult, learnmult, nelem);
                }
                else if (strcmp(token, "lowerbounds") == 0)
                {
                    Parse_lowerbounds(sb, token, iTokenType);
                }
                else if (strcmp(token, "components") == 0)
                {
                    Parse_components(sb, token, iTokenType);
                }
                else if (strcmp(token, "interval") == 0)
                {
                    this->interval = ReadNumber<int>(sb, token, iTokenType);
                }
                else if (strcmp(token, "numcomponents") == 0)
                {
                    this->numcomponents = ReadNumber<int>(sb, token, iTokenType);
                }
                else if (strcmp(token, "numblocks") == 0)
                {
                    this->numblocks = ReadNumber<int>(sb, token, iTokenType);
                }
                else if (strcmp(token, "partfilters") == 0)
                {
                    Parse_partfilters(sb, token, iTokenType);
                }
                else if (strcmp(token, "defs") == 0)
                {
                    Parse_defs(sb, token, iTokenType);
                }
                else if (strcmp(token, "maxsize") == 0)
                {
                    int nelem;
                    int * maxsize = Read1DArray<int>(nelem, sb, token, iTokenType);
                    memcpy(this->maxsize, maxsize, nelem * sizeof(int));
                }
                else if (strcmp(token, "minsize") == 0)
                {
                    int nelem;
                    int * minsize = Read1DArray<int>(nelem, sb, token, iTokenType);
                    memcpy(this->minsize, minsize, nelem * sizeof(int));
                }
                else if (strcmp(token, "thresh") == 0)
                {
                    this->thresh = ReadNumber<double>(sb, token, iTokenType);
                }
                else if (strcmp(token, "negfrompos_overlap") == 0)
                {
                    this->negfrompos_overlap = ReadNumber<double>(sb, token, iTokenType);
                }
            } while (iTokenType != '>'); 
            AssertNumEqual(iTokenType, '>');
            delete sb;
        }
        else
        {
            cout << "File format incorrect, model expected" << endl;
        }
    }
    else
    {
        cout << "Cannot open File!" << endl;
    }
    return 0;
}
