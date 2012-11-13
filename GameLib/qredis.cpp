#include <string>
#include "qredis.h"
#include "log.h"
#include "ConfigMgr.h"
#include "GameCmd.h"

using namespace std;

const int qredis::TABLE_REDIS_EXPIRE_TIME = 300;

qredis qredis::sInstance;

qredis& qredis::instance()
{
	return sInstance;
}

qredis::~qredis()
{

}

qredis::qredis()
{
	m_RedisTimer.SetTimeEventObj(this, REDIS_TIMER);
}

void qredis::InitQredis()
{
	// redis
	RedisServerConfig redisconfig = g_GetConfigMgr().getRedisConfig();
	memcpy(m_RedisConf.pszServer, redisconfig.chIP.c_str(), 31);
	m_RedisConf.nPort = redisconfig.port;

	m_redis = redisConnect(m_RedisConf.pszServer, m_RedisConf.nPort);
	if (m_redis->err)	//连接redis
	{
		log_error("Connect Redis Server failed, ip:%s, port:%d\n", m_RedisConf.pszServer, m_RedisConf.nPort);
		m_RedisTimer.StartTimer(2);
	}
	else
	{
		//StartRankKey();
		log_debug("Initredis-connect OK...\r\n");
	}
}

int qredis::ProcessOnTimerOut(int nTimerId)
{
	switch (nTimerId)
	{
	case REDIS_TIMER:
		return ProcTimeOutRedis();
	default:
		break;
	}
	return 0;
}

int qredis::ProcTimeOutRedis()
{
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "PING"));
	if (reply == NULL || strcmp(reply->str, "PONG") != 0)
	{
		log_debug(" reconnect redis-server...\r\n");
		m_redis = redisConnect(m_RedisConf.pszServer, m_RedisConf.nPort);
		if (m_redis->err)	//连接redis
		{
			//ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Connect Redis Server failed, ip:%s, port:%d\n"), m_RedisConf.pszServer, m_RedisConf.nPort), FALSE);	
			log_debug("Connect Redis Server failed, ip:%s, port:%d\n", m_RedisConf.pszServer, m_RedisConf.nPort);
			m_RedisTimer.StopTimer();
			m_RedisTimer.StartTimer(5);
		}
		else
			log_debug("redis-reconnect OK...\r\n");
	}
	if (reply!=NULL)
	{
		freeReplyObject(reply);
	}
	return 0;
}

/*************************************************************************
                       检测redis是否连接成功
**************************************************************************/
bool qredis::CheckIsConnect()
{
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "PING"));
	if (reply != NULL && strcmp(reply->str, "PONG") == 0)
	{
		freeReplyObject(reply);
		return true;		
	}

	else
	{
		if (reply == NULL)
		{
			log_debug("reply is null so over time\n");
		}
		if (reply != NULL)
		{
			freeReplyObject(reply);
		}

		log_debug("reconnect redis-server...\r\n");
		m_redis = redisConnect(m_RedisConf.pszServer, m_RedisConf.nPort);
		if (m_redis->err)	//连接redis
		{
			log_debug("Connect Redis Server failed, ip:%s, port:%d\n", m_RedisConf.pszServer, m_RedisConf.nPort);
			m_RedisTimer.StopTimer();
			m_RedisTimer.StartTimer(2);
		}
		else
		{
			log_debug("redis-reconnect OK...\r\n");
			return true;
		}
	}

	return false;
}

bool qredis::Set(char *key, char *value)
{
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "SET %s %s", key, value));
	if (reply != NULL)
	{
		log_debug("set key returned: %s || %s\n", key, reply->str);
		freeReplyObject(reply);
	}
	return true;
}

bool qredis::Del(char *key)
{
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "DEL %s", key));
	if (reply != NULL)
	{
		log_debug("del key returned: %s || %s\n", key, reply->str);
		freeReplyObject(reply);
	}

	return true;
}

int qredis::GetInt(char *key)
{

	int rec = -3;
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "EXISTS %s", key));
	if (reply != NULL)
	{
		rec = reply->integer;
		freeReplyObject(reply);
	}
	else
	{
		return 0;
	}

	if (rec <= 0)
	{
		log_debug("error in GetInt\n");
		return 0;
	}

	// spl
	int rint = 0, sid;
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "GET %s", key));
	if (reply != NULL)
	{
		sscanf(reply->str,"%d-%d",&sid,&rint);
		//int rint = atoi(reply->str);
		freeReplyObject(reply);
	}
	return rint;
}

bool qredis::IsExitKey(char *key)
{

	int rec = -3;
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "EXISTS %s", key));
	if (reply != NULL)
	{
		rec = reply->integer;
		freeReplyObject(reply);
		log_debug("the exists return value is %d", rec);
	}
	else
	{
		log_debug("error in IsExitKey\n");
		return false;
	}

	if (rec <= 0)
	{
		//log_debug("error in IsExitKey\n");
		return false;
	}

	return true;
}

bool qredis::GetValue(char *key, char * dest)
{
	int rec = -3;
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "EXISTS %s", key));
	if (reply != NULL)
	{
		rec = reply->integer;
		freeReplyObject(reply);
	}
	else
	{
		return false;
	}

	if (rec <= 0)
	{
		log_debug("error in GetInt\n");
		return false;
	}

	// spl
	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "GET %s", key));
	log_debug("get after in GetInt\n");
	if (reply != NULL)
	{
		memcpy(dest, reply->str, strlen(reply->str) + 1);
		freeReplyObject(reply);
		return true;
	}
	return false;
}

bool qredis::ExpireKey(char *key, int etime)
{
	char value[200] = {0};
	int  expire = etime;      //30天过期
	memset(value, 0, sizeof(value));
	sprintf(value, "%d", expire);

	reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "EXPIRE %s %s", key, value));
	if (reply != NULL)
	{
		log_debug("set key:%s, time = %d, return flag = %d\n", key, expire, reply->integer);
		freeReplyObject(reply);
		return true;
	}
	return false;
}