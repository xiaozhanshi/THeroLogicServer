#include "PlatFunc.h"
#include "LuckySystem.h"
#include "log.h"
#include "qredis.h"
#include "DBOperator.h"

#define  S48H 172800
#define  S24H 86400

using namespace std;

CLuckySystem CLuckySystem::sInstance;

CLuckySystem& CLuckySystem::instance()
{
	return sInstance;
}

CLuckySystem::CLuckySystem()
{

}

CLuckySystem::~CLuckySystem()
{

}

int CLuckySystem::CheckReward(int mid)
{
	// get the key如果这个key不存在，则重新设置为1个连续登陆
	// 如果这个key存在，则检测是否已经领奖，如果是否的话，怎么可以领奖，
	if (g_qredis.CheckIsConnect())
	{
		char key[100] = {0};
		sprintf(key, "USERLOGINREWARD:%d", mid); 
		bool ret = g_qredis.IsExitKey(key);
		log_debug("after isexitkey, the ret is %d, the key is %s", ret, key);
		if (!ret)
		{
			// 第一次登陆（中间有可能断开登陆）
			int continuedate = 1;
			int isgetreward = 0;
			long int mtime = GetUnixTime();//untime
			int restime = 30;//GetDelayTime();//剩余时间

			char value[200] = {0};
			sprintf(value, "%d-%d-%ld-%d", continuedate, isgetreward, mtime, restime);
			log_debug(" false before set, the value is %s", value);
			g_qredis.Set(key, value);
			log_debug(" false after set");

			// 超时设置
			g_qredis.ExpireKey(key, S48H);
			log_debug("after ExpireKey");

			// 返回可以领奖 TODO
			log_debug("the luck value is %s", value);

			return continuedate; // 返回连续登陆的天数
		}
		else
		{
			// 连续登陆情况
			char value[200] = {0};
			log_debug(" true before getValue");
			ret = g_qredis.GetValue(key, value);
			if (!ret)
			{
				log_error("can't find the key, the key is %s", key);
				// 返回不可以领奖
				return 0;
			}
			log_debug(" true after getValue");

			// check the value 并 设置 其他值
			int continuedate = 0;
			int isgetreward = 1;
			long int mtime = 0;
			int restime = 0;
			sscanf(value,"%d-%d-%ld-%d",&continuedate,&isgetreward, &mtime, &restime);

			if (mtime == 0)
			{
				// 返回不可以领奖
				log_error("the mtime == 0");
				return 0;
			}

			long int nowtime = GetUnixTime();
			log_debug("the nowtime is %ld, the mtime is %ld", nowtime, restime);
			if (nowtime - mtime <= restime && isgetreward == 1)
			{
				// 返回不可以领奖（今天已经领奖）
				return 0;
			}
			if (nowtime - mtime <= restime && isgetreward == 0)
			{
				// 返回可以领奖(今天没领奖)
				return continuedate;
			}
			if (nowtime - mtime > restime + 30)//S24H) //大于一天没登陆
			{
				int continuedate = 1;
				int isgetreward = 0;
				long int mtime = GetUnixTime();//untime
				int restime = 30;//GetDelayTime();//剩余时间

				char value[200] = {0};
				sprintf(value, "%d-%d-%ld-%d", continuedate, isgetreward, mtime, restime);
				log_debug(" false before set, the value is %s", value);
				g_qredis.Del(key);
				g_qredis.Set(key, value);
				log_debug(" false after set");

				// 超时设置
				g_qredis.ExpireKey(key, S48H);
				log_debug("after ExpireKey");

				// 返回可以领奖 TODO
				log_debug("the luck value is %s", value);

				return continuedate; // 返回连续登陆的天数
			}
			if (nowtime - mtime > restime)
			{
				// 返回可以领奖
				continuedate++;
				isgetreward = 0;
				mtime = GetUnixTime();
				restime = 30;//GetDelayTime();
				sprintf(value, "%d-%d-%d-%d", continuedate, isgetreward, mtime, restime);
				g_qredis.Del(key);
				g_qredis.Set(key, value);

				// 超时设置
				g_qredis.ExpireKey(key, S48H);

				return continuedate;
			}
		}
	}
}

bool CLuckySystem::GetReward(int mid)
{
	if (g_qredis.CheckIsConnect())
	{
		char key[100] = {0};
		sprintf(key, "USERLOGINREWARD:%d", mid);
		char value[200] = {0};
		bool ret = g_qredis.GetValue(key, value);

		if (!ret)
		{
			log_error("some thing error");
			return false;
		}

		// 第一次登陆（中间有可能断开登陆）
		int continuedate = 1;
		int isgetreward = 1;
		int mtime = 0;//untime
		int restime = 0;//剩余时间

		sscanf(value,"%d-%d-%d-%d",&continuedate,&isgetreward, &mtime, &restime);

		if (isgetreward == 1)
		{
			// 已经领奖了，返回false

			return false;
		}

		// 插入金币
		isgetreward = 1;

		sprintf(value, "%d-%d-%d-%d", continuedate, isgetreward, mtime, restime);
		g_qredis.Set(key, value);

		return true;
	}
	
	return false;
}

bool CLuckySystem::LoadRewardTable()
{
	g_GetCDBOperator().Load_Reward_Table(m_RewardTable);
	return true;
}