
#ifndef _CONNECTPOOL_H_
#define _CONNECTPOOL_H_

//#include "myosmutex.h"
#ifdef WIN32
#include <process.h>
#include <winsock2.h>
#else
#include <sys/errno.h>
#include <pthread.h>
#endif

#include <mysql.h> //�ļ�λ�� MySQL �ṩ�� C API Ŀ¼��
#include <mysqld_error.h>

#include <map>
#include <vector>
#include <stdexcept>
#include <string>
#include <string.h>
#include <stdlib.h>

using namespace std;

class CSql_error : public std::runtime_error
{
private:
	std::string m_err;
public:
  explicit CSql_error();
  
  explicit CSql_error(const std::string &whatarg);
 
  explicit CSql_error(const std::string &whatarg, const std::string &err);
    
  virtual ~CSql_error() throw ();
  
  virtual const char * what() const throw ();
};

class CConnect
{
private:
	//
protected:
    CConnect(const CConnect &);
    CConnect &operator=(const CConnect &);
	//����ԭ��
	std::string m_err;
public:
	//�������
	CConnect();
	
	//��������
	virtual ~CConnect();
	
	//����ָ�������ݿ�
	virtual bool Connect(const std::string &host,
		                 const std::string &user,
		                 const std::string &password,
		                 const std::string &dbname,
		                 unsigned int port,
						 const std::string &unix_socket,
						 const std::string &character) = 0;

	//�õ����ӵ��﷨
	virtual const std::string GetConnectSyntax() = 0;
	
	//�ж�����
	virtual bool Disconnect() = 0;

	//�õ����Ӷ���
	virtual void * GetConnect()  = 0;
	//virtual MYSQL * GetConnect()  = 0;
		//����ԭ��
	std::string What(){return m_err;}
};


class CMysqlConnect : public CConnect
{
private:
	//һ������
	MYSQL *m_conn;

	//MYSQL ������ copy,���� ��������
	CMysqlConnect(const CMysqlConnect &rhs);

	//��ֵ����Ҳ������
	CMysqlConnect & operator=(const CMysqlConnect &rhs);
	
protected:
	//
public:
	//�������
	CMysqlConnect();
		
	//��������
	virtual ~CMysqlConnect();

	//����ָ�������ݿ�
	virtual bool Connect(const std::string &host,
		                 const std::string &user,
		                 const std::string &password,
		                 const std::string &dbname,
		                 unsigned int port,
						 const std::string &unix_socket,
						 const std::string &character);
		
	//�ж�����
	virtual bool Disconnect();

	//�õ�����
	void * GetConnect();

	//�õ����ӵ��﷨
	virtual const std::string GetConnectSyntax();

};

class CDataStore
{
private:
	//
protected:
	//
public:

	CDataStore();
	
	virtual ~CDataStore();
	
	virtual bool SetTransAction(CConnect * conn) = 0;
	
	//ִ�����ݶ�������(DDL)�����
	virtual bool Exec(const std::string &ddl) = 0;

	//ִ�����ݶ������(DML)�����
	virtual bool Query(const std::string &dml) = 0;

	//�����ύ
	virtual bool Commit() = 0;

	//����ع�
	virtual bool RollBack() = 0;

	//�õ���ѯ��¼��
	virtual unsigned long RowCount() = 0;

	//����ԭ��
	virtual const std::string What() = 0;

	//�õ�ָ����ĳ���ֶε��ַ�������ֵ
	virtual const std::string GetItemString(
		unsigned long row,
		unsigned int index) = 0;
	virtual const std::string GetItemString(
		unsigned long row,
		const std::string &fieldname) = 0;

	
	//�õ�ָ����ĳ���ֶε���ֵ
	virtual float GetItemFloat(unsigned long row,
		const unsigned int index) = 0;
	virtual float GetItemFloat(unsigned long row,
		const std::string &fieldname) = 0;

	//�õ�ָ����ĳ���ֶε�����ֵ
	virtual long GetItemLong(unsigned long row,
		const unsigned int index) = 0;
	virtual long GetItemLong(unsigned long row,
		const std::string &fieldname) = 0;
};


class CMysqlStore : public CDataStore
{
private:
	//ָ��  mysql ������ָ��
	MYSQL * m_connptr;

	//ָ��  mysql �Ĳ�ѯ���ݼ�
	MYSQL_RES *m_resultptr;

	//����Ӱ��ļ�¼��
	unsigned long m_row;
	
	//����ԭ��
	std::string m_err;

	//�µ����������к�
	long m_increaseID;

	//�����ύģʽ
	bool m_autocommit;

	//�ֶ��������ֶ����͵Ķ�Ӧ��
	enum  filedtype_t
	{CHAR = 1,INT = 2,DATETIME = 3,DOUBLE = 4,DEC = 5,UNKNOWN = 6};
	struct typeset_t
	{
		std::string name;
		filedtype_t type;
		unsigned int length;
		//��ѯ�б��е���λ��
		unsigned int index;
		typeset_t();	
	};

	//ȡ����Ϣ״̬
	bool m_getstatus;
	
	//�ֶ���Ϣ��
	std::vector<typeset_t> m_fieldtype;

	typedef std::vector<std::string> row_t;
	
	std::vector<row_t> m_recordset;

	//�����ʱ����
	void Clear();	

	//�ҵ���
	row_t * FindRow(unsigned long findrow);
	
	//�ҵ���Ӧ�������
	unsigned int GetFieldIndex(const std::string &fieldname);

	//�����������к�
	void SetIncreaseID(long id);

	const std::string GetItemValue(unsigned long row,
		unsigned int index);

	const std::string GetItemValue(unsigned long row,
		const std::string &fieldname);

protected:
	//
public:

	CMysqlStore();

	virtual ~CMysqlStore();


	//�������Ӷ���
	bool SetTransAction(CConnect * conn);

	//�õ���ǰִ��״̬
	bool GetStatus();
	
	//����ԭ��
	virtual const std::string What();
	
	//ִ�����ݶ�������(DDL)�����
	virtual bool Exec(const std::string &ddl);

	//ִ�����ݲ�������(DML)�����
	virtual bool Query(const std::string &dml);

	filedtype_t SetFieldType(enum_field_types fieldtype);

	//�õ������������к�
	long GetIncreaseID();
	
	//�����ύ
	virtual bool Commit();

	//����ع�
	virtual bool RollBack();

	//�õ���ѯ��¼��
	virtual unsigned long RowCount();
		
	//�õ�ָ����ĳ���ֶε��ַ�������ֵ
	virtual const std::string GetItemString(unsigned long row,
		unsigned int index);
	
	virtual const std::string GetItemString(unsigned long row,
		const std::string &fieldname);
		
	//�õ�ָ����ĳ���ֶε���ֵ
	virtual float GetItemFloat(unsigned long row,
		const unsigned int index);
		
	virtual float GetItemFloat(unsigned long row,
		const std::string &fieldname);
	
	//�õ�ָ����ĳ���ֶε�����ֵ
	virtual long GetItemLong(unsigned long row,
		const unsigned int index);
		
	virtual long GetItemLong(unsigned long row,
		const std::string &fieldname);

	MYSQL* GetMySqlConn()
	{
		return m_connptr;
	}
};

#endif

