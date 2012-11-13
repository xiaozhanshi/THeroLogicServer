
#include "DatabaseMysql.h"
#include "connectpool.h"
#include <cstdarg>
#include "log.h"

using namespace std;

DatabaseMysql::DatabaseMysql()
:m_conn(NULL)
,m_store(NULL)
{	
}

DatabaseMysql::~DatabaseMysql()
{
    Disconnect();
}

bool DatabaseMysql::Connect(const DBConfig_type & config)
{
    m_conn = new CMysqlConnect();
    m_store = new CMysqlStore();
    char szUnixSock[32] = {0};

    log_debug("DatabaseMysql::Connect. HOST:=%s, PORT:=%s, USER:=%s, PASSWORD:=%s, DBNAME:=%s", 
        config.host.c_str(), config.port.c_str(), config.user.c_str(), config.passwd.c_str(), config.db.c_str());

    try
    {
        if(!m_conn->Connect(config.host, config.user, config.passwd, config.db, atoi(config.port.c_str()), szUnixSock, "utf8"))
        {
            log_error("FAILED. Connect to mysql database error:%s.", m_conn->What().c_str());
            return false;
        }
    }
    catch(...)
    {
        log_error("A exception occured when connect to db!");
        return false;
    }

    m_store->SetTransAction(m_conn);
    return true;
}

QueryResult* DatabaseMysql::PQuery(const char* format, ...)
{
    char szQryBuf[MAX_QUERY_LEN];
    va_list ap;
    va_start(ap, format);
    int ret = vsnprintf( szQryBuf, MAX_QUERY_LEN, format, ap);
    va_end(ap);
    if( -1 == ret )
    {
        log_error("SQL Query error(not execute) for format '%s'.", format);
        return NULL;
    }
    bool success;
    success = m_store->Query(szQryBuf);
	log_debug("SQL '%s' Query", szQryBuf);
    if(!success)
    {
        log_error("SQL '%s' Query error.", szQryBuf);
        return NULL;
    }
    return m_store;
}

bool DatabaseMysql::PQuery(const string & strSQL)
{
	return m_store->Query(strSQL.c_str());
}

QueryResult* DatabaseMysql::PQueryS(const string & strSQL)
{
	bool success;
	success = m_store->Query(strSQL.c_str());
	log_debug("SQL '%s' Query", strSQL.c_str());
	if(!success)
	{
		log_error("SQL '%s' Query error.", strSQL.c_str());
		return NULL;
	}
	return m_store;
}

bool DatabaseMysql::PExecute(const char* format, ...)
{
    char szQryBuf[MAX_QUERY_LEN] = {0};
    va_list ap;
    va_start(ap, format);
    int ret = vsnprintf( szQryBuf, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if( -1 == ret)
    {
        log_error("SQL Query error(not execute) for format '%s'.", format);
        return false;
    }

	log_debug("%s",szQryBuf);
    return m_store->Exec(szQryBuf);
}

bool DatabaseMysql::PExecute_(const string & strSQL)
{
    return m_store->Exec(strSQL);
}

void DatabaseMysql::Disconnect()
{
    if(m_conn)
    {
        m_conn->Disconnect();
        delete m_conn;
        m_conn = NULL;
    }

    if(m_store)
    {
        delete m_store;
        m_store = NULL;
    }
}

int DatabaseMysql::getRows()
{
    return m_store->RowCount();
}

int DatabaseMysql::getIncreaseID()
{
	return m_store->GetIncreaseID();
}

