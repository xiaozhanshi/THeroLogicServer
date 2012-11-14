
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
        //��Ϸ������������
        GAME_SERVER_REPORT = 0x0001,

        //��ط�������������DELAG��������.
        MONITOR_SERVER_DELAY_TEST_REQUEST,

        REGISTER_SERVER_REQUEST
        // PACKET { CMD:REGISTER_SERVER_REQUEST,  INT:������ID, INT:����������. }
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
	int Time;		// ����ʱ��
}MechineComStructTimeTable_T;

typedef struct MergeResourseTable_s    //�ϳɵ���ԭ���Ͻṹ
{
	int Gid;
	int pcate;
	int pframe;
}MergeResourseTable_T;

typedef struct MergePKTable_s        //�ϳɵ�����Ҫ�������༰�����ṹ
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

typedef struct Props_s            //���ԭ����������pcate���η���pframe ��������������num �ṹ
{
	int pcate;
	int pframe;
	int num;     //����
}Props_T;

typedef struct ResourceMechineProductype_s     //��Դ���������ͽṹ
{
	int Pcate;
	int Pframe;
	int Level;
	int NeedTime;
	int Coin;
	int Power;
	int UserLevel;
	int Mtime;  //�߼����Ҽ���ʱ��
}ResourceMechineProductype_T;

typedef struct ResourceProduc_s     //�����Դ���������Խṹ
{
	int Level;
	int NeedTime;
	int Coin;
	int Power;
	int UserLevel;
}ResourceProduc_T;

typedef struct ResourceProducTable_s       //�û���Դ�������ṹ
{
	int Gid;
	int Mid;
	int Pcate;
	int Pframe;
	int Pmode;             //  ������ʽ
	int Level;
	int State;            //  ��Դ��״̬ ������״̬�� ��������״̬)
	int Usetime;         //   ��Դ����������״̬�ۼ�ʱ��
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
typedef vector<MergeResourseTable_T> MergeResourseTable_list;  //�ϳɵ���ԭ���Ͻṹ����
typedef vector<MergePKTable_T> MergePKTable_list;             //�ϳɵ�����Ҫ�������༰����
typedef vector<Props_T> Props_list;                          //ԭ����������pcate���η���pframe ��������������num
typedef vector<ResourceMechineProductype_T> ResourceMechineProductype_list;  //��Դ����������
typedef vector<ResourceProduc_T> ResourceProduc_list;        //�����Դ��������������
typedef vector<ResourceProducTable_T>  UserResourceProducTable_list;      //����û���Դ����������Ϣ

#endif  // GAME_DATA_H__20110611

