
#include "AsyncDBOperator.h"
#include "ConfigMgr.h"
#include "log.h"
#include "DatabaseMysql.h"
#include <cstdarg>

CAsyncDBOperator::~CAsyncDBOperator(void)
{
}

CAsyncDBOperator::CAsyncDBOperator(const string & sql)
:m_strSQL(sql)
{
}

int CAsyncDBOperator::call (DatabaseMysql * pDatabase)
{
    log_debug("CAsyncDBOperator::call. SQL:=%s.", m_strSQL.c_str());
    if (pDatabase->PExecute_(m_strSQL) == false)
    {
        log_error("CAsyncDBOperator::call. failed. SQL:=%s", m_strSQL.c_str());
    }

    return 0;
}

bool CAsyncTask::init(DatabaseMysql *& pDatabase)
{
    pDatabase = new DatabaseMysql;
    if( pDatabase == NULL )
    {
        log_error("CAsyncTask::init. pDatabase == NULL.");
        return false;
    }

    if(pDatabase->Connect(g_GetConfigMgr().getDBConfig()) == false)
    {
        log_error("CAsyncTask::init. FAILED CONNECT TO DB!");
        return false;
    }

    return true;
}
void CAsyncTask::stop()
{
    m_bRun = false;
}
void CAsyncTask::run()
{
    DatabaseMysql * m_pDatabase = NULL;

    if(init(m_pDatabase) == false)
    {
        log_error("CAsyncTask init fail \n");
        return ;
    }

    assert(m_pDatabase != NULL);
    m_bRun = true;
    while( m_bRun )
    {
        if(!queue_.empty())
        {
            lock_.Lock();
            CAsyncDBOperator * pAsync = queue_.front(); 
            queue_.pop();
            lock_.Unlock();

            if(pAsync != NULL)
            {
                pAsync->call(m_pDatabase);
                delete pAsync;
            }
        }
        else
        {
            sleep(100);
        }
    }
    log_error("CAsyncTask run over \n");
}

CAsyncTask::CAsyncTask()
{
    m_bRun = false;
}
CAsyncTask::~CAsyncTask()
{
}

void CAsyncTask::push(CAsyncDBOperator * inPacket)
{
    lock_.Lock();
    queue_.push(inPacket);
    lock_.Unlock();
}

void CAsyncTask::execSQL_( const char * format, ... )
{
    m_pszSQL[0] = '\0';

    va_list ap;
    va_start(ap, format);
    int ret = vsnprintf( m_pszSQL, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if( -1 == ret)
    {
        log_error("SQL Query error(not execute) for format '%s'.", format);
        return;
    }

    execSQL(string(m_pszSQL));
}

void CAsyncTask::execSQL( const string & strSQL )
{
    push( new CAsyncDBOperator(strSQL) );
}

