#ifndef __DATABASE_MYSQL_H
#define __DATABASE_MYSQL_H

#include <string>
using namespace std;


#include "define.h"

class CMysqlConnect;
class CMysqlStore;
typedef CMysqlStore QueryResult;

typedef struct _tagDBConfig
{
    string host;
    string port;
    string user;
    string passwd;
    string db;
}DBConfig_type;

class DatabaseMysql
{	
protected:
    friend class CDBOperator;
    friend class CAsyncTask;

    DatabaseMysql(const DatabaseMysql &);
    DatabaseMysql & operator=(const DatabaseMysql &);

public:
    DatabaseMysql();
    ~DatabaseMysql();

    //连接到指定数据库.
    bool Connect(const DBConfig_type & config);

    //执行SQL查询语句
    QueryResult * PQuery(const char* format, ...);

	bool PQuery(const string & strSQL);

	QueryResult* PQueryS(const string & strSQL);

    //执行SQL语句
    bool PExecute(const char* format, ...);

    //执行SQL语句
    bool PExecute_(const string & strSQL);

    //断开与数据库的连接
    void Disconnect();

    //获取受影响的行数
    int getRows();

	//获取自增序列
	int getIncreaseID();

public:
    CMysqlConnect*	m_conn;
    CMysqlStore*	m_store;
};

#endif

