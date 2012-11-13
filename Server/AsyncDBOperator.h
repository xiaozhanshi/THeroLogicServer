
#ifndef _ASYNC_DBOPERATOR_H_20110714
#define _ASYNC_DBOPERATOR_H_20110714

#include <string>
#include <queue>

#include "define.h"
#include "NonCopyableClass.h"
#include "Thread.h"

class NETInputPacket;
class DatabaseMysql;

using namespace std;

class CAsyncDBOperator : public NonCopyableClass
{
public:
    explicit CAsyncDBOperator(const string & sql);
public:
    ~CAsyncDBOperator(void);

public:
    int call (DatabaseMysql * pDatabase);

private:
    const string m_strSQL;
};

class  CAsyncTask : public Thread
{
public:
    CAsyncTask();
    virtual ~CAsyncTask();

private:
    bool init(DatabaseMysql *& pDatabase);

public:
    virtual void stop();

    virtual void run();

    virtual void push(CAsyncDBOperator * op);

    void execSQL( const string & strSQL );

    void execSQL_(const char * format, ... );

private:
    bool    m_bRun;
    MyLock  lock_;

    queue<CAsyncDBOperator *> queue_;

    char m_pszSQL[MAX_QUERY_LEN];
};

#endif //_ASYNC_DBOPERATOR_H_20110714

