#ifndef _DB_OPERATOR_H_20110803
#define _DB_OPERATOR_H_20110803

#include "SingletonClass.h"
#include "DatabaseMysql.h"
#include "split_string.h"
#include "define.h"
#include <map>
#include <vector>
#include <stdarg.h>
#include "GameData.h"
#include "TaskHandler.h"
#include "RegistSystem.h"
#include "StoreSystem.h"
#include "LuckySystem.h"
#include "PacketBase.h"
#include "Player.h"

typedef CMysqlStore QueryResult;
class  CAsyncTask;
struct MemcacheConfig;

struct Server_type
{
	int32 id;			//������ID
	int32 groups;		//����������
	int32 mgid;			//��������
	int32 type;			//����������
	int32 subtype;		//������������
	int32 begin;		//��ʼ�����
	int32 end;			//���������
	int32 maxload;		//�����
	string ip;			//������IP��ַ
	string port;		//�������˿�
};

typedef struct MechineType_PcatePframe_S 
{
	int pcate;
	int pframe;
}MechineType_PcatePframe_T;

struct ServerState_type : public Server_type
{
	int32 state;		//������״̬, 0 ������ 1 ���ִ���
	int32 load;			//����������
};

typedef vector<Server_type > ServerList_type;

typedef vector<ServerState_type > ServerStateList_type;

// struct for goods;
typedef struct GoodsStruct_s
{
	int32 sid;
	int32 cid;
	string sname;
	string size;
	string position;
	short isupgrate;
	short nums;
	short issell;
	short ishouse;
	short people;
	int32 consumeTime;
	int32 buildResource;
	short requireBaseLevel;
}GoodsStruct_t;

typedef struct BaseInfo_Struct_s
{
	int ibasew;
	int ibaseh;
	int iwallw;
	int iwallh;
	int istatus;
	int lefttime;
	int iebasew;
	int iebaseh;
}BaseInfo_Struct_t;

typedef struct UserGameInfo_Struct_s
{
	int coin;
	int money;
	int level;
}UserGameInfo_Struct_t;

typedef vector<GoodsStruct_t > GoodsStruct_t_list;

typedef struct OrmenGoodsStruct_s
{
	int pcate;
	int pframe;
	string name;
	short type;
	short sizex;
	short sizey;
	int coin;
	int money;
	int nlevel;
	short crossflag;
	string info;
	short isCanSell;
	int sellCoin;
}OrmenGoodsStruct_t;

typedef struct EnlargeTableStruct_s
{
	int Gid;
	int nlevel;
	int coin; 
	int money;
	int sizex;
	int sizey;
	int ctime;
	int mtime;
}EnlargeTableStruct_t;

typedef struct UserOrmenGoodsStruct_s
{
	int pid;
	int mid;
	int pcate;
	int pframe;
	short locationx;
	short locationy;
	int curDir;
	int position;
}UserOrmenGoodsStruct_t;

typedef struct ReUserOrmenGoodsStruct_s
{
	int pid;
	int pcate;
	int pframe;
	string name;
	int sellCoin;
	short sizex;
	short sizey;
	short crossFlag;
	int curDir;
	short locationx;
	short locationy;
}ReUserOrmenGoodsStruct_t;

typedef vector<OrmenGoodsStruct_t> OrmenGoodsStruct_t_list;
typedef vector<ReUserOrmenGoodsStruct_t> ReUserOrmenGoodsStruct_t_list;
typedef vector<EnlargeTableStruct_t> EnlargeTable_list;

#define ID_JIANZHU 1
extern GoodsStruct_t_list g_Goodslist;

class CDBOperator : public AutoDeleteSingleton < CDBOperator >
{
     friend  class AutoDeleteSingleton < CDBOperator >; 
public:
    CDBOperator(void);
    virtual ~CDBOperator(void);

    enum 
    {
        DB_OK				= 0,
        DB_NONE_RECORD, 
        DB_UNKNOWN_ERROR
    };
    // ���ݿ�����
    bool    init();
    
//     // �����첽�߳�
//     bool startAsyncDB();

    // �ر�����
    void    ShutDown();

    // ��ȡ˫��ʱ����Ϣ
    bool    getDoubleExpTime( map<uint32, map<uint32, uint32> > &DoubleExp );

#if 0
    // ����PHPʹ������. �Ͻӿ�
    bool    UseSpeakByPHP( const uint32 uuid );
#endif

	// ����PHPʹ�õ���
	bool UseProps( const uint32 uuid, const int mppid, const int pcate, const int pframe );
    
    // ����PHPAPI
    bool    ExePhpAPI( const string & url,const string & param);

    //CURL�����ִ�ʱ��д��
    static int writer(char *data, size_t size, size_t nmemb, string *writerData);

    void    useProps_( const uint32 uuid, const uint32 props, const int32 count);

    // ��þ�����ʱ��
    bool    getPromoteTime( map<uint32, map<uint32, uint32> > &PromoteTime);

    // ���һ����ҵ����������id
    uint32  GetUserServerID( uint32 UserID );

	// ����û�����֤key
	bool getUserKey(const uint32 uuid, string & key);

    void    SetUserServerID( uint32 UserID ,uint32 ServerID );
    // ���һ����ҵ�״̬
    uint32  GetUserState( uint32 UserID );
    void    SetUserState( uint32 UserID,uint32 nState);
    
    // ���÷���������
    void    SetServerStress( uint32 nValue );

	void updateTask(const uint32 uuid, const TaskInfo & task, bool bOnlyMemcached);

	void updateTask(const uint32 uuid, const string  & key, const string & value );


	bool getUseTaskInfo(const uint32 uuid, TaskInfos_type & tasks);


	void getTaskInfoFromDB(const uint32 uuid, const uint32 taskid, TaskInfos_type & tasks);

	void UpdateTaskToMem(const uint32 uuid, const uint32 taskid, const string & tasks);

	bool getParserSubTasksInfo(const string & subTask, TaskInfo & task, TaskInfos_type & tasks);

	void ParserTaskInfo(const uint32 uuid, const uint32 taskid, TaskInfos_type & tasks);

	bool ParserSubTaskInfo(const string & str, TaskInfo & task);

	bool getTaskDefine(TaskDefines_type & tasks);




public:
	bool GetServerList(const int mgid,  ServerStateList_type & servers);

	bool UpdateServerState(ServerStateList_type & servers);

	int getUserWinInfo(const uint32 uuid, int & level);

	static int CalcLevel(int nPoint);

	bool getServerConfig( const uint32 sid, Server_type & s );

	bool getChatCenter(const int mgid, const int32 groups, string & host, string & port);

	bool getLastASVersion(const int32 /*svid*/, const int32 groups, string & version);

	int getIncreaseID();

	// herosystem
public:
	bool GetGoodsBySid(const int32 gsid, GoodsStruct_t_list & goodslists);
	bool AddMessage(int & msgid, const string & receivename, const string & sendername, string & message, int messagetype, int unixtime);
	bool CheckUserInfoCollect(UserInfoMechineCollect_T & UserInfoCollect);
	bool CheckUserBase(BaseInfo_Struct_t& Baseinfo, int gameMid);
	bool CheckUserExperimentMechine(User_Experiment_Mechine_t& UserExperimentMechine, SysMechineInfo & MechineItem, int gameMid);
	bool GetUserBuyMechineList(UserBuyMechineTable_list& UserExperimentMechine, ReturnMechineInfoTable_list  & returnMechineList, SysMechineInfo & MechineItem, int gameMid);
	bool GetUserProduction(vector<UserOrmenGoodsStruct_t> & Productionlist, MechineType_PcatePframe_T & ProductionType, int gameMid, int position);
	bool UpdateUserInfoMechineCollectTable(UserInfoMechineCollect_T & UserInfoCollect);
	bool UpdateUserResMechineProducTable(ResourceProducTable_T & UserResProduc); //�����û���Դ��������(��ʱ�����ã�
	bool UpdateUserProducTable(ResourceProducTable_T & ResProduc);  //�����û���Դ����������Դ���������ã�
	bool UpdateUserMechineTable(User_Experiment_Mechine_t& UserExperimentMechine);//�����û�������
	bool UpdateUserBase(BaseInfo_Struct_t& Baseinfo, int gameMid);
	bool UpdateUserMoney(int gameMid, int coin, int money);
	bool CheckAndAddUser(const string & IphoneMac, string& NickName, int & gameMid);
	bool CheckFBUser(const string & FBID, int & gameMid, string & mail, string & password, string & name);
	bool CheckFBUserBinding(const string & FBID, int & gameMid, string & userNmae, string & passWord);
	bool CheckFBUserBinding2(const string & FBID, int & gameMid, string & userNmae, string & passWord);
	bool CheckAddFriend(string & userNmae);
	bool GetUserGameInfo(int gameId, UserGameInfo_Struct_t & UserGameInfo);
	bool BindFBuser(const string & FBID, int & gameMid, string & userNmae, string & passWord);
	bool UserChangeHeader(const string & userNmae, int htype);
	bool UpdateMessage( string & friendName, string & userName, int flag);
	bool CheckUser(const string & UserName, const string & md5password, int & gameMid);
	bool GetUserMailPassword(string & name, string & mail, string & password);
	bool GetUSerGid(const string & UserName, int & gameMid);
	bool CheckAndAddRegistUser(string& NickName, int & gameMid);
	bool AddRegistUser(regist_Info_t & userInfo);
	bool ChangeUserPassword(string & name, string & md5password, string & password);
	bool ChangeUserEmail(string & name, string & email, int isPub);
	bool AddFriend(string & FriendName, string & myName);
	bool GetFriendInfo(string fName, FriendInfo & FriendInfoList);
	bool GetUserHeadimageIndex(int gameid, short & headimageindex, string & fbid);
	bool GetUserHeadimageIndex_notRegist(int gameid, short & headimageindex);
	bool GetFBID_by_userName(string & UserName, string & FBID);
	bool GetSystemMessageInfoList(vector<SystemMessageInfo> & SystemMessageList);
	bool GetSystemOrmenGoodsList(OrmenGoodsStruct_t_list & OrmenGoodsList);
	bool GetSystemEnlargeTableList(EnlargeTable_list & EnlargeTableList);
	bool GetMoreGameInfoList(vector<GameListInfo> & MoreGameinfoList);
	bool GetMechineList(SysMechineTable_list & systemMechineList, OrmenGoodsStruct_t_list & OrmenGoodsList);
	bool GetInstorUserLevelList(InstorUserLevel_list & LevelList);
	bool GetMechineComStructTimeList(MechineComStructTimeTable_List & ComStructTimeList);
	bool GetMechineUpLevelComsumList(MechineUpLevelComsumTable_list & UpLevelComsumList);
	bool GetInfoMechineCollectTableList(InfoMechineCollectTable_list & infoCollectList);
	bool GetInstorUpLevelList(InstrutorUpLevelTable_list & UpLevelList);
	bool GetPowerMechineUpLevelList(PowerMechineUpLevelTable_list & UpLevelList);
	bool CheckIsAFriend(string & FriendName, string & myName);
	bool DelFriend(string & FriendName, string & myName);
	bool GetFriendList(string & MyName, vector<string> & FriendNameList);
	bool UserBuyProduction(UserOrmenGoodsStruct_t & production, int & gid);
	bool DelUserProduction(int pid);
	bool ChangeUserProductionDirLocation(int pid, int curDir, int locationx, int locationy);
	bool ChangeUserProductionHouse(int pid, int position);
	bool GetProduction_Type_by_Pid(int pid, int & pcate, int & pframe);
	bool GetMessageInfoList(string myName, vector<UserMessageInfo> & UserMessageList);
	bool Get_Price_FromBD(vector<StoreProPrice_t> & goods_price);
	bool Load_Reward_Table(vector<PlayerRewardLevel_t> & reward_table);
	bool Load_Rank_Table(vector<int> & Rank_table);
	bool CreateNewUser(string& NickName, int & gameMid);
	bool GetLoginReply(const int gameid, NETOutputPacket & pPacket);
	bool GetLRegistReply(const int gameid, NETOutputPacket & pPacket);
	bool GetRegistReply(const int gameid, NETOutputPacket & pPacket);
	bool GetMergeResourseTablelist(MergeResourseTable_list & ResourceTablelist);//��ȡ���ߺϳ�ԭ����
	bool GetMergePKTablelist(MergePKTable_list & MergePKTablelist);  //��ȡ���ߺϳ���Ҫ�������༰����
	bool GetResourceMechineProductypeList(ResourceMechineProductype_list & ResourceMechineProductypeList);  //��ȡ��Դ��������ʽ
	bool InsertUserResourceMechineRecord(ResourceProducTable_T & Record); //�����û���Դ��������
private:
    DatabaseMysql*  m_pMysql;
    //CAsyncTask * m_pAsyncTask;
};

// ��ȡDB�������
#ifndef  g_GetCDBOperator
#define  g_GetCDBOperator()   (CDBOperator::Instance())
#endif 


#endif	//_DB_OPERATOR_H_20110803

