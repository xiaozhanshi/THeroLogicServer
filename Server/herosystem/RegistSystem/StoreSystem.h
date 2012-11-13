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
// 1.�Ѽ۸��ӵ��ڴ�ṹ��
// 2.�ҵ���Ʒ�ļ۸񣬲�����û���Ǯ���ܷ��򣬲����򷵻�false
// 3.�ܹ���������û������ݿ��У����۳���Ӧ��Ǯ, �ѿ۵�ʵ��Ǯ�����ظ��ͻ���,����ر�Ĳ�Ʒ���ر�Ĵ�������Ӣ��
