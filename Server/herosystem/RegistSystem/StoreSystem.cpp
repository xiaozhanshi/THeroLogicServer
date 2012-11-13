#include "PlatFunc.h"
#include "StoreSystem.h"
#include "log.h"
#include "qredis.h"
#include "DBOperator.h"

CStoreSystem CStoreSystem::sInstance;

CStoreSystem& CStoreSystem::instance()
{
	return sInstance;
}

CStoreSystem::CStoreSystem()
{

}

CStoreSystem::~CStoreSystem()
{

}

bool CStoreSystem::buy_Production_by_id(int mid, int nums, int sid, int cid)
{

}

void CStoreSystem::load_price_from_db()
{
	g_GetCDBOperator().Get_Price_FromBD(m_price_vec);

	log_debug("the price len is %d", m_price_vec.size());
}

bool CStoreSystem::get_good_price(int sid, int cid, int & coin, int &gem)
{
	log_debug("get_good_price %d, %d, enter", sid, cid);

	int plen = m_price_vec.size();
	if (plen == 0)
	{
		load_price_from_db();
	}
	plen = m_price_vec.size();
	if (plen == 0)
	{
		log_error("get_good_price %d, %d, can't get price table\n", sid, cid);
		return false;
	}

	StoreProPrice_t tpvalue;
	coin = 0;
	gem = 0;

	bool findFlag = false;
	log_debug("get_good_price, get the pcate %d, pframe %d\n", sid, cid);
	for(int i = 0; i < plen; i++)
	{
		tpvalue = m_price_vec[i];
		if (tpvalue.sid == sid && tpvalue.cid == cid)
		{
			coin = tpvalue.coin;
			gem = tpvalue.gem;
			findFlag = true;
			break;
		}
	}
	log_debug("get_good_price %d, the findFlag is  %d\n", sid, findFlag);

	log_debug("get_good_price %d, leave\n", sid);
	
	return findFlag;
}

bool CStoreSystem::insert_good_to_user(int sid, int cid)
{

}
