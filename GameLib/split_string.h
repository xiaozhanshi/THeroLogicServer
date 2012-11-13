#ifndef _SPLIT_STRING_H_
#define _SPLIT_STRING_H_ 

#include <vector>
#include <stdlib.h>
#include <string>

using namespace std;

template <class _Type>
vector<char > & toVector( const _Type & v, vector<char > & buf )
{
	buf.clear();
	copy(v.begin(), v.end(), back_inserter(buf));
	return buf;
}

template <class _Type>
void toVector( const _Type & v,vector<char> & RefVec)
{
    RefVec.resize(v.size());
    RefVec.clear();
    copy(v.begin(), v.end(), back_inserter(RefVec.begin()));	
}

string remove_space(const string & str);

void toVector(const char * date, const size_t nLen,vector<char> & RefVec);

void SplitString(const string & sStr, const char sDelimiter, vector < string > & vArray);

int atoi_s(const string & str);

string & toString(const vector<char >  & v, string & buf);

#endif //_SPLIT_STRING_H_

