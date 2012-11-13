
#include "connectpool.h"
#include "log.h"

// linux ��ϵͳ������� -lmysql -I/usr/local/mysql/inlucde
#ifdef WIN32
#pragma comment(lib, "libmysql.lib")
#endif        


//CConnPool * CConnPool::m_instance = NULL;

CSql_error::CSql_error():std::runtime_error("Failed query"),m_err()
{
	//
}

CSql_error::CSql_error(const std::string &whatarg)
:std::runtime_error(whatarg),m_err()
{
	//
}

CSql_error::CSql_error(const std::string &whatarg, const std::string &err)
:std::runtime_error(whatarg),m_err(err)
{
	//
}

CSql_error::~CSql_error() throw ()
{
	//
}

const char * CSql_error::what() const throw ()
{
	return m_err.c_str();
}

//�������
CConnect::CConnect()
{
	//
}

//��������
CConnect::~CConnect()
{
	//
}

//�������
CMysqlConnect::CMysqlConnect():m_conn(NULL)
{
	m_err = "";
	//
}

//��������
CMysqlConnect::~CMysqlConnect()
{
	if(m_conn)
	{
		Disconnect();
	}
}

//����ָ�������ݿ�
bool CMysqlConnect::Connect(const std::string &host,
							const std::string &user,
							const std::string &password,
							const std::string &dbname,
							unsigned int port, 
							const std::string &unix_socket,
							const std::string &character)
{
	//��ʼ�����ݽṹ
	if(NULL == (m_conn = mysql_init(m_conn)))
	{
		return false;
	}
	int ret = mysql_options(m_conn, MYSQL_OPT_RECONNECT, "1");
	if(NULL == mysql_real_connect(m_conn,host.c_str(),
		user.c_str(),password.c_str(),dbname.c_str(),port,unix_socket.c_str(),
		CLIENT_MULTI_RESULTS|CLIENT_MULTI_STATEMENTS))
	{
		throw CSql_error(mysql_error(m_conn));
		return false ;
	}
	mysql_set_character_set(m_conn, character.c_str());
	return true;
}

//�ж�����
bool CMysqlConnect::Disconnect()
{
	if(m_conn)
	{
		mysql_close(m_conn);
		m_conn = NULL;
	}
	return true;
}

//�õ�����
void * CMysqlConnect::GetConnect()
{
	if(m_conn == NULL)
	{
		return NULL;
	}

	//�Զ���������
	if(mysql_ping(m_conn) != 0) //����ʧ�� Ҫ���
	{
		m_err = mysql_error(m_conn);
		
		return NULL;
	}

	return m_conn;
}

//�õ����ӵ��﷨
const std::string CMysqlConnect::GetConnectSyntax()
{
	return "";
}

CDataStore::CDataStore()
{
	//
}

CDataStore::~CDataStore()
{
	//
}

CMysqlStore::typeset_t::typeset_t()
{
	name = "";
	type = (filedtype_t)6;
	length = 0;
	index = 0;
}

CMysqlStore::CMysqlStore():m_connptr(NULL),m_resultptr(NULL),
m_row(0),m_err(""),m_increaseID(0),
m_autocommit(true),m_getstatus(false)
{
	//
}

CMysqlStore::~CMysqlStore()
{
	Clear();
}

void CMysqlStore::Clear()
{
	m_recordset.clear();
	m_fieldtype.clear();
	m_row = 0;
	if (m_resultptr != NULL)
	{
		mysql_free_result(m_resultptr);
		m_resultptr = NULL;
	}
	SetIncreaseID(0);
}

//�������Ӷ���
bool CMysqlStore::SetTransAction(CConnect * conn)
{
	if(conn == 0)
	{
		m_err = "conn == 0";
		return false;
	}

	m_connptr = (MYSQL * )conn->GetConnect();
	if(m_connptr == NULL)
	{
		//CConnPool::Instance().Erase(conn); //������ʧЧ
		m_err = "m_connptr == 0";
		m_err += conn->What();
		return false;
	}

	mysql_autocommit(m_connptr,m_autocommit);
	return true;
}

//�õ���ǰִ��״̬
bool CMysqlStore::GetStatus()
{
	return m_getstatus;
}

//����ԭ��
const std::string CMysqlStore::What()
{
	return m_err;
}

CMysqlStore::filedtype_t CMysqlStore::SetFieldType(enum_field_types fieldtype)
{
	filedtype_t type;
	switch(fieldtype)
	{
	case MYSQL_TYPE_STRING:
		//
	case MYSQL_TYPE_VAR_STRING:
		//
	//case MYSQL_TYPE_TEXT:
		//
	case MYSQL_TYPE_BLOB:
		//
	case MYSQL_TYPE_SET:
		//
	case MYSQL_TYPE_GEOMETRY:
		//
	case MYSQL_TYPE_NULL:
		type = CHAR;
		break;
	case MYSQL_TYPE_TINY:
		//
	case MYSQL_TYPE_SHORT:
		//
	case MYSQL_TYPE_LONG:
		//
	case MYSQL_TYPE_INT24:
		//
	case MYSQL_TYPE_BIT:
		//
	case MYSQL_TYPE_ENUM:
		//
	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_LONGLONG:
		type = INT;
		break;
	case MYSQL_TYPE_DECIMAL:
		//
	case MYSQL_TYPE_NEWDECIMAL:
		type = DEC;
		break;
	case MYSQL_TYPE_FLOAT:
		//
	case MYSQL_TYPE_DOUBLE:
		type = DOUBLE;
		break;
	case MYSQL_TYPE_TIMESTAMP:
		//
	case MYSQL_TYPE_DATE:
		//
	case MYSQL_TYPE_TIME:
		type = DATETIME;
		break;
	default:
		type = UNKNOWN;
		break;
	}
	return type;
}


//����ع�
bool CMysqlStore::RollBack()
{
	if(m_autocommit)
	{
		return true;
	}

	if(mysql_rollback(m_connptr) == 0)
	{
		return true;
	}

	return false;
}

//�����ύ
bool CMysqlStore::Commit()
{
	if(m_autocommit)
	{
		return true;
	}

	if(mysql_commit(m_connptr) == 0)
	{
		return true;
	}

	return false;
}

//ִ�����ݶ�������(DDL)�����
bool CMysqlStore::Exec(const std::string &ddl)
{
	//�������
	Clear();

	if(0 == mysql_query(m_connptr,ddl.c_str()))
	{
		//�õ���Ӱ�������
		do{
			m_row +=(unsigned long)mysql_affected_rows(m_connptr);
			m_increaseID =(long)mysql_insert_id(m_connptr);
		}
		while(!mysql_next_result(m_connptr));
	}
	else
	{
		m_err = mysql_error(m_connptr);
		log_error("mysql exec error %s\n", m_err.c_str());
		return false;
	}

	return true;
}

void CMysqlStore::SetIncreaseID(long id)
{
	m_increaseID = id;
}

//�õ������������к�
long CMysqlStore::GetIncreaseID()
{
	return m_increaseID;
}

CMysqlStore::row_t * CMysqlStore::FindRow(unsigned long findrow)
{
	if(findrow >= this->RowCount() || m_recordset.size() == 0)
	{
		return NULL;
	}

	return &m_recordset[findrow];
}

//�õ���ѯ��¼��
unsigned long CMysqlStore::RowCount()
{
	return m_row;
}


unsigned int CMysqlStore::GetFieldIndex(const std::string &fieldname)
{
	unsigned int index = 10000;
	for(unsigned int i = 0; i < m_fieldtype.size(); ++i)
	{
		if(0 == strcmp(m_fieldtype[i].name.c_str(),
			fieldname.c_str()))
		{
			index = m_fieldtype[i].index;
			break;
		}
	}
	return index;
}


const std::string CMysqlStore::GetItemValue(unsigned long row,
											unsigned int index)
{
	if(index >= m_fieldtype.size())
	{
		m_err = "column index upper bound";
		//�õ���ǰִ��״̬
		m_getstatus = false;
		return "";
	}

	row_t * rowvalue = FindRow(row);
	if(rowvalue ==  NULL)
	{
		m_err = "row index upper bound";
		m_getstatus = false;
		return "";
	}

	return (*rowvalue)[index];
}

const std::string CMysqlStore::GetItemValue(unsigned long row,
											const std::string &fieldname)
{
	int index = 10000;
	if((index = GetFieldIndex(fieldname)) >= 10000)
	{
		m_err = "column index upper bound";
		return "";
	}

	row_t * rowvalue = FindRow(row);
	if(rowvalue ==  NULL)
	{
		m_err = "row index upper bound";
		m_getstatus = false;
		return "";
	}

	return (*rowvalue)[index];
}

//�õ�ָ����ĳ���ֶε��ַ�������ֵ
const std::string CMysqlStore::GetItemString(unsigned long row,
											 unsigned int index)
{
	return GetItemValue(row,index);
}

const std::string CMysqlStore::GetItemString(unsigned long row,
											 const std::string &fieldname)
{
	return GetItemValue(row,fieldname);
}

//�õ�ָ����ĳ���ֶε���ֵ
float CMysqlStore::GetItemFloat(unsigned long row,
								const unsigned int index)
{
	return (float)atof(GetItemValue(row,index).c_str());
}


float CMysqlStore::GetItemFloat(unsigned long row,
								const std::string &fieldname)
{
	return (float)atof(GetItemValue(row,fieldname).c_str());
}
 

//�õ�ָ����ĳ���ֶε�����ֵ
long CMysqlStore::GetItemLong(unsigned long row,
							  const unsigned int index)
{
	return atol(GetItemValue(row,index).c_str());
}

long CMysqlStore::GetItemLong(unsigned long row,
							  const std::string &fieldname)
{
	return atol(GetItemValue(row,fieldname).c_str());
}


//ִ�����ݲ�������(DML)�����
bool CMysqlStore::Query(const std::string &dml)
{
	//�������
	Clear();

	if(mysql_query(m_connptr,dml.c_str()) != 0)
	//if(mysql_real_query(m_connptr,dml.c_str(), dml.length()) != 0)
	{
		m_err = mysql_error(m_connptr);
		log_error("Query mysql exec error %s\n", m_err.c_str());
		return false;
	}
	do
	{
		//log_debug("the start query\n");
		m_resultptr = mysql_store_result(m_connptr);
		if(m_resultptr == NULL)
			continue;
		//�õ���ѯ���ص�����
		m_row += (unsigned long)mysql_affected_rows(m_connptr);
		//log_debug("the after query, the rote is %d\n", m_row);
		//ָ��  mysql �Ĳ�ѯ�ֶμ�
		MYSQL_FIELD *fieldptr = NULL;

		//ȡ�ø��ֶ���������
		while(fieldptr = mysql_fetch_field(m_resultptr))
		{
			typeset_t typeset;
			typeset.index = (unsigned int)m_fieldtype.size();
			typeset.length = fieldptr->length;
			typeset.name = fieldptr->name;
			typeset.type = SetFieldType(fieldptr->type);
			m_fieldtype.push_back(typeset);
		}

		MYSQL_ROW currrow = NULL;
		while((currrow = mysql_fetch_row(m_resultptr)))
		{
			//���еļ�¼
			const unsigned int colcount = mysql_num_fields(m_resultptr);
			row_t rows(colcount);
			for(unsigned int i = 0; i < colcount; ++i)
			{
				rows[i] = currrow[i] ? currrow[i] : "NULL";
			}
			m_recordset.push_back(rows);
		}
		mysql_free_result(m_resultptr); 
		m_resultptr = NULL;
	}
	while(!mysql_next_result(m_connptr));

	//mysql_free_result(m_resultptr); 
	//m_resultptr = NULL;

	return true;
}

