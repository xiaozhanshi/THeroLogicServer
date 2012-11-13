#include "DBOperator.h"
#include "ConfigMgr.h"
#include "log.h"
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <exception>
#include <map>
#include "connectpool.h"
#include "AsyncDBOperator.h"
#include "ConfigMgr.h"
#include "json/json.h"
#include "curl/curl.h"
#include "curl/easy.h"
#include "PlatFunc.h"
#include "Player.h"

using namespace std;

namespace
{
    static const uint32 SECONDS_PER_ONE_MINUTE  = 60;
    static const uint32 SECONDS_PER_ONE_HOUR    = 3600;

}

GoodsStruct_t_list g_Goodslist;	// 建筑物数据

int ascii2utf (char *s, unsigned char *utf, int utfmax)
{
	int retval;
	for (retval = 0; *s && utfmax > 1; utfmax -= 2, retval += 2) {
		*utf++ = *s++;
		*utf++ = 0;
	}
	if (utfmax > 0) {
		*utf = *s;
		++retval;
	}
	return retval;
}

//json to int 转换
int32 JsonToInt(Json::Value &object,const char * str)
{
    if(object.isNull())
    {
        log_error("JsonToInt JSON OBJECT IS NULL");
        return -1;
    }

	if(object.isMember(str) == false)
	{
		log_error("JsonToInt There is no member named %s",str);
		return -1;
	}
    if(object[str].type() == Json::intValue)
    {
        return object[str].asInt();
    }
    else if (object[str].type() == Json::stringValue)
    {
        return atoi_s(object[str].asString());
    }
    else
    {
        log_error("JsonToInt %s VALUE TYPE ERROR.",str);
        return -1;
    }
}

class CatStr
{
public:
    CatStr(stringstream & str, const string strIgnored = string("")): m_str(str), m_strIgnored(strIgnored) {}

    void operator()(const string & str)
    {
        if (m_strIgnored != str)
        {
            if ( m_str.str().empty() )
            {
                m_str<<str;
            }else{
                m_str<<","<<str;
            }
        }
    }

private:
    stringstream & m_str;
    const string m_strIgnored;
};

// 进行数据库的连接
bool CDBOperator::init()
{
    m_pMysql = new DatabaseMysql;
    const DBConfig_type & config = g_GetConfigMgr().getDBConfig();

    return m_pMysql->Connect(config);
}

// 启动异步线程
// bool CDBOperator::startAsyncDB()
// {
// //     m_pAsyncTask = new CAsyncTask;
// //     m_pAsyncTask->start();
// 
//     return true;
// }

// 关闭清理
void CDBOperator::ShutDown()
{
    if(m_pMysql != NULL)
    {
        m_pMysql->Disconnect();
        delete m_pMysql;
        m_pMysql = NULL;
    }
//     if(m_pAsyncTask != NULL)
//     {
//         m_pAsyncTask->kill();
// 
//         delete m_pAsyncTask;
//         m_pAsyncTask = NULL;
//     }
}
// 从数据库读取双倍时间信息到DoubleExp
bool CDBOperator::getDoubleExpTime( map<uint32, map<uint32, uint32> > &DoubleExp)
{
    uint32 nWhichDay	= 0;
    uint32 nRecordCount = 0;
    while( nWhichDay < 7 )
    {
        QueryResult *result = m_pMysql->PQuery( "select begin, end from windones.doubleexp where type=%u and enable=1;", nWhichDay );
        if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
        {
            nWhichDay++;
            continue;
        }
        map<uint32, uint32> TimerValue;
        nRecordCount = 0;
        while( nRecordCount < result->RowCount() )
        {
            string strStartRowValue = result->GetItemString( nRecordCount, 0 );
            vector<string> attrs;
            // split时间为小时和分
            SplitString( strStartRowValue, ':', attrs );
            // 将双倍开始时间转换为秒，该秒数为距离当天0点的秒数
            uint32 nStartSeconds = atoi_s( attrs[0] ) * SECONDS_PER_ONE_HOUR + atoi_s( attrs[1] ) * SECONDS_PER_ONE_MINUTE;
            attrs.clear();
            string strEndRowValue = result->GetItemString( nRecordCount, 1 );
            SplitString( strEndRowValue, ':', attrs );
            // 将双倍结束时间转换为秒
            uint32 nEndSeconds = atoi_s( attrs[0] ) * SECONDS_PER_ONE_HOUR + atoi_s( attrs[1] ) * SECONDS_PER_ONE_MINUTE;
            TimerValue.insert( make_pair( nStartSeconds, nEndSeconds ) ); 			
            nRecordCount++;
        }
        DoubleExp.insert( make_pair( nWhichDay, TimerValue ) );
        nWhichDay++;
    }
    return true;
}

// 获得一个玩家的聊天服务器id
uint32  CDBOperator::GetUserServerID( uint32 UserID )
{
	// TODO: use redis C_CHAT_SID%MID
	return 1;
}

// 获取用户取得的md5验证的key
bool CDBOperator::getUserKey(const uint32 uuid, string & key)
{

	return true;
}

void   CDBOperator::SetUserServerID( uint32 UserID ,uint32 ServerID )
{
	// TODO : USE Redis
}

// 获得一个玩家的状态
uint32 CDBOperator::GetUserState( uint32 UserID )
{
	// TODO : USE Redis C_CHAT_STATE%MID
	return 0;
}

void  CDBOperator::SetUserState( uint32 UserID,uint32 nState)
{

}

// 设置服务器负载
void CDBOperator::SetServerStress( uint32 nValue )
{
	// todo: use redis
}

#if 0

// 调用PHP使用喇叭. 老接口, 新代码应使用 CDBOperator::UseProps
bool CDBOperator::UseSpeakByPHP( const uint32 uuid )
{
    string url = g_GetConfigMgr().getPhpUrl();
    Json::Value object;
    object["sitekey"] = "";
    object["method"] = "GameForserver.useHorn";
    object["mnick"] = "";
    object["mid"] = static_cast<int>(uuid);
    object["key"] = g_GetConfigMgr().getPhpPwd();
    object["param"] = "";
    object["group"] = "";
    string data = string("win_param=") + object.toStyledString();//post数据

    return ExePhpAPI(url,data);
}

#endif

bool CDBOperator::UseProps( const uint32 uuid, const int mppid , const int pcate, const int pframe )
{
	Json::Value param;
	param["mppid"] = mppid;
	param["pcate"] = pcate;
	param["pframe"] = pframe;

	string url = g_GetConfigMgr().getPhpUrl();
	Json::Value object;
	object["sitekey"] = "";
	object["method"] = "GameForserver.cUseProp";
	object["mnick"] = "";
	object["mid"] = static_cast<int>(uuid);
	object["key"] = g_GetConfigMgr().getPhpPwd();
	object["param"] = param;
	object["group"] = "";
	string data = string("win_param=") + object.toStyledString();//post数据

	return ExePhpAPI(url,data);
}

bool CDBOperator::getServerConfig( const uint32 sid, Server_type & s )
{
	///读取server启动配置
	QueryResult* result = m_pMysql->PQuery("SELECT `subtype`, `svtype`, `groups`, `mgid` FROM `windones_sysdata`.`server_reconstruct` "
		" where svid=%u;", sid);
	if(result == NULL)
	{
		log_error("CDBOperator::getRoomList. FAILED .");
		return false;
	}

	s.subtype = result->GetItemLong(0, "subtype");
	s.type =  result->GetItemLong(0, "svtype");
	s.groups = result->GetItemLong(0, "groups");
	s.mgid = result->GetItemLong(0, "mgid");

	log_debug("CDBOperator::getRoomList SVID=%d, SEVERTYPE=%d, MGID:=%d, GROUPS:=%d.",sid, s.type, s.mgid, s.groups);

	return true;
}

bool CDBOperator::getChatCenter( const int mgid, const int32 groups, string & host, string & port)
{
	log_debug("CDBOperator::getChatCenter. GROUPS:=%d, MGID:=%d.", groups, mgid);
	//string 
	QueryResult* result =  m_pMysql->PQuery("USE `%s`;SELECT `svip`, `svport` FROM `server_reconstruct` "
		" WHERE `svtype` = 9 AND mgid = %d AND groups=%d;", "windones_sysdata", mgid, groups);
	if(result != NULL)
	{	
		host	= result->GetItemString(0, "svip");
		port	= result->GetItemString(0, "svport");

		log_error("CDBOperator::getChatCenter. GROUPS:=%d, HOST:=%s.PORT:=%s.", groups, host.c_str(), port.c_str());

		return true;
	}
	else
	{
		log_error("CDBOperator::getChatCenter. FAILED GROUPS:=%d.", groups);

		return false;
	}
}

bool CDBOperator::getLastASVersion( const int32 /*svid*/, const int32 groups, string & version )
{
	QueryResult* result = m_pMysql->PQuery("SELECT `version` FROM `windones_sysdata`.`version` "
		" WHERE `windones_sysdata`.`version`.`groups` = %d "
		" ORDER BY `windones_sysdata`.`version`.`time` DESC LIMIT 1;", groups);
	if(result != NULL)
	{	
		version	= result->GetItemString(0, "version");

		log_error("CDBOperator::getLastASVersion. GROUPS:=%d, VERSION:=%s.", groups, version.c_str());

		return true;
	}
	else
	{
		log_error("CDBOperator::getLastASVersion. FAILED GROUPS:=%d.", groups);

		return false;
	}
}

void CDBOperator::updateTask( const uint32 uuid, const TaskInfo & task, bool bOnlyMemcached )
{

}

void CDBOperator::updateTask( const uint32 uuid, const string & key, const string & value )
{

}

bool CDBOperator::getUseTaskInfo( const uint32 uuid, TaskInfos_type & tasks )
{

}

void CDBOperator::getTaskInfoFromDB( const uint32 uuid, const uint32 taskid, TaskInfos_type & tasks )
{

}

void CDBOperator::UpdateTaskToMem( const uint32 uuid, const uint32 taskid, const string & tasks )
{

}

bool CDBOperator::getParserSubTasksInfo( const string & subTask, TaskInfo & task, TaskInfos_type & tasks )
{

}

void CDBOperator::ParserTaskInfo( const uint32 uuid, const uint32 taskid, TaskInfos_type & tasks )
{

}

bool CDBOperator::ParserSubTaskInfo( const string & str, TaskInfo & task )
{

	return false;
}

bool CDBOperator::getTaskDefine( TaskDefines_type & tasks )
{
	return true;
}

// 调用PHPAPI
bool CDBOperator::ExePhpAPI( const string & url,const string & param/*,  Json::Value & json_object*/)
{
  //  CURL *curl;
  //  string buffer;												//返回的Json字串
  //  curl_global_init(CURL_GLOBAL_ALL);							//初始化所有可能的调用
  //  curl = curl_easy_init();									//初始化一个CURL类型的指针
  //  curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
  //  curl_easy_setopt(curl,CURLOPT_POST,1);
  //  curl_easy_setopt(curl,CURLOPT_COPYPOSTFIELDS,param.c_str());

  //  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writer);

  //  curl_easy_setopt(curl,CURLOPT_WRITEDATA,&buffer);
  //  curl_easy_setopt(curl,CURLOPT_USERAGENT, "libcurl-agent/1.0");
  //  curl_easy_perform(curl);
  //  curl_easy_cleanup(curl);
  //  curl_global_cleanup();

  //  if(buffer.find("ret") == string::npos)
  //  {
		//log_error("ExePhpAPI:%s,%s",url.c_str(),param.c_str());
  //      log_error("JSON STRING IS NOT COMPLETED buffer:=%s",buffer.c_str());
  //      return false;
  //  }

  //  Json::Reader reader;
  //  Json::Value json_object;

  //  if (!reader.parse(buffer,json_object))
  //  {
		//log_error("call PARSE JSON FAIED.PHP RETURN:=%s.", buffer.c_str());
  //      return false;
  //  }

  //  if(json_object.size() < 1 )
  //  {
  //      log_error(" JSON STRING SIZE TOO SMALL xValue:=%s",buffer.c_str());
  //      return false;
  //  }

  //  int ret = JsonToInt(json_object,"ret");//Json字串解析后的结果，0表示调用PHP接口正确

  //  if(ret != 0)//调用失败
  //  {
		//log_error("CALL PHP API FAILED URL:=%s,\nDATA:=%s,\nRETURN:=%s.", url.c_str(),param.c_str(), buffer.c_str());
  //      return false;
  //  }
  //  else
  //  {
  //      log_debug("%s CALL PHP API SUCCESS", __FUNCTION__);
  //  }

    return true;
}

int CDBOperator::writer(char *data, size_t size, size_t nmemb, string *writerData)
{
    unsigned long sizes = size * nmemb;

    if( writerData == NULL ) 
    {
        return 0;
    }
    writerData->append(data, sizes);
    return sizes;
}

void CDBOperator::useProps_( const uint32 uuid, const uint32 props, const int32 count)
{

}

// 从数据库读取双倍时间信息到DoubleExp
bool CDBOperator::getPromoteTime( map<uint32, map<uint32, uint32> > &PromoteTime)
{

    return true;
}

CDBOperator::CDBOperator()
:m_pMysql(NULL)
{	
}

CDBOperator::~CDBOperator()
{
    ShutDown();
}

bool CDBOperator::GetGoodsBySid(const int32 gsid, GoodsStruct_t_list & goodslists)
{
	goodslists.clear();
	QueryResult *result = m_pMysql->PQuery("SELECT  `cid`,  `sname`,  `size`,  `position`,  `isupgrate`, "
		"`nums`,  `issell`,  `ishouse`,  `people`,  `consumeTime`, `buildResource`, `requireBaseLevel` FROM `GoodsList` WHERE `Sid`=%d;", gsid);

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		log_error("CDBOperator::GetGoodsBySid. ASSERT(!result || result->RowCount() == 0).");
		return false;
	}

	for( unsigned long i = 0; i <  result->RowCount(); ++i)
	{
		GoodsStruct_t s;

		s.cid			= result->GetItemLong( i, "cid" );
		s.sname			= result->GetItemString( i, "sname" );
		s.size			= result->GetItemString( i, "size" );
		s.position		= result->GetItemString( i, "position" );
		s.isupgrate		= result->GetItemLong( i, "isupgrate" );
		s.nums			= result->GetItemLong( i, "nums" );
		s.issell		= result->GetItemLong( i, "issell" );
		s.ishouse		= result->GetItemLong( i, "ishouse" );
		s.people		= result->GetItemLong( i, "people" );
		s.consumeTime	= result->GetItemLong( i, "consumeTime" );
		s.buildResource	= result->GetItemLong( i, "buildResource" );	
		s.requireBaseLevel	= result->GetItemLong( i, "requireBaseLevel" );

		log_debug("the goodls name is %s, the position %s, the consumeTime is %d\n", s.sname.c_str(), s.position.c_str(), s.consumeTime);

		goodslists.push_back(s);
	}
	return true;
}

bool CDBOperator::GetLoginReply(const int gameid, NETOutputPacket & pPacket)
{
	QueryResult *result = m_pMysql->PQuery("SELECT  * FROM `Player_Account` WHERE `mid`=%d;", gameid);
	ReplyType_login reply = g_GetConfigMgr().getReplyConfig();

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		pPacket.Begin(reply.cmd);
		pPacket.WriteInt(1);
		pPacket.End();
		log_error("CDBOperator::GetLoginReply. ASSERT(!result || result->RowCount() == 0).");
		return false;
	}

	// update login time
	long int mtime = GetUnixTime();

	char stime[100] = {0};
	sprintf(stime, "%ld", mtime);

	string mmtime;
	mmtime = stime;

	pPacket.Begin(reply.cmd);
	pPacket.WriteInt(0);
	int slen = reply.data.size();
	for (int i = 0; i < slen; i++)
	{
		const char *rtype = reply.data[i].type.c_str();
		switch (rtype[0]) 
		{
		case 'i'://int
			//log_debug("packet the int is %d, %s\n", result->GetItemLong( i, reply.data[i].dbitem ), reply.data[i].dbitem.c_str());
			pPacket.WriteInt(result->GetItemLong( 0, reply.data[i].dbitem ));
			break;
		case 'g'://string
			pPacket.WriteString(result->GetItemString( 0, reply.data[i].dbitem ));
			break;
		default:
			break;
		}
	}
	pPacket.WriteString(mmtime);

	// update login time two tables

	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `Player_Account` SET `mtime` = '%d' WHERE `mid` = '%d';", mtime, gameid);
	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::Player_Account mtime not change.");
		}
	}

	sprintf(iszSql, "UPDATE `UserRegistTable` SET `LastLoginTime` = '%d' WHERE `mid` = '%d';", mtime, gameid);
	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::UserRegistTable mtime not change.");
		}
	}

	return true;
}

bool CDBOperator::GetRegistReply(const int gameid, NETOutputPacket & pPacket)
{
	QueryResult *result = m_pMysql->PQuery("SELECT  * FROM `Player_Account` WHERE `mid`=%d;", gameid);
	ReplyType_login reply = g_GetConfigMgr().getRegistRely();

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		pPacket.Begin(reply.cmd);
		pPacket.WriteInt(1);
		pPacket.End();
		log_error("CDBOperator::GetRegistReply. ASSERT(!result || result->RowCount() == 0).");
		return false;
	}

	pPacket.Begin(reply.cmd);
	pPacket.WriteInt(0);
	int slen = reply.data.size();
	for (int i = 0; i < slen; i++)
	{
		const char *rtype = reply.data[i].type.c_str();
		switch (rtype[0]) 
		{
		case 'i'://int
			//log_debug("packet the int is %d, %s\n", result->GetItemLong( i, reply.data[i].dbitem ), reply.data[i].dbitem.c_str());
			pPacket.WriteInt(result->GetItemLong( 0, reply.data[i].dbitem ));
			break;
		case 'g'://string
			pPacket.WriteString(result->GetItemString( 0, reply.data[i].dbitem ));
			break;
		default:
			break;
		}
	}

	return true;
}

bool CDBOperator::GetLRegistReply(const int gameid, NETOutputPacket & pPacket)
{
	QueryResult *result = m_pMysql->PQuery("SELECT  * FROM `Player_Account` WHERE `mid`=%d;COMMIT;", gameid);
	ReplyType_login reply = g_GetConfigMgr().getReplyConfig();

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		pPacket.Begin(reply.cmd);
		pPacket.WriteInt(1);
		pPacket.End();
		log_error("CDBOperator::GetLoginReply. ASSERT(!result || result->RowCount() == 0).");
		return false;
	}

	pPacket.Begin(reply.cmd);
	pPacket.WriteInt(0);
	int slen = reply.data.size();
	for (int i = 0; i < slen; i++)
	{
		const char *rtype = reply.data[i].type.c_str();
		switch (rtype[0]) 
		{
		case 'i'://int
			//log_debug("packet the int is %d, %s\n", result->GetItemLong( i, reply.data[i].dbitem ), reply.data[i].dbitem.c_str());
			pPacket.WriteInt(result->GetItemLong( 0, reply.data[i].dbitem ));
			break;
		case 'g'://string
			pPacket.WriteString(result->GetItemString( 0, reply.data[i].dbitem ));
			break;
		default:
			break;
		}
	}

	return true;
}

bool CDBOperator::AddMessage(int & msgid, const string & receivename, const string & sendername, string & message, int messagetype, int unixtime)
{
	char iszSql[2048] = {0};
	int mstate = 2; // 未读
	sprintf(iszSql, "INSERT INTO `UserMessageTable`(`recename`, `sendername`, `message`, `time`, `type`, `state`)VALUES ('%s', '%s', '%s', '%d', '%d', '%d');", 
		receivename.c_str(), sendername.c_str(), message.c_str(), unixtime, messagetype, mstate);
	if(m_pMysql->PExecute_(iszSql))
	{
		unsigned long count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::AddMessage error.");
			msgid = 0;
			return false;
		}
		msgid = getIncreaseID();
		return true;
	}

	msgid = 0;
	return false;
}

bool CDBOperator::CheckUserInfoCollect(UserInfoMechineCollect_T & UserInfoCollect)
{
	log_debug("befaure query Info collect\n");
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserInfoCollectTable` WHERE `Gid`='%d' AND `Mid` = '%d';", UserInfoCollect.Gid, UserInfoCollect.Mid);
	log_debug("after query info collect\n");

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		// create the init base data
		char iszSql[2048] = {0};
		sprintf(iszSql, "INSERT INTO `UserInfoCollectTable`(`Gid`, `Mid`, `LeftTime`, `Status`, `NeedTime`, `Pcate`, `Pframe`)VALUES ('%d', '%d', '%d', '%d', '%d', '%d', '%d');", 
			UserInfoCollect.Gid, UserInfoCollect.Mid, UserInfoCollect.LeftTime, UserInfoCollect.Status, UserInfoCollect.NeedTime, UserInfoCollect.pcate, UserInfoCollect.pframe);
		string userbase = iszSql;
		log_debug("the user CheckUserInfoCollect is %s\n", userbase.c_str());
		if(m_pMysql->PExecute_(userbase))
		{
			long count = m_pMysql->getRows();
			if(count <= 0)
			{
				log_debug("quare error user CheckUserInfoCollect\n");
				return false;
			}
		}
		else
		{
			log_debug("quare error user user CheckUserInfoCollect\n");
			return false;
		}

		log_debug("insert the CheckUserInfoCollect id is %d, uid is %d\n", UserInfoCollect.Gid, UserInfoCollect.Mid);
	}

	UserInfoCollect.Gid = result->GetItemLong( 0, "Gid" );
	UserInfoCollect.Mid = result->GetItemLong( 0, "Mid" );
	UserInfoCollect.LeftTime = result->GetItemLong( 0, "LeftTime" );
	UserInfoCollect.NeedTime = result->GetItemLong( 0, "NeedTime" );
	UserInfoCollect.Status = result->GetItemLong( 0, "Status" );
	UserInfoCollect.pcate = result->GetItemLong( 0, "Pcate" );
	UserInfoCollect.pframe = result->GetItemLong( 0, "Pframe" );

	return true;
}

bool CDBOperator::InsertUserResourceMechineRecord(ResourceProducTable_T & Record)
{
	log_debug("befaure query resource produc\n");
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserResourceMechineProducTable` WHERE `Gid`='%d' AND `Mid` = '%d';", Record.Gid, Record.Mid);
	log_debug("after query resource produc\n");

	if (!result || result->RowCount() == 0)
	{
		char iszSql[2048] = {0};
		sprintf(iszSql, "INSERT INTO `UserResourceMechineProducTable`(`Gid`, `Pcate`, `Pmode`, `Pframe`, `Level`, `Status`, `Usetime`)VALUES ('%d', '%d', '%d', '%d', '%d', '%d', '%d');", 
			Record.Gid, Record.Pcate, Record.Pframe, Record.Pmode, Record.Level, Record.State, Record.Usetime);
		string userbase = iszSql;
		log_debug("the user CheckUserInfoCollect is %s\n", userbase.c_str());
		if(m_pMysql->PExecute_(userbase))
		{
			long count = m_pMysql->getRows();
			if(count <= 0)
			{
				log_debug("quare error user InsertUserResourceMechineRecord\n");
				return false;
			}
		}
		else
		{
			log_debug("quare error user user InsertUserResourceMechineRecord\n");
			return false;
		}
		log_debug("insert the InsertUserResourceMechineRecord id is %d, Pcate is %d\n", Record.Gid, Record.Pcate);
	}

	Record.Gid = result->GetItemLong( 0, "Gid" );
	Record.Pcate = result->GetItemLong( 0, "Pcate" );
	Record.Pframe = result->GetItemLong( 0, "Pframe" );
	Record.Pmode = result->GetItemLong( 0, "Pmode" );
	Record.Level = result->GetItemLong( 0, "Level" );
	Record.State = result->GetItemLong( 0, "State" );
	Record.Usetime = result->GetItemLong( 0, "Usetime" );
	
	return true;
}

bool CDBOperator::CheckUserBase(BaseInfo_Struct_t& Baseinfo, int gameMid)
{
	log_debug("befaure query base\n");

	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserBaseTable` WHERE `mid`='%d';", gameMid);

	log_debug("after query base\n");

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		// create the init base data
		string userbase = g_GetConfigMgr().getCreateUserBaseSql();
		log_debug("the user base is %s\n", userbase.c_str());
		if(m_pMysql->PExecute_(userbase))
		{
			long count = m_pMysql->getRows();
			if(count <= 0)
			{
				log_debug("quare error user base\n");
				return false;
			}
		}
		else
		{
			log_debug("quare error user user base\n");
			return false;
		}

		int gid = getIncreaseID();
		char iszSql[2048] = {0};
		sprintf(iszSql, "UPDATE `UserBaseTable` SET `mid` = '%d'  WHERE `Gid` = '%d';", gameMid, gid);
		log_debug("the update user mid sql is %s\n", iszSql);

		if(m_pMysql->PExecute_(iszSql))
		{
			int count = m_pMysql->getRows();
			if(count <= 0)
			{
				log_error("CDBOperator::UserChangeHeader not change.");
				return false;
			}
		}
		else
		{
			log_debug("error in chanage user base\n");
			return false;
		}
		log_debug("insert the base id is %d, uid is %d\n", gid, gameMid);
	}

	// have a record now
	Baseinfo.ibasew = result->GetItemLong( 0, "basesizex" );
	Baseinfo.ibaseh = result->GetItemLong( 0, "basesizey" );
	Baseinfo.iwallw = result->GetItemLong( 0, "wallsizex" );
	Baseinfo.iwallh = result->GetItemLong( 0, "wallsizey" );
	Baseinfo.istatus = result->GetItemLong( 0, "status" );
	Baseinfo.lefttime = result->GetItemLong( 0, "endtimer" );
	Baseinfo.iebasew = result->GetItemLong( 0, "ebasesizex" );
	Baseinfo.iebaseh = result->GetItemLong( 0, "ebasesizey" );

	return true;
}

// 获取玩家默认机器，没有则插入一条记录
bool CDBOperator::CheckUserExperimentMechine(User_Experiment_Mechine_t& UserExperimentMechine, SysMechineInfo & MechineItem, int gameMid)
{
	log_debug("befaure query CheckUserExperimentMechine\n");
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `User_ExperInstor_MechineTable` WHERE `mid`='%d' AND `pcate` = '%d' AND `pframe` = '%d';", gameMid, MechineItem.pcate, MechineItem.pframe);
	log_debug("after query CheckUserExperimentMechine\n");
	int gid = 0;
	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		// create the init base data
		char iszSql[2048] = {0};
		sprintf(iszSql, "INSERT INTO `User_ExperInstor_MechineTable`(`mid`, `pcate`, `pframe`, `position`, `level`, `curDir`, `positionx`, `positiony`, `status`, `endtime`)VALUES ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');", 
			gameMid, MechineItem.pcate, MechineItem.pframe, 1, 0, MechineItem.curLip, MechineItem.mpositionx, MechineItem.mpositiony, 0, 0);
		string userbase = iszSql;
		log_debug("the user ExperimentMechine is %s\n", iszSql);
		if(m_pMysql->PExecute_(userbase))
		{
			long count = m_pMysql->getRows();
			if(count <= 0)
			{
				log_debug("quare error user ExperimentMechine\n");
				return false;
			}
		}
		else
		{
			log_debug("quare error user user ExperimentMechine\n");
			return false;
		}

		// 创建成功
		gid = getIncreaseID();
		UserExperimentMechine.Gid = gid;
		UserExperimentMechine.pcate = MechineItem.pcate;
		UserExperimentMechine.pframe = MechineItem.pframe;
		UserExperimentMechine.position = 1;
		UserExperimentMechine.level = 0;
		UserExperimentMechine.curDir = MechineItem.curLip;
		UserExperimentMechine.positionx = MechineItem.mpositionx;
		UserExperimentMechine.positiony = MechineItem.mpositiony;
		UserExperimentMechine.status = 0;
		UserExperimentMechine.isDirty = false;
		UserExperimentMechine.endtime = 0;
		log_debug("insert the CheckUserExperimentMechine id is %d, uid is %d\n", gid, gameMid);

		return true;
	}

	// have a record now
	UserExperimentMechine.Gid = result->GetItemLong( 0, "Gid" );
	UserExperimentMechine.pcate = result->GetItemLong( 0, "pcate" );
	UserExperimentMechine.pframe = result->GetItemLong( 0, "pframe" );
	UserExperimentMechine.position = result->GetItemLong( 0, "position" );
	UserExperimentMechine.level = result->GetItemLong( 0, "level" );
	UserExperimentMechine.curDir = result->GetItemLong( 0, "curDir" );
	UserExperimentMechine.positionx = result->GetItemLong( 0, "positionx" );
	UserExperimentMechine.positiony = result->GetItemLong( 0, "positiony" );
	UserExperimentMechine.status = result->GetItemLong( 0, "status" );
	UserExperimentMechine.endtime = result->GetItemLong( 0, "endtime" );

	UserExperimentMechine.isDirty = false;

	return true;
}

// 获取玩家机器(可以多台机器)
bool CDBOperator::GetUserBuyMechineList(UserBuyMechineTable_list& UserExperimentMechine, ReturnMechineInfoTable_list  & returnMechineList, SysMechineInfo & MechineItem, int gameMid)
{
	User_Experiment_Mechine_t oneItem;
	log_debug("befaure query GetUserBuyMechineList\n");
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `User_ExperInstor_MechineTable` WHERE `mid`='%d' AND `pcate` = '%d';", gameMid, MechineItem.pcate);
	log_debug("after query GetUserBuyMechineList\n");
	int gid = 0;
	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		return false;
	}

	int len = result->RowCount();

	for (int i = 0; i < len; i++)
	{
		oneItem.Gid = result->GetItemLong( i, "Gid" );
		oneItem.pcate = result->GetItemLong( i, "pcate" );
		oneItem.pframe = result->GetItemLong( i, "pframe" );
		oneItem.position = result->GetItemLong( i, "position" );
		oneItem.level = result->GetItemLong( i, "level" );
		oneItem.curDir = result->GetItemLong( i, "curDir" );
		oneItem.positionx = result->GetItemLong( i, "positionx" );
		oneItem.positiony = result->GetItemLong( i, "positiony" );
		oneItem.status = result->GetItemLong( i, "status" );
		oneItem.endtime = result->GetItemLong( i, "endtime" );

		oneItem.isDirty = false;

		UserExperimentMechine.push_back(oneItem);

		ReturnMechineInfo returnMechine;
		returnMechine.gid = oneItem.Gid;
		returnMechine.pcate = oneItem.pcate;
		returnMechine.pframe = oneItem.pframe;
		returnMechine.name = MechineItem.name;
		returnMechine.sellprice = MechineItem.sellprice;
		returnMechine.sizex = MechineItem.sizex;
		returnMechine.sizey = MechineItem.sizey;
		returnMechine.level = oneItem.level;
		returnMechine.curDir = oneItem.curDir;
		returnMechine.mpositionx = oneItem.positionx;
		returnMechine.mpositiony = oneItem.positiony;
		returnMechine.status = oneItem.status;

		returnMechineList.push_back(returnMechine);

	}

	return true;
}

bool CDBOperator::GetUserProduction(vector<UserOrmenGoodsStruct_t> & Productionlist, MechineType_PcatePframe_T & ProductionType, int gameMid, int position)
{
	log_debug("befaure query GetUserProduction\n");
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `User_ExperInstor_MechineTable` WHERE `mid` = '%d' AND `position` = '%d' AND `pcate` = '%d';", gameMid, position, ProductionType.pcate);
	log_debug("after query GetUserProduction\n");

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		return false;
	}

	UserOrmenGoodsStruct_t oneproduction;
	for (int i = 0; i < result->RowCount(); i++)
	{
		oneproduction.pid = result->GetItemLong( i, "Gid" );
		oneproduction.mid = result->GetItemLong( i, "mid" );
		oneproduction.pcate = result->GetItemLong( i, "pcate" );
		oneproduction.pframe = result->GetItemLong( i, "pframe" );
		oneproduction.locationx = result->GetItemLong( i, "positionx" );
		oneproduction.locationy = result->GetItemLong( i, "positiony" );
		oneproduction.curDir = result->GetItemLong( i, "curDir" );
		oneproduction.position = result->GetItemLong( i, "position" );

		Productionlist.push_back(oneproduction);
	}


	return true;
}

bool CDBOperator::UpdateUserMechineTable(User_Experiment_Mechine_t& UserExperimentMechine)
{
	char iszSql[2048] = {0};

	sprintf(iszSql, "UPDATE `User_ExperInstor_MechineTable` SET `position` = '%d', `level` = '%d',`curDir` = '%d', `positionx` = '%d', `positiony` = '%d', `status` = '%d', `endtimer` = '%d' WHERE `Gid` = '%d';", 
		UserExperimentMechine.position, UserExperimentMechine.level, UserExperimentMechine.curDir, UserExperimentMechine.positionx, UserExperimentMechine.positiony, UserExperimentMechine.status, UserExperimentMechine.endtime, UserExperimentMechine.Gid);

	if(m_pMysql->PExecute_(iszSql))
	{
		UserExperimentMechine.isDirty = false;
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("UpdateUserMechineTable not change.");
			return false;
		}
		return true;
	}
	log_debug("change user UpdateUserMechineTable sql is %s\n", iszSql);
	return false;
}

bool CDBOperator::UpdateUserProducTable(ResourceProducTable_T & ResProduc)
{
	char iszSql[2048] = {0};

	sprintf(iszSql, "UPDATE `UserResourceMechineProducTable` SET `Pcate` = '%d', `Pframe` = '%d', `Pmode` = '%d', `Level` = '%d', `State` = '%d', `Usetime` = '%d', WHERE `Gid` = '%d' AND `Mid` = '%d';",
		ResProduc.Pcate, ResProduc.Pframe, ResProduc.Pmode, ResProduc.Level, ResProduc.State, ResProduc.Usetime, ResProduc.Gid, ResProduc.Mid);

	if(m_pMysql->PExecute_(iszSql))
	{
		//ResProduc.isDirty = false;
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("UpdateUserProducTable not change.");
			return false;
		}
		return true;
	}
	log_debug("change user UpdateUserProducTable sql is %s\n", iszSql);
	return false;
}

bool CDBOperator::UpdateUserInfoMechineCollectTable(UserInfoMechineCollect_T & UserInfoCollect)
{
	char iszSql[2048] = {0};

	sprintf(iszSql, "UPDATE `UserInfoCollectTable` SET `LeftTime` = '%d', `Status` = '%d',`NeedTime` = '%d' WHERE `Gid` = '%d' AND `Mid` = '%d';", 
		UserInfoCollect.LeftTime, UserInfoCollect.Status, UserInfoCollect.NeedTime, UserInfoCollect.Gid, UserInfoCollect.Mid);

	if(m_pMysql->PExecute_(iszSql))
	{
		UserInfoCollect.isDirty = false;
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("UpdateUserInfoMechineCollectTable not change.");
			return false;
		}
		return true;
	}
	log_debug("change user UpdateUserInfoMechineCollectTable sql is %s\n", iszSql);
	return false;
}

bool CDBOperator::UpdateUserResMechineProducTable(ResourceProducTable_T & UserResProduc)
{
	char iszSql[2048] = {0};

	sprintf(iszSql, "UPDATE `UserResourceMechineProducTable` SET `State` = '%d', `Usetime` = '%d', WHERE `Gid` = '%d', AND `Mid` = '%d';",
		UserResProduc.State, UserResProduc.Usetime, UserResProduc.Gid, UserResProduc.Mid);

	if(m_pMysql->PExecute_(iszSql))
	{
		UserResProduc.isDirty = false;
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("UserResourceMechineProducTable not change.");
			return false;
		}
		return true;
	}
	log_debug("change user UserResourceMechineProducTable sql is %s\n", iszSql);
	return false;
}

bool CDBOperator::UpdateUserBase(BaseInfo_Struct_t& Baseinfo, int gameMid)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `UserBaseTable` SET `basesizex` = '%d', `basesizey` = '%d',`status` = '%d', `endtimer` = '%d', `wallsizex` = '%d', `wallsizey` = '%d', `ebasesizex` = '%d' , `ebasesizey` = '%d' WHERE `mid` = '%d';", 
		gameMid, Baseinfo.ibasew, Baseinfo.ibaseh, Baseinfo.istatus, Baseinfo.lefttime, Baseinfo.iwallw, Baseinfo.iwallh, Baseinfo.iebasew, Baseinfo.iebaseh, gameMid);

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::UpdateUserBase not change.");
			return false;
		}
		return true;
	}
	log_debug("change user base sql is %s\n", iszSql);
	return false;
}

bool CDBOperator::UpdateUserMoney(int gameMid, int coin, int money)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `Player_Account` SET `money` = '%d', `coin` = '%d' WHERE `mid` = '%d';", 
		money, coin, gameMid);

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::UpdateUserMoney not change.");
			return false;
		}
		return true;
	}
	log_debug("change user money sql is %s\n", iszSql);
	return false;
}

bool CDBOperator::CheckAndAddUser(const string & IphoneMac, string& NickName, int & gameMid)
{
	QueryResult *result = m_pMysql->PQuery("SELECT `mid` FROM `IphoneRegistTable` WHERE `IphoneMac`='%s';COMMIT;", IphoneMac.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		log_error("CDBOperator::CheckAndAddUser. new iphone client.");
		char iszSql[2048] = {0};
		string newuser = g_GetConfigMgr().getCreateNewUserSql();

		if(m_pMysql->PExecute_(newuser))
		{
			unsigned long count = m_pMysql->getRows();
			if(count <= 0)
			{
				log_error("CDBOperator::CheckAndAddUser. create iphone client error.");
				return false;
			}
			// 获取mid并插入iphonetable
			int mid = getIncreaseID();	
			sprintf(iszSql, "INSERT INTO `IphoneRegistTable`(`IphoneMac`, `mid`)VALUES ('%s', %d);", 
				IphoneMac.c_str(), mid);
			if (m_pMysql->PExecute_(iszSql))
			{
				unsigned long count = m_pMysql->getRows();
				if(count <= 0)
				{
					log_error("CDBOperator::IphoneRegistTable. create iphone client error.");
					return false;
				}
			}
			gameMid = mid;

			// update nickname
			sprintf(iszSql, "UPDATE `Player_Account` SET `nickName` = '%s'  WHERE mid = %d;",
				NickName.c_str(), gameMid);
			m_pMysql->PExecute_(iszSql);
			int count2 = m_pMysql->getRows();
			if(count2 <= 0)
			{
				log_error("CDBOperator::CheckFBUser. create fb client error.");
				return false;
			}
			return true;
		}
		log_error("the sql is %s, CheckAndAddUser record ERROR\n", iszSql);
		return false;		
	}
	
	if (result->RowCount() > 1)
	{
		return false;
	}

	gameMid = result->GetItemLong( 0, "mid" );
	return true;
}

bool CDBOperator::CheckFBUser(const string & FBID, int & gameMid, string & mail, string & password, string & name)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `FacebookID`='%s';", FBID.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录 , new fb user
	{
		log_error("CDBOperator::CheckFBUser. new fb client.need to regist\n");
		return false;	
	}

	if (result->RowCount() > 1)
	{
		log_error("CDBOperator::CheckFBUser. fb client.more game id\n");
		return false;
	}

	gameMid = result->GetItemLong( 0, "mid" );
	mail = result->GetItemString( 0, "mail" );
	name = result->GetItemString( 0, "name" );

	result = m_pMysql->PQuery("SELECT * FROM `UserPassTable` WHERE `name`='%s';", name.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录 , new fb user
	{
		log_error("CDBOperator::CheckFBUser password error\n");
		return false;	
	}

	password = result->GetItemString( 0, "password" );

	return true;
}

bool CDBOperator::CheckFBUserBinding2(const string & FBID, int & gameMid, string & userNmae, string & passWord)
{
	bool flag = false;

	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `FacebookID`='%s';", FBID.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录 , new fb user
	{
		log_error("CDBOperator::CheckFBUserBinding new\n");
		flag = true;	
	}

	if (flag)
	{
		QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `name`='%s';", userNmae.c_str());

		if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录 , new fb user
		{
			log_error("CDBOperator::CheckFBUserBinding new\n");
			return true;	
		}

		gameMid = result->GetItemLong( 0, "mid" );
		passWord = result->GetItemString( 0, "PassWord" );
		return false;
	}

	return true;
}

bool CDBOperator::CheckFBUserBinding(const string & FBID, int & gameMid, string & userNmae, string & passWord)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `FacebookID`='%s';", FBID.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录 , new fb user
	{
		log_error("CDBOperator::CheckFBUserBinding new\n");
		return false;	
	}

	gameMid = result->GetItemLong( 0, "mid" );
	userNmae = result->GetItemString( 0, "name" );
	passWord = result->GetItemString( 0, "PassWord" );
	return true;
}

bool CDBOperator::CheckAddFriend(string & userNmae)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `name`='%s';", userNmae.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录 ,no such user
	{
		log_error("CDBOperator::CheckAddFriend no exist\n");
		return false;	
	}
	
	if (result->RowCount() > 1)
	{
		log_error("CDBOperator::CheckAddFriend.client.more username\n");
		return false;
	}
	return true;
}

bool CDBOperator::GetUserGameInfo(int gameId, UserGameInfo_Struct_t & UserGameInfo)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `Player_Account` WHERE `mid`='%d';", gameId);

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录 ,no such user
	{
		log_error("CDBOperator::GetUserGameInfo no exist\n");
		return false;	
	}

	if (result->RowCount() > 1)
	{
		log_error("CDBOperator::GetUserGameInfo.client.more username\n");
		return false;
	}

	UserGameInfo.coin = result->GetItemLong( 0, "coin" );
	UserGameInfo.money = result->GetItemLong( 0, "money" );
	UserGameInfo.level = result->GetItemLong( 0, "level" );
	return true;
}

bool CDBOperator::BindFBuser(const string & FBID, int & gameMid, string & userNmae, string & passWord)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `UserRegistTable` SET `FacebookID` = '%s'  WHERE `name` = '%s';", FBID.c_str(), userNmae.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::BindFBuser. set fb error.");
			return false;
		}
		return true;
	}
	return false;
}

bool CDBOperator::UserChangeHeader(const string & userNmae, int htype)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `Player_Account` SET `fbheadID` = '%d'  WHERE `nickName` = '%s';", htype, userNmae.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::UserChangeHeader not change.");
		}
		return true;
	}
	return false;
}

bool CDBOperator::UpdateMessage( string & friendName, string & userNmae, int flag)
{
	char iszSql[2048] = {0};
	if (flag == 0)
	{
		// delete the message
		sprintf(iszSql, "delete from `UserMessageTable` where `recename` = '%s' AND `sendername` = '%s';", userNmae.c_str(), friendName.c_str());

		if(m_pMysql->PExecute_(iszSql))
		{
			unsigned long count = m_pMysql->getRows();
			if(count <= 0)
			{
				log_error("CDBOperator::UpdateMessage delete error.");
				return false;
			}
			return true;
		}
	}
	else
	{
		sprintf(iszSql, "UPDATE `UserMessageTable` SET `state` = '%d'  WHERE `recename` =  '%s' AND `sendername` = '%s';", flag, userNmae.c_str(), friendName.c_str());

		if(m_pMysql->PExecute_(iszSql))
		{
			int count = m_pMysql->getRows();
			if(count <= 0)
			{
				log_error("CDBOperator::UpdateMessage error.");
				return false;
			}
			return true;
		}
	}

	return false;
}

bool CDBOperator::CreateNewUser(string& NickName, int & gameMid)
{
	char iszSql[2048] = {0};
	string newuser = g_GetConfigMgr().getCreateNewUserSql();
	log_debug("create new user , sql is %s\n", newuser.c_str());
	if(m_pMysql->PExecute_(newuser))
	{
		long count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::CreateNewUser. create iphone client error.");
			gameMid = 0;
			return false;
		}
		int mid = getIncreaseID();	
		gameMid = mid;
		// update nickname
		sprintf(iszSql, "UPDATE `Player_Account` SET `nickName` = '%s'  WHERE mid = %d;",
			NickName.c_str(), gameMid);
		m_pMysql->PExecute_(iszSql);
		long count2 = m_pMysql->getRows();
		log_debug("update name is %d\n", count2);
		return true;
	}
	gameMid = 0;
	log_error("DB CreateNewUser ERROR! the sql is %s\n", newuser.c_str());
	return false;

	//QueryResult *result = m_pMysql->PQueryS(newuser);

	//if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	//{

	//	log_error("CDBOperator::CreateNewUser. create user client error.");
	//	gameMid = 0;
	//	return false;		
	//}

	//if (result->RowCount() > 1)
	//{
	//	log_error("CDBOperator::CreateNewUser. create iphone client error.");
	//	gameMid = 0;
	//	return false;
	//}
	//int mid = getIncreaseID();	
	//gameMid = mid;
	//// update nickname
	//sprintf(iszSql, "UPDATE `Player_Account` SET `nickName` = '%s'  WHERE mid = %d;",
	//	NickName.c_str(), gameMid);
	//m_pMysql->PExecute_(iszSql);

	//return true;
}

bool CDBOperator::CheckUser(const string & UserName, const string & md5password, int & gameMid)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `name`= '%s'AND PassWord = '%s';", UserName.c_str(), md5password.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{

		log_error("CDBOperator::CheckUser. no such client.");

		return false;		
	}

	if (result->RowCount() > 1)
	{
		return false;
	}

	gameMid = result->GetItemLong( 0, "mid" );
	return true;
}

bool CDBOperator::GetUserMailPassword(string & name, string & mail, string & password)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserPassTable` WHERE `name` = '%s';", name.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{

		log_error("CDBOperator::GetUserMailPassword. no such client.");
		return false;		
	}

	if (result->RowCount() > 1)
	{
		log_error("CDBOperator::GetUserMailPassword.2 no such client.");
		return false;
	}

	password = result->GetItemString( 0, "password" );
	mail = result->GetItemString(0, "mail");
	log_debug("the mail is %s,password is %s\n", mail.c_str(), password.c_str());
	return true;

	//char iszSql[2048] = {0};
	//sprintf(iszSql, "SELECT * FROM `UserPassTable` WHERE `name` = '%s';", name.c_str());
	//log_debug("the find mail sql is %s\n", iszSql);
	//if(m_pMysql->PExecute_(iszSql))
	//{
	//	int count = m_pMysql->getRows();
	//	if(count <= 0)
	//	{
	//		log_error("CDBOperator::GetUserMailPassword.can't find the user.\n");
	//		return false;
	//	}
	//	password = m_pMysql->m_store->GetItemString(0, "password");
	//	mail = m_pMysql->m_store->GetItemString(0, "mail");
	//	return true;
	//}

	//return false;	
}
bool CDBOperator::GetUSerGid(const string & UserName, int & gameMid)
{
	QueryResult *result = m_pMysql->PQuery("SELECT `mid` FROM `Player_Account` WHERE `nickName`= '%s';COMMIT;", UserName.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{

		log_error("CDBOperator::CheckUser. no such client.");
		gameMid = 0;
		return false;		
	}

	if (result->RowCount() > 1)
	{
		gameMid = 0;
		return false;
	}

	gameMid = result->GetItemLong( 0, "mid" );
	return true;
}

bool CDBOperator::CheckAndAddRegistUser(string& NickName, int & gameMid)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `name`='%s';", NickName.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		log_error("CDBOperator::AddRegistUser. create new client");
		return false;
	}

	log_error("CDBOperator::AddRegistUser. create old client");
	return true;


	//char s_sql[1024];
	//sprintf(s_sql, "SELECT * FROM `UserRegistTable` WHERE `name`='%s';", NickName.c_str());
	//log_debug("before regist sql is %s\n", s_sql);

	//if(m_pMysql->PExecute_(s_sql))
	//{
	//	int count = m_pMysql->getRows();
	//	if(count <= 0)
	//	{
	//		log_error("CDBOperator::AddRegistUser. create new client");
	//		return false;
	//	}
	//	log_debug("CheckAndAddRegistUser the count is %d\n", count);
	//	return true;
	//}

	//log_debug("after regist sql is %s\n", s_sql);
	////QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `name`='%s';", NickName.c_str());
	////if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录, 可以注册
	////{
	////	log_error("CDBOperator::CheckAndAddRegistUser. new iphone client.");
	////	return true;		
	////}

	//return true;
}

bool CDBOperator::AddRegistUser(regist_Info_t & userInfo)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "INSERT INTO `UserRegistTable`(`mail`, `PassWord`, `name`, `mid`, `ispub`)VALUES ('%s', '%s', '%s', '%d', '%d');", 
		userInfo.email.c_str(), userInfo.md5password.c_str(), userInfo.name.c_str(), userInfo.gameMid, userInfo.isPub);

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::AddRegistUser. create new client error.");
			return false;
		}
	}

	sprintf(iszSql, "INSERT INTO `UserPassTable`(`mail`, `password`, `name`)VALUES ('%s', '%s', '%s');", 
		userInfo.email.c_str(), userInfo.password.c_str(), userInfo.name.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::AddRegistUser. create new client error.");
			return false;
		}
		return true;
	}

	return false;		
}

bool CDBOperator::ChangeUserPassword(string & name, string & md5password, string & password)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `UserRegistTable` SET `PassWord` = '%s'  WHERE `name` = '%s';", md5password.c_str(), name.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::ChangeUserPassword. set md5password error.");
			return false;
		}
	}

	sprintf(iszSql, "UPDATE `UserPassTable` SET `password` = '%s'  WHERE `name` = '%s';", 
		password.c_str(), name.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("ChangeUserPassword. set password error.");
			return false;
		}
		return true;
	}

	return false;		
}

bool CDBOperator::ChangeUserEmail(string & name, string & email, int isPub)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `UserRegistTable` SET `mail` = '%s', `ispub` = '%d'  WHERE `name` = '%s';", email.c_str(), isPub, name.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::ChangeUserEmail. set email error.");
			return false;
		}
	}

	sprintf(iszSql, "UPDATE `UserPassTable` SET `mail` = '%s'  WHERE `name` = '%s';", 
		email.c_str(), name.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("ChangeUserEmail. set password error.");
			return false;
		}
		return true;
	}

	return false;		
}

bool CDBOperator::AddFriend(string & FriendName, string & myName)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "INSERT INTO `MemberShip`(`UserName`, `FriendName`)VALUES ('%s', '%s');", 
		myName.c_str(), FriendName.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		unsigned long count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::AddFriend error.");
			return false;
		}
	}

	sprintf(iszSql, "INSERT INTO `MemberShip`(`UserName`, `FriendName`)VALUES ('%s', '%s');", 
		FriendName.c_str(), myName.c_str());
	if(m_pMysql->PExecute_(iszSql))
	{
		unsigned long count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::AddFriend error.");
			DelFriend(FriendName, myName);
			return false;
		}

		return true;
	}
	return false;		
}

bool CDBOperator::GetFriendList(string & MyName, vector<string> & FriendNameList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `MemberShip` WHERE `UserName`='%s';", MyName.c_str());

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}
	for (int i = 0; i < len; i++)
	{
		string fname = result->GetItemString( i, "FriendName" );
		FriendNameList.push_back(fname);
	}
	return true;
}

bool CDBOperator::GetFriendInfo(string fName, FriendInfo & FriendInfoList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `name`='%s';", fName.c_str());

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}
	string name = result->GetItemString( 0, "name" );
	string mail = result->GetItemString( 0, "mail" );
	int LastLoginTime = result->GetItemLong( 0, "LastLoginTime" );
	int HeadImage = result->GetItemLong( 0, "HeadImage" );
	string fbids = result->GetItemString( 0, "FacebookID" );

	FriendInfoList.name = name;
	FriendInfoList.mail = mail;
	FriendInfoList.LastLoginTime = LastLoginTime;
	FriendInfoList.headimage = HeadImage;
	FriendInfoList.fbid = fbids;

	return true;
}

bool CDBOperator::GetUserHeadimageIndex(int gameid, short & headimageindex, string & fbid)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `mid`='%d';", gameid);

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	headimageindex = result->GetItemLong( 0, "HeadImage" );
	fbid = result->GetItemString( 0, "FacebookID" );
	return true;
}

bool CDBOperator::GetUserHeadimageIndex_notRegist(int gameid, short & headimageindex)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `Player_Account` WHERE `mid`='%d';", gameid);

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	headimageindex = result->GetItemLong( 0, "fbheadID" );
	return true;
}

bool CDBOperator::GetFBID_by_userName(string & UserName, string & FBID)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserRegistTable` WHERE `name`='%s';", UserName.c_str());

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		log_error("get user fbid error, but he has fb header\n");
		return false;	
	}
	FBID = result->GetItemString( 0, "FacebookID" );

	return true;
}

bool CDBOperator::UserBuyProduction(UserOrmenGoodsStruct_t & production, int & gid)
{
	gid = 0;
	char iszSql[2048] = {0};

	MechineType_PcatePframe_T InfoMechinePcate = g_GetConfigMgr().getMechine_Info_Type();
	MechineType_PcatePframe_T ResourceMechinePcate = g_GetConfigMgr().getMechine_Resource_Type();
	
	// 如果是情报机或资源机要插入建筑时间TODO
	if (production.pcate == InfoMechinePcate.pcate || production.pcate == ResourceMechinePcate.pcate)
	{
		int time = g_GetConfigMgr().getComStructTimeBy_Pcate_Pframe(production.pcate, production.pframe) + GetUnixTime();
		int status = 1; // 建筑中
		sprintf(iszSql, "INSERT INTO `User_ExperInstor_MechineTable`(`mid`, `pcate`, `pframe`, `position`, `level`, `curDir`, `positionx`, `positiony`, `status`, `endtime`)VALUES ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');", 
			production.mid, production.pcate, production.pframe, 1, 1, production.curDir, production.locationx, production.locationy, status, time);
	}
	else
	{
		sprintf(iszSql, "INSERT INTO `User_ExperInstor_MechineTable`(`mid`, `pcate`, `pframe`, `position`, `level`, `curDir`, `positionx`, `positiony`, `status`, `endtime`)VALUES ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');", 
			production.mid, production.pcate, production.pframe, 1, 1, production.curDir, production.locationx, production.locationy, 0, 0);
	}

	if(m_pMysql->PExecute_(iszSql))
	{
		unsigned long count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::UserBuyProduction error.");
			return false;
		}
		gid = getIncreaseID();
		return true;
	}

	log_error("UserBuyProduction error, the sql is %d\n", iszSql);
	return false;
}

bool CDBOperator::DelUserProduction(int pid)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "delete from `User_ExperInstor_MechineTable` where `Gid` = '%d';", pid);

	if(m_pMysql->PExecute_(iszSql))
	{
		unsigned long count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::DelUserProduction error.");
			return false;
		}
		return true;
	}

	return false;		
}

// 修改物品的属性
bool CDBOperator::ChangeUserProductionDirLocation(int pid, int curDir, int locationx, int locationy)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `User_ExperInstor_MechineTable` SET `curDir` = '%d', `positionx` = '%d', `positiony` = '%d'  WHERE `Gid` = '%d';", curDir, locationx, locationy, pid);

	log_debug("the ChangeUserProductionDirLocation iszSql is %s\n", iszSql);

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::ChangeUserProductionDirLocation error.");
			return false;
		}

		return true;
	}
	return false;		
}

// 修改物品的属性
bool CDBOperator::ChangeUserProductionHouse(int pid, int position)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "UPDATE `User_ExperInstor_MechineTable` SET `position` = '%d'  WHERE `Gid` = '%d';", position, pid);

	if(m_pMysql->PExecute_(iszSql))
	{
		int count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::ChangeUserProductionHouse error.");
			return false;
		}

		return true;
	}
	return false;		
}

bool CDBOperator::GetProduction_Type_by_Pid(int pid, int & pcate, int & pframe)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `User_ExperInstor_MechineTable` WHERE `Gid`='%d';", pid);

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		log_error("get GetProduction_Type_by_Pid error \n");
		return false;	
	}

	pcate = result->GetItemLong( 0, "Pcate" );
	pframe = result->GetItemLong( 0, "Pframe" );

	return true;
}

// 只获取未读消息
bool CDBOperator::GetMessageInfoList(string myName, vector<UserMessageInfo> & UserMessageList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `UserMessageTable` WHERE `recename`='%s';", myName.c_str());

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		UserMessageInfo message;

		message.msgID = result->GetItemLong( i, "messageid" );
		message.senderName = result->GetItemString( i, "sendername" );
		message.content = result->GetItemString( i, "message" );
		message.messagetype = result->GetItemLong( i, "type" );
		message.time = result->GetItemLong( i, "time" );
		message.state = result->GetItemLong( i, "state" );

		UserMessageList.push_back(message);
	}

	return true;
}

bool CDBOperator::GetSystemMessageInfoList(vector<SystemMessageInfo> & SystemMessageList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `SystemMessage`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		SystemMessageInfo message;

		message.msgID = result->GetItemLong( i, "messageid" );
		message.title = result->GetItemString( i, "title" );
		message.content = result->GetItemString( i, "message" );
		message.messagetype = result->GetItemLong( i, "type" );
		message.time = result->GetItemLong( i, "time" );

		SystemMessageList.push_back(message);
	}

	return true;
}

bool CDBOperator::GetSystemOrmenGoodsList(OrmenGoodsStruct_t_list & OrmenGoodsList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `OrnamentTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		OrmenGoodsStruct_t ormengood;

		ormengood.pcate = result->GetItemLong( i, "Pcate" );
		ormengood.pframe = result->GetItemLong( i, "Pframe" );
		ormengood.name = result->GetItemString( i, "name" );
		ormengood.type = result->GetItemLong( i, "type" );
		ormengood.sizex = result->GetItemLong( i, "sizex" );
		ormengood.sizey = result->GetItemLong( i, "sizey" );
		ormengood.coin = result->GetItemLong( i, "coin" );
		ormengood.money = result->GetItemLong( i, "money" );
		ormengood.nlevel = result->GetItemLong( i, "nlevel" );
		ormengood.crossflag = result->GetItemLong( i, "crossFlag" );
		ormengood.info = result->GetItemString( i, "info" );
		ormengood.isCanSell = result->GetItemLong( i, "isCanSell" );
		ormengood.sellCoin = result->GetItemLong( i, "sellCoin" );

		OrmenGoodsList.push_back(ormengood);
	}

	return true;
}

bool CDBOperator::GetSystemEnlargeTableList(EnlargeTable_list & EnlargeTableList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `BaseEnlargeTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		EnlargeTableStruct_t enlargeitem;

		enlargeitem.Gid = result->GetItemLong( i, "Gid" );
		enlargeitem.nlevel = result->GetItemLong( i, "nlevel" );
		enlargeitem.coin = result->GetItemLong( i, "coin" );
		enlargeitem.money = result->GetItemLong( i, "money" );
		enlargeitem.sizex = result->GetItemLong( i, "sizex" );
		enlargeitem.sizey = result->GetItemLong( i, "sizey" );
		enlargeitem.ctime = result->GetItemLong( i, "ctime" );
		enlargeitem.mtime = result->GetItemLong( i, "mtime" );

		EnlargeTableList.push_back(enlargeitem);
	}

	return true;
}

bool CDBOperator::GetMoreGameInfoList(vector<GameListInfo> & MoreGameinfoList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `GameListTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		GameListInfo message;

		message.gameid = result->GetItemLong( i, "GameID" );
		message.url = result->GetItemString( i, "GameURL" );
		message.iden = result->GetItemString( i, "GameIden" );
		message.icon = result->GetItemString( i, "GameIcon" );

		MoreGameinfoList.push_back(message);
	}

	return true;
}

bool CDBOperator::GetMechineList(SysMechineTable_list & systemMechineList, OrmenGoodsStruct_t_list & OrmenGoodsList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `MechineTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		SysMechineInfo MechineItem;

		MechineItem.pcate = result->GetItemLong( i, "pcate" );
		MechineItem.pframe = result->GetItemLong( i, "pframe" );
		MechineItem.name = result->GetItemString( i, "name" );
		MechineItem.sizex = result->GetItemLong( i, "sizex" );
		MechineItem.sizey = result->GetItemLong( i, "sizey" );
		MechineItem.coinprice = result->GetItemLong( i, "coinprice" );
		MechineItem.isCanUpLevel = result->GetItemLong( i, "isCanUpLevel" );
		MechineItem.isCanSell = result->GetItemLong( i, "isCanSell" );
		MechineItem.isCanStore = result->GetItemLong( i, "isCanStore" );
		MechineItem.BuyLevel = result->GetItemLong( i, "BuyLevel" );
		MechineItem.limit = result->GetItemLong( i, "limit" );
		MechineItem.curLip = result->GetItemLong( i, "curLip" );
		MechineItem.mpositionx = result->GetItemLong( i, "mpositionx" );
		MechineItem.mpositiony = result->GetItemLong( i, "mpositiony" );
		MechineItem.sellprice = result->GetItemLong( i, "Sellprice" );
		MechineItem.cossflag = result->GetItemLong( i, "cossflag" );

		systemMechineList.push_back(MechineItem);

		OrmenGoodsStruct_t ormengood;

		ormengood.pcate = MechineItem.pcate;
		ormengood.pframe = MechineItem.pframe;
		ormengood.name = MechineItem.name;
		ormengood.type = 4;
		ormengood.sizex = MechineItem.sizex;
		ormengood.sizey = MechineItem.sizey;
		ormengood.coin = MechineItem.coinprice;
		ormengood.money = 0;
		ormengood.nlevel = MechineItem.BuyLevel;
		ormengood.crossflag = MechineItem.cossflag;
		ormengood.info = result->GetItemString( i, "info" );
		ormengood.isCanSell = MechineItem.isCanSell;
		ormengood.sellCoin = MechineItem.sellprice;

		OrmenGoodsList.push_back(ormengood);
	}

	return true;
}

bool CDBOperator::GetInstorUserLevelList(InstorUserLevel_list & LevelList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `InstorUserLevel`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		InstorUserLevelRelation_T LevelItem;

		LevelItem.instorLevel = result->GetItemLong( i, "InstorLevel" );
		LevelItem.userLevel = result->GetItemLong( i, "NeedUserLevel" );

		LevelList.push_back(LevelItem);
	}
	return true;
}

bool CDBOperator::GetInstorUpLevelList(InstrutorUpLevelTable_list & UpLevelList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `InstrutorUpLevelTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		InstrutorUpLevelTable_T LevelItem;

		LevelItem.Level = result->GetItemLong( i, "Level" );
		LevelItem.Time = result->GetItemLong( i, "Time" );
		LevelItem.Coin = result->GetItemLong( i, "Coin" );
		LevelItem.MoneyTime = result->GetItemLong( i, "Money" );

		UpLevelList.push_back(LevelItem);
	}
	return true;
}

bool CDBOperator::GetMechineUpLevelComsumList(MechineUpLevelComsumTable_list & UpLevelComsumList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `MechineUpLevelComSumTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		MechineUpLevelComsumTable_T LevelItem;

		LevelItem.pcate = result->GetItemLong( i, "pcate" );
		LevelItem.pframe = result->GetItemLong( i, "pframe" );
		LevelItem.Level = result->GetItemLong( i, "Level" );
		LevelItem.Time = result->GetItemLong( i, "Time" );
		LevelItem.Coin = result->GetItemLong( i, "Coin" );
		LevelItem.MoneyTime = result->GetItemLong( i, "Money" );

		UpLevelComsumList.push_back(LevelItem);
	}
	return true;
}

bool CDBOperator::GetInfoMechineCollectTableList(InfoMechineCollectTable_list & infoCollectList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `InfoMechineCollectTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		InfoMechineCollectTable_T LevelItem;

		LevelItem.pcate = result->GetItemLong( i, "Pcate" );
		LevelItem.pframe = result->GetItemLong( i, "Pframe" );
		LevelItem.Level = result->GetItemLong( i, "Level" );
		LevelItem.Time = result->GetItemLong( i, "needTime" );
		LevelItem.EarCoin = result->GetItemLong( i, "Earcoin" );
		LevelItem.MoneyTime = result->GetItemLong( i, "Mtime" );

		infoCollectList.push_back(LevelItem);
	}
	return true;
}

bool CDBOperator::GetMergeResourseTablelist(MergeResourseTable_list & ResourceTablelist)  //实现获取道具合成原材料
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `MergeResourceTable`;");

	int len = result->RowCount();
	if ( !result || 0==len )
	{
		return false;
	}

	for (int i=0; i < len; i++)
	{
		MergeResourseTable_T ResourceItem;

		ResourceItem.Gid = result->GetItemLong( i, "Gid" );
		ResourceItem.pcate = result->GetItemLong( i, "pcate" );
		ResourceItem.pframe = result->GetItemLong( i, "pframe" );

		ResourceTablelist.push_back(ResourceItem);
	}
	return true;
}

bool CDBOperator::GetMergePKTablelist(MergePKTable_list & MergePKTablelist)      //实现道具合成需要材料种类和数量
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `MergePKTable`;");

	int len = result->RowCount();
	if ( !result || 0==len )
	{
		return false;
	}

	for (int i=0; i<len; i++)
	{
		MergePKTable_T MergePKItem;

		MergePKItem.Gid = result->GetItemLong( i, "Gid" );
		MergePKItem.Pcate = result->GetItemLong( i, "Pcate" );
		MergePKItem.Pframe = result->GetItemLong( i, "Pframe" );
		MergePKItem.Level = result->GetItemLong( i, "NLevel");
		MergePKItem.Tid1 = result->GetItemLong( i, "Tid1" );
		MergePKItem.Nums1 = result->GetItemLong( i, "Nums1" );
		MergePKItem.Tid2 = result->GetItemLong( i, "Tid2" );
		MergePKItem.Nums2 = result->GetItemLong( i, "Nums2" );
		MergePKItem.Tid3 = result->GetItemLong( i, "Tid3" );
		MergePKItem.Nums3 = result->GetItemLong( i, "Nums3" );
		MergePKItem.Tid4 = result->GetItemLong( i, "Tid4" );
		MergePKItem.Nums4 = result->GetItemLong( i, "Nums4" );

		MergePKTablelist.push_back(MergePKItem);
	}
	return true;
}

//获取能源机生产方式表信息
bool CDBOperator::GetResourceMechineProductypeList(ResourceMechineProductype_list & ResourceMechineProductypeList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `ResourceMechineLimitTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;
	}

	for (int i = 0; i < len; i++)
	{
		ResourceMechineProductype_T ResourceItem;

		ResourceItem.Pcate = result->GetItemLong( i, "Pcate" );
		ResourceItem.Pframe = result->GetItemLong( i, "Pframe" );
		ResourceItem.Level = result->GetItemLong( i, "Level" );
		ResourceItem.NeedTime = result->GetItemLong( i, "NeedTime" );
		ResourceItem.Coin = result->GetItemLong( i, "Coin" );
		ResourceItem.Power = result->GetItemLong( i, "Power" );
		ResourceItem.UserLevel = result->GetItemLong( i, "UserLevel" );
		ResourceItem.Mtime = result->GetItemLong( i, "Mtime" );
		
		ResourceMechineProductypeList.push_back(ResourceItem);
	}
	return true;
}


bool CDBOperator::GetMechineComStructTimeList(MechineComStructTimeTable_List & ComStructTimeList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `MechineComStructTimeTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		MechineComStructTimeTable_T ComItem;

		ComItem.pcate = result->GetItemLong( i, "pcate" );
		ComItem.pframe = result->GetItemLong( i, "pframe" );
		ComItem.Time = result->GetItemLong( i, "comstructtime" );

		ComStructTimeList.push_back(ComItem);
	}
	return true;
}

bool CDBOperator::GetPowerMechineUpLevelList(PowerMechineUpLevelTable_list & UpLevelList)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `PowerMechineUpLevelTable`;");

	int len = result->RowCount();
	if ( !result || len == 0 )
	{
		return false;	
	}

	for (int i = 0; i < len; i++)
	{
		PowerMechineUpLevelTable_T LevelItem;

		LevelItem.Level = result->GetItemLong( i, "Level" );
		LevelItem.Time = result->GetItemLong( i, "NeedTime" );
		LevelItem.Coin = result->GetItemLong( i, "Coin" );
		LevelItem.MoneyTime = result->GetItemLong( i, "Mtime" );

		LevelItem.PowerLimitA = result->GetItemLong( i, "PowerLimitA" );
		LevelItem.PowerLimitB = result->GetItemLong( i, "PowerLimitB" );
		LevelItem.PowerLimitC = result->GetItemLong( i, "PowerLimitC" );
		LevelItem.PowerLimitD = result->GetItemLong( i, "PowerLimitD" );
		LevelItem.PowerLimitE = result->GetItemLong( i, "PowerLimitE" );

		UpLevelList.push_back(LevelItem);
	}
	return true;
}

bool CDBOperator::CheckIsAFriend(string & FriendName, string & myName)
{
	QueryResult *result = m_pMysql->PQuery("SELECT * FROM `MemberShip` WHERE `UserName`='%s'AND `FriendName` = '%s';", myName.c_str(), FriendName.c_str());

	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录 ,no such user
	{
		return false;	
	}
	return true;
}

bool CDBOperator::DelFriend(string & FriendName, string & myName)
{
	char iszSql[2048] = {0};
	sprintf(iszSql, "delete from `MemberShip` where `UserName` = '%s' and `FriendName` = '%s';", 
		myName.c_str(), FriendName.c_str());

	if(m_pMysql->PExecute_(iszSql))
	{
		unsigned long count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_error("CDBOperator::DelFriend error.");
			return false;
		}
		return true;
	}

	return false;		
}

int CDBOperator::getIncreaseID()
{
	return m_pMysql->getIncreaseID();
}

bool CDBOperator::GetServerList(const int mgid, ServerStateList_type & servers )
{
	servers.clear();

	// TODO: just return
	return true;

	QueryResult *result = m_pMysql->PQuery("SELECT  `svid`,  `svtype`,  `svip`,  `svport`,  `maxload`, "
		"`startRoomId`,  `endRoomId`,  `subtype`,  `mgid`,  `groups` FROM `windones_sysdata`.`server_reconstruct` WHERE `mgid`=%d;", mgid);
	if ( !result || result->RowCount() == 0 ) // 如果没有满足该查询条件的记录
	{
		log_error("CDBOperator::GetServerList. ASSERT(!result || result->RowCount() == 0).");
		return false;
	}

	for( unsigned long i = 0; i <  result->RowCount(); ++i)
	{
		ServerState_type s;

		s.id			= result->GetItemLong( i, "svid" );
		s.type			= result->GetItemLong( i, "svtype" );
		s.ip			= result->GetItemString( i, "svip" );
		s.port			= result->GetItemString( i, "svport" );
		s.begin			= result->GetItemLong( i, "startRoomId" );
		s.end			= result->GetItemLong( i, "endRoomId" );
		s.subtype		= result->GetItemLong( i, "subtype" );
		s.mgid			= result->GetItemLong( i, "mgid" );
		s.groups		= result->GetItemLong( i, "groups" );
		s.maxload		= result->GetItemLong( i, "maxload" );

		log_error("CDBOperator::GetServerList id:=%d, type:=%d, subtype:=%d, begin:=%d,end:=%d,groups:=%d,mgid:=%d,ip:=%s,port:=%s",
			s.id, s.type, s.subtype, s.begin, s.end, s.groups, s.mgid, s.ip.c_str(), s.port.c_str());

		servers.push_back(s);
	}

	return true;
}

bool CDBOperator::UpdateServerState( ServerStateList_type & servers )
{
	return true;
}

int CDBOperator::getUserWinInfo( const uint32 uuid, int & level )
{

	return 0;
}

static const int s_nLevel[] = 
{
	480,	1200,	2160,	3360,	4880,	6720,	8880,	11360,	14160,	17280,
	26880,	41280,	60480,	84480,	113280,	148480,	190080,	238080,	292480,	353280,
	506613,	673280,	853280,	1046613,1253280,1473280,1706613,1953280,2213280,2486613,
	3019947,3585280,4182613,4811947,5473280,6166613,6891947,7649280,8438613,2147483647 
};

int CDBOperator::CalcLevel( int nPoint )
{
	int nEnd;
	nEnd = sizeof(s_nLevel) / sizeof(int) - 1;

	if(nPoint < s_nLevel[0])
	{
		return 1;
	}
	else if( nPoint >= s_nLevel[nEnd] )
	{
		return nEnd + 2;
	}

	int i = 0, nBegin = 0;
	while( nEnd - nBegin > 1)
	{
		i = (nEnd - nBegin) / 2 + nBegin;
		if( nPoint < s_nLevel[i] )
		{
			nEnd = i;
		}
		else if( nPoint > s_nLevel[i] )
		{
			nBegin = i;
		}
		else
		{
			break;
		}
	}

	if( 1 == nEnd - nBegin )
	{
		i = nBegin;
	}

	return i + 2;
}

bool  CDBOperator::Get_Price_FromBD(vector<StoreProPrice_t> & goods_price)
{

	char s_sql[1024];
	string tablename = "`GoodsPrice`";
	sprintf(s_sql, "SELECT * FROM %s;", tablename.c_str());

	int sid = 0;
	int cid = 0;
	int coin = 0;
	int gem = 0;
	StoreProPrice_t temitem;

	if(m_pMysql->PQuery(s_sql))
	{
		unsigned long count = m_pMysql->getRows();
		if(count <= 0)
		{
			log_debug(" Get_Price_FromBD error, count == 0\n");
			return false;
		}
		for (int i = 0; i< count; i++)
		{
			sid = m_pMysql->m_store->GetItemLong(i, "Sid");
			cid = m_pMysql->m_store->GetItemLong(i, "cid");
			coin = m_pMysql->m_store->GetItemLong(i, "coin");
			gem = m_pMysql->m_store->GetItemLong(i, "gem");

			temitem.sid = sid;
			temitem.cid = cid;
			temitem.coin = coin;
			temitem.gem = gem;
			goods_price.push_back(temitem);
		}
		return true;
	}
	return false;
}

bool CDBOperator::Load_Reward_Table(vector<PlayerRewardLevel_t> & reward_table)
{
	
	return true;
}

bool CDBOperator::Load_Rank_Table(vector<int> & Rank_table)
{
	char s_sql[1024];
	string tablename = "`RankTable`";
	sprintf(s_sql, "SELECT * FROM %s;", tablename.c_str());

	int playermid = 0;
	int Ranking = 0;

	if(m_pMysql->PQuery(s_sql))
	{
		unsigned long count = m_pMysql->getRows();
		log_debug(" Load_Rank_Table , count == %d\n", count);
		if(count <= 0)
		{
			log_debug(" Load_Rank_Table error, count == 0\n");
			return false;
		}
		Rank_table.reserve(count + 10);
		Rank_table[0] = 0;
		for (int i = 0; i< count; i++)
		{
			playermid = m_pMysql->m_store->GetItemLong(i, "Mid");
			Ranking = m_pMysql->m_store->GetItemLong(i, "Ranking");

			if (Ranking > count + 10)
			{
				log_debug(" Load_Rank_Table error, ranking == %d\n", Ranking);
				return false;
			}
			Rank_table[Ranking] = playermid;
		}
		return true;
	}
	return false;
}

