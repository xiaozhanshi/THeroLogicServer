#include "DBOperator.h"
#include "PlatFunc.h"
#include "RankingSystem.h"
#include "log.h"

using namespace std;

CRankSystem CRankSystem::sInstance;

CRankSystem& CRankSystem::instance()
{
	return sInstance;
}

CRankSystem::CRankSystem()
{

}

CRankSystem::~CRankSystem()
{

}

void CRankSystem::Load_RankTable()
{
	g_GetCDBOperator().Load_Rank_Table(m_Rank_Table);

	int sizeR = m_Rank_Table.size();

	log_debug("the ranktable size is %d\n", sizeR);
	for (int i = 0; i < sizeR; i++)
	{
		log_debug("the i %d is %d\n", i, m_Rank_Table[i]);
	}
}
