#include "ConfigMgr.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include "tinyxml.h"
#include "log.h"
#include "DBOperator.h"

using namespace std;
// init config
ConfigMgr::ConfigMgr(void)
{
    m_ServerID = 0;
	m_ExperimentMechine.pcate = 201;
	m_ExperimentMechine.pframe = 2002;

	m_InstorMechine.pcate = 201;
	m_InstorMechine.pframe = 2001;

	m_PowerMechine.pcate = 211;
	m_PowerMechine.pframe = 2012;

	m_Orm_Floor.pcate = 401;
	m_Orm_Floor.pframe = 4001;

	m_Orm_Floor_Goods.pcate = 501;
	m_Orm_Floor_Goods.pframe = 5001;

	m_Orm_Wall.pcate = 601;
	m_Orm_Wall.pframe = 6001;

	m_Orm_Wall_Goods.pcate = 701;
	m_Orm_Wall_Goods.pframe = 7001;

	m_Info_Mechine.pcate = 231;
	m_Info_Mechine.pframe = 2003;
	
	m_Resource_Mechine.pcate = 221;
	m_Resource_Mechine.pframe = 2005;
}

ConfigMgr::~ConfigMgr(void)
{
}
bool  ConfigMgr::Init()
{
    return true;
}
void  ConfigMgr::ShutDown()
{
    
}

void ConfigMgr::ReadConfig(char * configureName)
{
	if (m_localCfg.LoadFile(configureName) == false)
    {
        log_debug( " ConfigMgr::update. 加载配置文件. 格式错误 \n" );
        throw runtime_error("ConfigMgr::update. 加载配置文件, 格式错误\n");
    }

    TiXmlElement * pRootElement = m_localCfg.RootElement();
    if (pRootElement == NULL)
    {
        log_debug( " ConfigMgr::update. 读取config.xml失败. \n");
        throw runtime_error("ConfigMgr::update. ASSERT(pRootElement != NULL)");
    }

    Load(pRootElement);

    if (m_globalCfg.LoadFile(GetGlobalConfig(pRootElement).c_str()) == false)
        return;

    Load(m_globalCfg.RootElement());
}
std::string ConfigMgr::GetGlobalConfig( TiXmlElement * pRootElement )
{
    TiXmlNode * pGlobalElement = pRootElement->FirstChild("GLOBAL");
    if(pGlobalElement == NULL)
        return "";

    return pGlobalElement->ToElement()->Attribute("path");
}
void ConfigMgr::Load( TiXmlElement * pRootElement )
{
    readDBConfig( pRootElement );
	readCentServerInfo( pRootElement );
    readServerInfo( pRootElement);
    readMonitor( pRootElement );
    readPhpUrl( pRootElement );
	readRedisConfig(pRootElement);
	readCreateNewUserConfig(pRootElement);
	readReplyIphoneLogin(pRootElement);
	readReplyRegist(pRootElement);
	readCreateUserBaseConfig(pRootElement);
}

void ConfigMgr::readDBConfig(TiXmlElement * pRootElement)
{
    TiXmlNode * pDBNode = pRootElement->FirstChild("DB");
    if (pDBNode != NULL)
    {	
        TiXmlNode *pGameItems = pDBNode;
        m_dbConfig.host = pGameItems->ToElement()->Attribute("host");
        m_dbConfig.port = pGameItems->ToElement()->Attribute("port");
        m_dbConfig.passwd = pGameItems->ToElement()->Attribute("passwd");
        m_dbConfig.db = pGameItems->ToElement()->Attribute("db");
        m_dbConfig.user = pGameItems->ToElement()->Attribute("user");

        log_debug( " ConfigMgr::readDBConfig. HOST:=%s, PORT:=%s, USER:=%s, PW:=%s, DB:=%s\n", 
        m_dbConfig.host.c_str(), m_dbConfig.port.c_str(), m_dbConfig.user.c_str(), m_dbConfig.passwd.c_str(), m_dbConfig.db.c_str());
    }
}

void ConfigMgr::readCentServerInfo(TiXmlElement * pRootElement)
{
	TiXmlNode * pDBNode = pRootElement->FirstChild("CentServer");
	if (pDBNode != NULL)
	{	
		TiXmlNode *pGameItems = pDBNode;
		m_CentConfig.host = pGameItems->ToElement()->Attribute("host");
		m_CentConfig.port = pGameItems->ToElement()->Attribute("port");

		log_debug("CentServer IP:%s Port:%s \n",m_CentConfig.host.c_str(),m_CentConfig.port.c_str());
	}
}

void ConfigMgr::readServerInfo(TiXmlElement * pRootElement)
{
	//getConfig("APP", "SVID", m_ServerID);
	TiXmlNode * pDBNode = pRootElement->FirstChild("ServerInfo");
	if (pDBNode != NULL)
	{	
		TiXmlNode *pGameItems = pDBNode;        
		m_nGroupID  = atoi(pGameItems->ToElement()->Attribute("GroupID"));
		log_debug("read groupid is %d \n",m_nGroupID);

		m_ServerConfig.id = m_ServerID = atoi(pGameItems->ToElement()->Attribute("ServerID"));
		log_debug("read serverid is %d \n",m_ServerID);

		m_ServerConfig.type = m_nServerType = atoi(pGameItems->ToElement()->Attribute("ServerType"));
		log_debug("read servertype is %d \n",m_nServerType);

		m_ServerConfig.host = pGameItems->ToElement()->Attribute("IP");
		m_ServerConfig.port = atoi(pGameItems->ToElement()->Attribute("PORT"));
	}
}

void ConfigMgr::readMonitor(TiXmlElement * pRootElement)
{
    if (m_monitor.host.empty())
    {
        TiXmlNode * pMonitor = pRootElement->FirstChild("Monitor");
        if (pMonitor != NULL)
        {
            m_monitor.host = pMonitor->ToElement()->Attribute("HOST");
            m_monitor.port = atoi(pMonitor->ToElement()->Attribute("PORT"));
            m_monitor.enable = !(strcmp(pMonitor->ToElement()->Attribute("ENABLE"), "0") == 0);
            log_debug(" ConfigMgr::readMonitor. HOST:=%s, PORT:=%d, ENABLE:=%d\n", m_monitor.host.c_str(), m_monitor.port, m_monitor.enable ? 1 : 0 );
        }
    }
}
void ConfigMgr::readPhpUrl(TiXmlElement * pRootElement)
{
    if(m_php_url.empty())
    {
        TiXmlNode * pNode = pRootElement->FirstChild("PHPAPI");
        if( pNode != NULL )
        {
            m_php_url = pNode->ToElement()->Attribute("URL");
            m_php_pwd = pNode->ToElement()->Attribute("PWD");

            log_error("php_url. m_url:=%s.pwd = %s ",m_php_url.c_str(),m_php_pwd.c_str());
        }
    }
}

bool ConfigMgr::getAttr( TiXmlDocument & xml, const char * key, const char * attr, string & v )
{
	TiXmlElement * p = xml.RootElement();
	if (p == NULL)
	{
		return false;
	}

	p = p->FirstChildElement(key);
	if ( p == NULL)
	{
		return false;
	}

	return p->QueryStringAttribute(attr, &v) == TIXML_SUCCESS;
}

bool ConfigMgr::getConfig( const char * key, const char * attr, int & v )
{
	string s;
	if (getConfig(key, attr, s) == true)
	{
		v = atoi_s(s);
		return true;
	}
	else
	{
		return false;
	}
}

void ConfigMgr::LoadCfgFromDB()
{
	//readServerInfo();

	//if (g_GetCDBOperator().getServerConfig(m_ServerID, m_serverinfo) == false)
	//{
	//	log_error("FAILED READ SERVER CONFIG.");
	//	return;
	//}

	//log_debug("read serverid is SVID:=%d, MGID:=%d, GROUPS:=%d, TYPE:=%d. \n",m_ServerID, m_serverinfo.mgid, m_serverinfo.groups, m_serverinfo.type);

	//readChatCenter();
	//readClientVersion();

	// 加载机器建筑时间
	g_GetCDBOperator().GetMechineComStructTimeList(m_Mechine_ConStructTimeList);
	log_debug("the m_Mechine_ConStructTimeList size is %d\n", m_Mechine_ConStructTimeList.size());
}

int ConfigMgr::getComStructTimeBy_Pcate_Pframe(int pcate, int pframe)
{
	int times = 0;
	int len = m_Mechine_ConStructTimeList.size();

	for (int i = 0; i < len; i++)
	{
		if (m_Mechine_ConStructTimeList[i].pcate == pcate && m_Mechine_ConStructTimeList[i].pframe)
		{
			times = m_Mechine_ConStructTimeList[i].Time;
			break;
		}
	}
	return times;
}

void ConfigMgr::readChatCenter()
{
	if (g_GetCDBOperator().getChatCenter(m_serverinfo.mgid, m_serverinfo.groups, m_CentConfig.host, m_CentConfig.port) == true)
	{
		log_error("ConfigMgr::readChatCenter. HOST:=%s, PORT:=%s.", m_CentConfig.host.c_str(), m_CentConfig.port.c_str());
	}
	else
	{
		log_error("ConfigMgr::readChatCenter  FAILED.");
		throw std::logic_error("ConfigMgr::readChatCenter  FAILED");
	}
}

void ConfigMgr::readClientVersion()
{
	g_GetCDBOperator().getLastASVersion(m_ServerID, m_serverinfo.groups, m_strClientVersion);

	if (m_strClientVersion.empty())
	{
		log_error("ASSERT(m_strClientVersion.empty() == false)");
		throw runtime_error("ASSERT(m_strClientVersion.empty() == false)");
	}
}

void ConfigMgr::readTaskDefine()
{
	m_taskDefine.clear();

	g_GetCDBOperator().getTaskDefine(m_taskDefine);

	log_debug("ConfigMgr::readTaskDefine. m_taskDefine:=%ld.", m_taskDefine.size());
}

const TaskDefines_type & ConfigMgr::getTaskDefine()
{
	if (m_taskDefine.empty() == true)
	{
		readTaskDefine();
	}

	return m_taskDefine;
}

void ConfigMgr::readRedisConfig(TiXmlElement *pRootElement)
{
	TiXmlNode * pDBNode = pRootElement->FirstChild("RedisServer");
	if (pDBNode != NULL)
	{	
		TiXmlNode *pGameItems = pDBNode;        
		m_redisConfig.chIP  = pGameItems->ToElement()->Attribute("HOST");
		m_redisConfig.port = atoi(pGameItems->ToElement()->Attribute("PORT"));
	}
}

// 读取创建一个新用户的sql
void ConfigMgr::readCreateNewUserConfig(TiXmlElement *pRootElement)
{
	TiXmlNode * pDBNode = pRootElement->FirstChild("CreateNewUser");
	if (pDBNode != NULL)
	{	
		TiXmlNode *pGameItems = pDBNode;        
		m_createNewUserSql  = pGameItems->ToElement()->Attribute("Sql");
	}
	log_debug("the create new user sql is %s\n", m_createNewUserSql.c_str());
}

void ConfigMgr::readCreateUserBaseConfig(TiXmlElement *pRootElement)
{
	TiXmlNode * pDBNode = pRootElement->FirstChild("CreateUserBase");
	if (pDBNode != NULL)
	{	
		TiXmlNode *pGameItems = pDBNode;        
		m_createUserBaseSql  = pGameItems->ToElement()->Attribute("Sql");
	}
	log_debug("the create user base sql is %s\n", m_createUserBaseSql.c_str());
}

void ConfigMgr::readReplyIphoneLogin(TiXmlElement *pRootElement)
{
	TiXmlNode * pDBNode = pRootElement->FirstChild("REPLY_IPHONE_LOGIN");

	if (pDBNode != NULL)
	{	
		string cmd = pDBNode->ToElement()->Attribute("CMD");
		m_userLoginReply.cmd = atoi(cmd.c_str());

		log_debug("the cmd is %s\n", cmd.c_str());
		TiXmlNode *pGameItems = pDBNode;   

		ReplyType vdata;
		for( TiXmlNode*  item = pGameItems->FirstChild( "spec" );item;item = item->NextSibling( "spec" ) ) 

		{
			if (item != NULL)
			{
				vdata.dbitem = item->ToElement()->Attribute("dbitem");
				vdata.type = item->ToElement()->Attribute("type");

				m_userLoginReply.data.push_back(vdata);
				log_debug("reply config is dbitem %s, type %s\n", vdata.dbitem.c_str(), vdata.type.c_str());
			}
		}
	}
}

void ConfigMgr::readReplyRegist(TiXmlElement *pRootElement)
{
	TiXmlNode * pDBNode = pRootElement->FirstChild("REPLY_REGIST");

	if (pDBNode != NULL)
	{	
		string cmd = pDBNode->ToElement()->Attribute("CMD");
		m_RegistReply.cmd = atoi(cmd.c_str());

		log_debug("the cmd is %s\n", cmd.c_str());
		TiXmlNode *pGameItems = pDBNode;   

		ReplyType vdata;
		for( TiXmlNode*  item = pGameItems->FirstChild( "spec" );item;item = item->NextSibling( "spec" ) ) 

		{
			if (item != NULL)
			{
				vdata.dbitem = item->ToElement()->Attribute("dbitem");
				vdata.type = item->ToElement()->Attribute("type");

				m_RegistReply.data.push_back(vdata);
				log_debug("reply config is dbitem %s, type %s\n", vdata.dbitem.c_str(), vdata.type.c_str());
			}
		}
	}
}