#ifndef _CONFIGMGR_H_
#define _CONFIGMGR_H_


#include <map>
#include <string>
#include <vector>
using namespace std;

#include "SingletonClass.h"
#include "define.h"
#include "GameData.h"
#include "DatabaseMysql.h"
#include "tinyxml.h"
#include "DBOperator.h"
#include "TaskHandler.h"

// memcache 配置
struct MemcacheConfig
{
    string host;
    string port;
    string key;
    int32  hashindex;

    bool operator < (const MemcacheConfig & other)const
    {
        if (key != other.key)
        {
            return key < other.key;
        }
        else
        {
            return  hashindex < other.hashindex;
        }
    }
};

// 中心服务器配置
struct CentServerConfig 
{
    string host;
    string port;
};

typedef struct ServerConfig_S 
{
	int id;
	int type;
	string host;
	int port;
}ServerConfig_T;

typedef struct _tagRedisServer
{
	string chIP;
	int port;
}RedisServerConfig;

typedef struct _tagReplyType
{
	string dbitem;
	string type;
}ReplyType;

typedef struct _tagReplyType_login
{
	int cmd;
	vector<ReplyType> data;
}ReplyType_login;

typedef vector<MemcacheConfig> MemcacheConfigs_type;

class ConfigMgr : public AutoDeleteSingleton < ConfigMgr >
{
    friend  class AutoDeleteSingleton < ConfigMgr >; 
public:
    ConfigMgr(void);
    virtual ~ConfigMgr(void);
    bool    Init();
    void    ShutDown();

	void LoadCfgFromDB();

    typedef map<string, string> key_value_type;
    typedef map<string, key_value_type> KeySet_type; 
    void ReadConfig(char * configureName);

    string  GetGlobalConfig(TiXmlElement * pRootElement);

    // 获得DB数据库配置
    const DBConfig_type & getDBConfig()const
    {
        return m_dbConfig;
    }
    // 获得中心服务器配置
    const CentServerConfig & getCentServerConfig()const
    {
        return m_CentConfig;
    }    
	const ServerConfig_T & getServerConfig()const
	{
		return m_ServerConfig;
	}
    uint32 getServerID()const
    {
        return m_ServerID;
    }
    uint32 getGroupID()const
    {
        return m_serverinfo.groups;
    }

	uint32 getServerType()const
	{
		return m_serverinfo.type;
	}

    // 获得监控服务器信息
    const Monitor_type & getMonitorInfo()const
    {
        return m_monitor;
    }
    const string & getCreateNewUserSql() const
    {
        return m_createNewUserSql;
    }
	const string & getCreateUserBaseSql() const
	{
		return m_createUserBaseSql;
	}
	const string & getPhpUrl() const
	{
		return m_php_url;
	}
    const string & getPhpPwd() const
    {
        return m_php_pwd;
    }

	const MechineType_PcatePframe_T & getMechine_Experiment_Type()const
	{
		return m_ExperimentMechine;
	}
	const MechineType_PcatePframe_T & getMechine_Instor_Type()const
	{
		return m_InstorMechine;
	}
	const MechineType_PcatePframe_T & getMechine_PowerMechine_Type()const
	{
		return m_PowerMechine;
	}
	const MechineType_PcatePframe_T & getOrm_Floor_Type()const
	{
		return m_Orm_Floor;
	}
	const MechineType_PcatePframe_T & getOrm_Floor_Goods_Type()const
	{
		return m_Orm_Floor_Goods;
	}
	const MechineType_PcatePframe_T & getOrm_Wall_Type()const
	{
		return m_Orm_Wall;
	}
	const MechineType_PcatePframe_T & getOrm_WallGoods_Type()const
	{
		return m_Orm_Wall_Goods;
	}

	const MechineType_PcatePframe_T & getMechine_Info_Type()const
	{
		return m_Info_Mechine;
	}
	const MechineType_PcatePframe_T & getRole_Type()const
	{
		return m_Role;
	}
	const MechineType_PcatePframe_T & getMechine_Resource_Type()const
	{
		return m_Resource_Mechine;
	}

	bool getConfig(const char * key, const char * attr, string & v)
	{
		if (getAttr(m_localCfg, key, attr, v) != 0)
		{
			return getAttr(m_globalCfg, key, attr, v) == 0;
		}
		else
		{
			return true;
		}
	}

	int getComStructTimeBy_Pcate_Pframe(int pcate, int pframe);

	const TaskDefines_type & getTaskDefine();


	bool getConfig(const char * key, const char * attr, int & v);

	void readChatCenter();

	void readCentServerInfo(TiXmlElement * pRootElement);

	void readClientVersion();
	
	const Server_type & getServerCfg()const
	{
		return m_serverinfo;
	}

	const ReplyType_login & getRegistRely() const
	{
		return m_RegistReply;
	}

	const ReplyType_login & getReplyConfig()const
	{
		return m_userLoginReply;
	}

	const string & getASVersion()const
	{
		return m_strClientVersion;
	}

	void readTaskDefine();

	const RedisServerConfig & getRedisConfig()const
	{
		return m_redisConfig;
	}
	int getCheckTime()
	{
		return 10;
	}
	int getWriteDBTime()
	{
		return 300;
	}
private:
	bool getAttr(TiXmlDocument & xml, const char * key, const char * attr, string & v);

	TiXmlDocument m_localCfg;
	TiXmlDocument m_globalCfg;

private:
    void Load( TiXmlElement * pRootElement );

    void readDBConfig(TiXmlElement * pRootElement);
    
    //void readCentServerConfig(TiXmlElement *pRootElement);
    void readServerInfo(TiXmlElement *pRootElement);
    void readMonitor(TiXmlElement * pRootElement);
    void readPhpUrl(TiXmlElement * pRootElement);
	void readRedisConfig(TiXmlElement *pRootElement);
	void readCreateNewUserConfig(TiXmlElement *pRootElement);
	void readCreateUserBaseConfig(TiXmlElement *pRootElement);
	void readReplyIphoneLogin(TiXmlElement *pRootElement);
	void readReplyRegist(TiXmlElement *pRootElement);

private:
    MemcacheConfigs_type    m_memConfig;
    DBConfig_type           m_dbConfig;
    CentServerConfig        m_CentConfig;
	ServerConfig_T			m_ServerConfig;
    int		                m_ServerID;
	int						m_nGroupID;
	int						m_nServerType;
	Server_type				m_serverinfo;
    Monitor_type            m_monitor;
    string                  m_php_url;
    string                  m_php_pwd;
	string					m_strClientVersion;
	string					m_createNewUserSql;
	string					m_createUserBaseSql;

	TaskDefines_type m_taskDefine;
	RedisServerConfig		m_redisConfig;
	ReplyType_login			m_userLoginReply;
	ReplyType_login			m_RegistReply;
	int						m_shouldReplyRegist;

	MechineType_PcatePframe_T m_ExperimentMechine;
	MechineType_PcatePframe_T m_InstorMechine;
	MechineType_PcatePframe_T m_PowerMechine;		// 能源上限机
	MechineType_PcatePframe_T m_Orm_Floor;			// 地板
	MechineType_PcatePframe_T m_Orm_Floor_Goods;	// 地板上面的物品
	MechineType_PcatePframe_T m_Orm_Wall;			// 墙壁
	MechineType_PcatePframe_T m_Orm_Wall_Goods;		// 墙壁上面的物品

	MechineType_PcatePframe_T m_Info_Mechine;		// 情报机
	MechineType_PcatePframe_T m_Resource_Mechine;	// 资源机
	MechineType_PcatePframe_T m_Role;	// 主角
	MechineType_PcatePframe_T m_Prop_Mechine;  //道具合成机
	MechineComStructTimeTable_List m_Mechine_ConStructTimeList;		// 升级消耗表

};

// 获取配置管理对象
#ifndef  g_GetConfigMgr
#define  g_GetConfigMgr()   (ConfigMgr::Instance())
#endif

#endif //_CONFIGMGR_H_

