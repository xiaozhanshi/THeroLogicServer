#ifndef __LUCKYSYSTEM__
#define __LUCKYSYSTEM__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

typedef struct PlayerRewardLevel_s
{
	int level;
	int sid;
	int cid;
	int nums;
}PlayerRewardLevel_t;

class CLuckySystem{
public:
	static CLuckySystem& instance();

public:
	int CheckReward(int mid);
	bool GetReward(int mid);
	bool LoadRewardTable();

protected:
	static CLuckySystem sInstance;

private:
	CLuckySystem();
	~CLuckySystem();

public:
	vector <PlayerRewardLevel_t> m_RewardTable;
};

#ifndef  g_lcuky
#define  g_lcuky (CLuckySystem::instance())
#endif

#endif