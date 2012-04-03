#pragma once
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

const int g_maxTokenLen = 100;
const int g_space = 1000;
const int g_number = 1001;
const int g_text = 1002;

class MyStringBuffer {
  private:
	string * buffer;
	int pointer;
	int len;
  public:
	MyStringBuffer(string * buffer) {
	  this->buffer = buffer;
	  this->pointer = 0; 
	  this->len = buffer->length();
	}

	char next() {
	  if (this->pointer <= this->len - 1)
		return this->buffer->at(this->pointer++);
	  else
		return EOF;
	}

	void moveback() {
	  this->pointer--;
	}
};

bool IsSpecialChar(char c);

char * NextToken(MyStringBuffer * sb, int & iTokenType);

void SkipComment(MyStringBuffer * sb, char * & pNextToken, int & iNextTokenType);

void SkipSpace(MyStringBuffer * sb, char * & pNextToken, int & iNextTokenType);

void ParseFields(vector<string> & vecFields, MyStringBuffer * sb, char * & token, int & iTokenType);

void ParseArrayDim(vector<int> & vecDims, MyStringBuffer * sb, char * & token, int & iTokenType);

template <class T>
void ParseArrayValue(int nelem, T * array, MyStringBuffer * sb, char * & token, int & iTokenType);

void ParseCellDim(vector<int> & vecDims, MyStringBuffer * sb, char * & token, int & iTokenType);

template <class T>
T ReadNumber(MyStringBuffer * sb, char * & token, int & iTokenType);

template <class T>
T* Read1DArray(int & nelem, MyStringBuffer * sb, char * & token, int & iTokenType, vector<int> * vecDims = NULL);

template <class T, class T_primitive>
void ReadNDArray(T parray, MyStringBuffer * sb, char * & token, int & iTokenType);

template <class T>
void push_vector(vector<T> & vecDst, T * arraySrc, int nelem);
