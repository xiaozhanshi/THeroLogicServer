#ifndef __STORESYSTEM__
#define __STORESYSTEM__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

typedef struct StoreProPrice_s
{
	int sid;
	int cid;
	int coin;
	int gem;
} StoreProPrice_t;

class CStoreSystem{
public:
	static CStoreSystem& instance();

public:
	void load_price_from_db();
	bool buy_Production_by_id(int mid, int nums, int sid, int cid);
	bool get_good_price(int sid, int cid, int & coin, int & gem);
	bool insert_good_to_user(int sid, int cid);

protected:
	static CStoreSystem sInstance;

private:
	CStoreSystem();
	~CStoreSystem();

private:
	vector<StoreProPrice_t> m_price_vec;
};

#ifndef  g_store()
#define  g_store() (CStoreSystem::instance())
#endif

#endif


// note
// 1.把价格表加到内存结构中
// 2.找到物品的价格，并检查用户的钱数能否购买，不能则返回false
// 3.能购买则插入用户的数据库中，并扣除相应的钱, 把扣的实际钱数返回给客户端,针对特别的产品做特别的处理，例如英雄
