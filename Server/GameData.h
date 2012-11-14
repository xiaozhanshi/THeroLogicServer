
#ifndef GAME_DATA_H__20110611
#define GAME_DATA_H__20110611

#include "define.h"
#include <string>
#include <vector>

using namespace std;

namespace Monitor
{
    enum
    {
        //游戏服务器心跳包
        GAME_SERVER_REPORT = 0x0001,

        //监控服务器主动发出DELAG测试请求.
        MONITOR_SERVER_DELAY_TEST_REQUEST,

        REGISTER_SERVER_REQUEST
        // PACKET { CMD:REGISTER_SERVER_REQUEST,  INT:服务器ID, INT:服务器类型. }
    };
};

struct Monitor_type
{
    string host;
    int    port;
    bool enable;
};

typedef struct MechineComStructTimeTable_S
{
	int pcate;
	int pframe;
	int Time;		// 建筑时间
}MechineComStructTimeTable_T;

typedef struct MergeResourseTable_s    //合成道具原材料结构
{
	int Gid;
	int pcate;
	int pframe;
}MergeResourseTable_T;

typedef struct MergePKTable_s        //合成道具需要材料种类及数量结构
{
	int Gid;
	int Pcate;
	int Pframe;
	int Level;
	int Tid1;
	int Nums1;
	int Tid2;
	int Nums2;
	int Tid3;
	int Nums3;
	int Tid4;
	int Nums4;
}MergePKTable_T;

typedef struct Props_s            //存放原材料主分类pcate、次分类pframe 及材料种类数量num 结构
{
	int pcate;
	int pframe;
	int num;     //数量
}Props_T;

typedef struct ResourceMechineProductype_s     //能源机生产类型结构
{
	int Pcate;
	int Pframe;
	int Level;
	int NeedTime;
	int Coin;
	int Power;
	int UserLevel;
	int Mtime;  //高级货币加速时间
}ResourceMechineProductype_T;

typedef struct ResourceProduc_s     //存放能源机部分属性结构
{
	int Level;
	int NeedTime;
	int Coin;
	int Power;
	int UserLevel;
}ResourceProduc_T;

typedef struct ResourceProducTable_s       //用户能源机生产结构
{
	int Gid;
	int Mid;
	int Pcate;
	int Pframe;
	int Pmode;             //  生产方式
	int Level;
	int State;            //  能源机状态 （生产状态、 生产结束状态)
	int Usetime;         //   能源机进行生产状态累计时间
	bool isDirty;
}ResourceProducTable_T;

typedef struct UserGameInfo_Struct_s
{
	string nickName;
	string password;
	int platform;
	int coin;
	int money;
	int level;
	int powA;
	int powB;
	int powC;
	int powD;
	int powE;
}UserGameInfo_Struct_t;

typedef vector<MechineComStructTimeTable_T> MechineComStructTimeTable_List;
typedef vector<MergeResourseTable_T> MergeResourseTable_list;  //合成道具原材料结构容器
typedef vector<MergePKTable_T> MergePKTable_list;             //合成道具需要材料种类及数量
typedef vector<Props_T> Props_list;                          //原材料主分类pcate、次分类pframe 及材料种类数量num
typedef vector<ResourceMechineProductype_T> ResourceMechineProductype_list;  //能源机生产容器
typedef vector<ResourceProduc_T> ResourceProduc_list;        //存放能源机部分属性容器
typedef vector<ResourceProducTable_T>  UserResourceProducTable_list;      //存放用户能源机生产的信息

#endif  // GAME_DATA_H__20110611

