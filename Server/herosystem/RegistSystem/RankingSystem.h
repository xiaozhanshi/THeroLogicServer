#ifndef __RANKINGSYSTEM__
#define __RANKINGSYSTEM__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

typedef struct RankStruct_S
{
	int playerid;
	int ranking;
}RankStruct_T;

class CRankSystem{
public:
	static CRankSystem& instance();

protected:
	static CRankSystem sInstance;

private:
	CRankSystem();
	~CRankSystem();

public:
	void Load_RankTable();

private:
	vector<int>	m_Rank_Table;
};

#ifndef  g_rank
#define  g_rank (CRankSystem::instance())
#endif

#endif

// 排名思路

// 1. vector<int> 下标作为排名，值为玩家ID  ------------  数据库排名表 id 排名
// 2. 服务器启动的时候加载排名表
// 3. 某个玩家排名更新时，(dirtytable)更新表也要更新（5分钟回写一次）,或者即时写入数据库