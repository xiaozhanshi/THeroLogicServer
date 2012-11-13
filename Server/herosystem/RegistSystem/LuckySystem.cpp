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
	// get the key������key�����ڣ�����������Ϊ1��������½
	// ������key���ڣ������Ƿ��Ѿ��콱������Ƿ�Ļ�����ô�����콱��
	if (g_qredis.CheckIsConnect())
	{
		char key[100] = {0};
		sprintf(key, "USERLOGINREWARD:%d", mid); 
		bool ret = g_qredis.IsExitKey(key);
		log_debug("after isexitkey, the ret is %d, the key is %s", ret, key);
		if (!ret)
		{
			// ��һ�ε�½���м��п��ܶϿ���½��
			int continuedate = 1;
			int isgetreward = 0;
			long int mtime = GetUnixTime();//untime
			int restime = 30;//GetDelayTime();//ʣ��ʱ��

			char value[200] = {0};
			sprintf(value, "%d-%d-%ld-%d", continuedate, isgetreward, mtime, restime);
			log_debug(" false before set, the value is %s", value);
			g_qredis.Set(key, value);
			log_debug(" false after set");

			// ��ʱ����
			g_qredis.ExpireKey(key, S48H);
			log_debug("after ExpireKey");

			// ���ؿ����콱 TODO
			log_debug("the luck value is %s", value);

			return continuedate; // ����������½������
		}
		else
		{
			// ������½���
			char value[200] = {0};
			log_debug(" true before getValue");
			ret = g_qredis.GetValue(key, value);
			if (!ret)
			{
				log_error("can't find the key, the key is %s", key);
				// ���ز������콱
				return 0;
			}
			log_debug(" true after getValue");

			// check the value �� ���� ����ֵ
			int continuedate = 0;
			int isgetreward = 1;
			long int mtime = 0;
			int restime = 0;
			sscanf(value,"%d-%d-%ld-%d",&continuedate,&isgetreward, &mtime, &restime);

			if (mtime == 0)
			{
				// ���ز������콱
				log_error("the mtime == 0");
				return 0;
			}

			long int nowtime = GetUnixTime();
			log_debug("the nowtime is %ld, the mtime is %ld", nowtime, restime);
			if (nowtime - mtime <= restime && isgetreward == 1)
			{
				// ���ز������콱�������Ѿ��콱��
				return 0;
			}
			if (nowtime - mtime <= restime && isgetreward == 0)
			{
				// ���ؿ����콱(����û�콱)
				return continuedate;
			}
			if (nowtime - mtime > restime + 30)//S24H) //����һ��û��½
			{
				int continuedate = 1;
				int isgetreward = 0;
				long int mtime = GetUnixTime();//untime
				int restime = 30;//GetDelayTime();//ʣ��ʱ��

				char value[200] = {0};
				sprintf(value, "%d-%d-%ld-%d", continuedate, isgetreward, mtime, restime);
				log_debug(" false before set, the value is %s", value);
				g_qredis.Del(key);
				g_qredis.Set(key, value);
				log_debug(" false after set");

				// ��ʱ����
				g_qredis.ExpireKey(key, S48H);
				log_debug("after ExpireKey");

				// ���ؿ����콱 TODO
				log_debug("the luck value is %s", value);

				return continuedate; // ����������½������
			}
			if (nowtime - mtime > restime)
			{
				// ���ؿ����콱
				continuedate++;
				isgetreward = 0;
				mtime = GetUnixTime();
				restime = 30;//GetDelayTime();
				sprintf(value, "%d-%d-%d-%d", continuedate, isgetreward, mtime, restime);
				g_qredis.Del(key);
				g_qredis.Set(key, value);

				// ��ʱ����
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

		// ��һ�ε�½���м��п��ܶϿ���½��
		int continuedate = 1;
		int isgetreward = 1;
		int mtime = 0;//untime
		int restime = 0;//ʣ��ʱ��

		sscanf(value,"%d-%d-%d-%d",&continuedate,&isgetreward, &mtime, &restime);

		if (isgetreward == 1)
		{
			// �Ѿ��콱�ˣ�����false

			return false;
		}

		// ������
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