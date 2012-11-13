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

// ����˼·

// 1. vector<int> �±���Ϊ������ֵΪ���ID  ------------  ���ݿ������� id ����
// 2. ������������ʱ�����������
// 3. ĳ�������������ʱ��(dirtytable)���±�ҲҪ���£�5���ӻ�дһ�Σ�,���߼�ʱд�����ݿ�