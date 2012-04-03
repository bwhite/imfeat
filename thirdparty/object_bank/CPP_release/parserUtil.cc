#include "parserUtil.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <cctype>
#include <typeinfo>
#include <iostream>
#include <assert.h>
#include <string.h>

#define AssertStringEqual(x1, x2) do {assert(strcmp(x1, x2) == 0);} while(0);
#define AssertNumEqual(x1, x2) do {assert(x1 == x2);} while(0);

template int* Read1DArray<int>(int & nelem, MyStringBuffer * sb, char * & token, int & iTokenType, vector<int> * vecDims = NULL);
template double* Read1DArray<double>(int & nelem, MyStringBuffer * sb, char * & token, int & iTokenType, vector<int> * vecDims = NULL);
template int ReadNumber<int>(MyStringBuffer * sb, char * & token, int & iTokenType);
template double ReadNumber<double>(MyStringBuffer * sb, char * & token, int & iTokenType);
template void push_vector<int>(vector<int> & vecDst, int * arraySrc, int nelem);
template void push_vector<double>(vector<double> & vecDst, double * arraySrc, int nelem);

bool IsSpecialChar(char c)
{
    return (c == '<' || c == '{' || c == '[' || c == ']' || c == '}' || c == '>' || c == ',' || c == '"' || c == '=') ? true : false;
}

char * NextToken(MyStringBuffer * sb, int & iTokenType)
{
    static char tokenbuf[g_maxTokenLen];
    int pidx = 0;
    char c = sb->next(); 

    if (isspace(c))
    {
        iTokenType = g_space;
        for(; isspace(c) && c != EOF; c = sb->next())
            ;
    }

    if (c == EOF)
    {
        iTokenType = EOF;
        return NULL;
    }
    else if (IsSpecialChar(c))
    {
        iTokenType = c;
        tokenbuf[pidx++] = c;
        c = sb->next();
    }
    else if (isdigit(c))
    {
        iTokenType = g_number;
        for(; (isdigit(c) || c == '.' || c == 'e' || c == '-') && c != EOF; c = sb->next())
            tokenbuf[pidx++] = c;
    }
    else // text
    {
        iTokenType = g_text;
        for(; !IsSpecialChar(c) && !isspace(c) && c != EOF; c = sb->next())
        {
            tokenbuf[pidx++] = c;
        }
    }

    sb->moveback();

    tokenbuf[pidx] = '\0';
    return tokenbuf;
}

void SkipComment(MyStringBuffer * sb, char * & pNextToken, int & iNextTokenType)
{
    int iTokenType;
    for(char * token = NextToken(sb, iTokenType); iTokenType != EOF && token[strlen(token)-1] != '\n'; token = NextToken(sb, iTokenType))
        ;

    pNextToken = NextToken(sb, iNextTokenType);
}

void SkipSpace(MyStringBuffer * sb, char * & pNextToken, int & iNextTokenType)
{
    int iTokenType;
    char * token;
    for (token = NextToken(sb, iTokenType); iTokenType != EOF && iTokenType == g_space; token = NextToken(sb, iTokenType))
        ;

    pNextToken = token;
    iNextTokenType = iTokenType;
}

void ParseFields(vector<string> & vecFields, MyStringBuffer * sb, char * & token, int & iTokenType)
{
    do
    {
        token = NextToken(sb, iTokenType);
        assert((iTokenType == g_text) || (iTokenType == g_number));
        vecFields.push_back(string(token));
        token = NextToken(sb, iTokenType);
    } while(iTokenType == ',');
}

void ParseArrayDim(vector<int> & vecDims, MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '[');
    do
    {
        token = NextToken(sb, iTokenType);
        AssertNumEqual(iTokenType, g_number);
        vecDims.push_back(atoi(token));
        token = NextToken(sb, iTokenType);
    } while(iTokenType == ',');
    AssertNumEqual(iTokenType, ']');
}

template <class T>
void ParseArrayValue(int nelem, T * array, MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '[');
    
    vector<string> vecFields;
    ParseFields(vecFields, sb, token, iTokenType);

    AssertNumEqual(iTokenType, ']');
    AssertNumEqual(vecFields.size(), nelem);
    token = NextToken(sb, iTokenType);

    for (int i = 0; i < nelem; i++)
    {
        if (strcmp(typeid(array[0]).name(), "i") == 0)
        {
            array[i] = atoi(vecFields[i].c_str());
        }
        else if(strcmp(typeid(array[0]).name(), "d") == 0)
        {
            array[i] = strtod(vecFields[i].c_str(), NULL);
        }
    }
}

void ParseCellDim(vector<int> & vecDims, MyStringBuffer * sb, char * & token, int & iTokenType)
{
    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '{');
    do
    {
        token = NextToken(sb, iTokenType);
        AssertNumEqual(iTokenType, g_number);
        vecDims.push_back(atoi(token));
        token = NextToken(sb, iTokenType);
    } while(iTokenType == ',');
    AssertNumEqual(iTokenType, '}');
}

template <class T>
T ReadNumber(MyStringBuffer * sb, char * & token, int & iTokenType)
{
    int nelem;
    T * res =  Read1DArray<T>(nelem, sb, token, iTokenType);
    AssertNumEqual(nelem, 1);

    T number = *res;
    delete [] res;
    return number;
}

template <class T>
T* Read1DArray(int & nelem, MyStringBuffer * sb, char * & token, int & iTokenType, vector<int> * vecDims = NULL)
{
    bool flag = false;
    if (vecDims == NULL)
    {
        vecDims = new vector<int>;
        flag = true;
    }

    ParseArrayDim((*vecDims), sb, token, iTokenType);
    nelem = 1;
    for (int i = 0; i < vecDims->size(); i++)
        nelem *= (*vecDims)[i];

    if (flag)
        delete vecDims;

    token = NextToken(sb, iTokenType);
    AssertNumEqual(iTokenType, '=');

    T * array = new T[nelem];
    ParseArrayValue<T>(nelem, array, sb, token, iTokenType);
    
    return array;
}

template <class T, class T_primitive>
void ReadNDArray(T parray, MyStringBuffer * sb, char * & token, int & iTokenType)
{
    T_primitive * array;
    int nelem;
    array = Read1DArray<T>(nelem, sb, token, iTokenType);
    T_primitive * parrayReshaped = (T_primitive *) parray;
    memcpy(parrayReshaped, array, nelem * sizeof(T_primitive));
    delete [] array;
}

template <class T>
void push_vector(vector<T> & vecDst, T * arraySrc, int nelem)
{
     vecDst.resize(nelem);
     memcpy(&vecDst[0], arraySrc, nelem * sizeof(T));
}
