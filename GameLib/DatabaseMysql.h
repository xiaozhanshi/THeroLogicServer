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

    //���ӵ�ָ�����ݿ�.
    bool Connect(const DBConfig_type & config);

    //ִ��SQL��ѯ���
    QueryResult * PQuery(const char* format, ...);

	bool PQuery(const string & strSQL);

	QueryResult* PQueryS(const string & strSQL);

    //ִ��SQL���
    bool PExecute(const char* format, ...);

    //ִ��SQL���
    bool PExecute_(const string & strSQL);

    //�Ͽ������ݿ������
    void Disconnect();

    //��ȡ��Ӱ�������
    int getRows();

	//��ȡ��������
	int getIncreaseID();

public:
    CMysqlConnect*	m_conn;
    CMysqlStore*	m_store;
};

#endif

