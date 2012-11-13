
#include "split_string.h"

#include <algorithm>
#include <ctype.h>
#include "define.h"

using namespace std;

string remove_space(const string & str)
{
	string tmp;
	remove_copy_if(str.begin(), str.end(), back_inserter(tmp), static_cast< int (*)(int)>(isspace) );
	return tmp;
} 

void SplitString(const string & sStr, const char sDelimiter, vector<string> & vArray)
{
    if( sStr.empty() )
        return ;
    string::size_type nLen    = 0;
    string strSub;
    string::size_type nPos    = 0;
	string::size_type nPosEnd = 0;
    do 
    {
        nPosEnd = sStr.find( sDelimiter, nPos );
        if( nPosEnd == sStr.npos )
        {  nPosEnd = sStr.length(); }

        if( nPosEnd == nPos )
            continue;

        nLen   = nPosEnd - nPos;
        strSub = sStr.substr( nPos, nLen );
        if (! strSub.empty())
        {            
           vArray.push_back( remove_space(strSub ));
        }
     }while( (nPos = nPosEnd + 1) < sStr.length() );
}

int atoi_s( const string & str )
{
	return str.empty() ? 0 : atoi(str.c_str());
}

void toVector( const char * date, const size_t nLen ,vector<char> & RefVec)
{
    RefVec.resize(nLen);
    RefVec.clear();
	for (size_t i = 0; i < nLen; ++i)
	{
        RefVec[i] = date[i];
	}
}
string & toString( const vector<char > & v, string & buf )
{
    buf.clear();
    copy(v.begin(), v.end(), back_inserter(buf));
    return buf;
}


