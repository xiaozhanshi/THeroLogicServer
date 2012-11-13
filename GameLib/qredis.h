#ifndef __QREDIS_HEADER__
#define __QREDIS_HEADER__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "TimerEvent.h"
#include "hiredis.h"

typedef struct Redis
{
	char pszServer[32];
	int  nPort;
	Redis()
	{
		memset(this, 0, sizeof(Redis));
	}
}TRedis;

class qredis  : public TimerOutEvent
{
public:
	static const int TABLE_REDIS_EXPIRE_TIME;
	static qredis& instance();

public:
	int ProcessOnTimerOut(int nTimerId);
	int ProcTimeOutRedis();
	bool CheckIsConnect();
	void InitQredis();

public:
	bool Set(char *key, char *value);
	bool Del(char *key);
	int	 GetInt(char *key);
	bool GetValue(char *key, char * dest);
	bool IsExitKey(char *key);
	bool ExpireKey(char *key, int etime);
protected:
	static qredis sInstance;

private:
	qredis();
	~qredis();
private:
	TRedis m_RedisConf;						//redis服务器配置
	redisContext *m_redis;					//连接hiredis
	redisReply *reply;						//hiredis回复
	TimerEvent m_RedisTimer;				//redis定时器
};

#ifndef  g_qredis
#define  g_qredis (qredis::instance())
#endif 

#endif
