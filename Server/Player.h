#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>
#include "PacketBase.h"
#include "define.h"
#include "TimerOutEvent.h"
#include "TimerEvent.h"
#include "GameData.h"

using namespace std;
class TcpHandler;
class CGameServer;

typedef struct _FriendInfo
{
	string name;
	int LastLoginTime;
	string mail;
	int headimage;
	string fbid;
}FriendInfo;

typedef struct _UserMessageInfo
{
	int msgID;
	string senderName;
	short messagetype;
	string content;
	int time;
	short state;
}UserMessageInfo;

typedef struct _SystemMessageInfo
{
	int msgID;
	short messagetype;
	string title;
	string content;
	int time;
}SystemMessageInfo;

// 机器信息表
typedef struct _SysMechineInfo
{
	int pcate;
	int pframe;
	string name;
	short sizex;
	short sizey;
	int coinprice;
	BYTE isCanUpLevel;
	BYTE isCanSell;
	int sellprice;
	BYTE isCanStore;
	short BuyLevel;
	short limit;
	short curLip;
	short mpositionx;
	short mpositiony;
	short cossflag;
}SysMechineInfo;

typedef struct User_Experiment_Mechine_s  //用户购买机器
{
	int Gid;
	int pcate;
	int pframe;
	short position;
	int level;
	int curDir;
	short positionx;
	short positiony;
	short status;
	int endtime;
	bool isDirty;
}User_Experiment_Mechine_t;

typedef struct _ReturnMechineInfo
{
	int gid;
	int pcate;
	int pframe;
	string name;
	int sellprice;
	short sizex;
	short sizey;
	int level;
	short curDir;
	short mpositionx;
	short mpositiony;
	short status;
}ReturnMechineInfo;

typedef struct _ReturnInstorUpLevelTJ
{
	int otherNeed;
	int needLevel;
	int coin;
	int moneyTime;		// 一个高级货币加速的时间
	int needTime;		// 需要时间
}ReturnInstorUpLevelTJ;

typedef struct _GameListInfo
{
	int gameid;
	string url;
	string iden;
	string icon;
}GameListInfo;

typedef struct _InstorUserLevelRelation_S
{
	int instorLevel;
	int userLevel;
}InstorUserLevelRelation_T;

typedef struct InstrutorUpLevelTable_S
{
	int Level;
	int Time;
	int Coin;
	int MoneyTime;		// 一个高级货币加速的时间
}InstrutorUpLevelTable_T;

typedef struct MechineUpLevelComsumTable_S
{
	int pcate;
	int pframe;
	int Level;
	int Time;
	int Coin;
	int MoneyTime;		// 一个高级货币加速的时间
}MechineUpLevelComsumTable_T;

typedef struct InfoMechineCollectTable_S
{
	int pcate;
	int pframe;
	int Level;
	int Time;
	int EarCoin;		// 挣取的钱
	int MoneyTime;		// 一个高级货币加速的时间
}InfoMechineCollectTable_T;

typedef struct UserInfoMechineCollect_S
{
	int Gid;
	int Mid;
	int pcate;
	int pframe;
	int LeftTime;
	int NeedTime;
	int Status;
	bool isDirty;
}UserInfoMechineCollect_T;

typedef struct PowerMechineUpLevelTable_S
{
	int Level;
	int Time;
	int Coin;
	int MoneyTime;		// 一个高级货币加速的时间
	int PowerLimitA;
	int PowerLimitB;
	int PowerLimitC;
	int PowerLimitD;
	int PowerLimitE;
}PowerMechineUpLevelTable_T;



typedef vector<SysMechineInfo> SysMechineTable_list;
typedef vector<InstorUserLevelRelation_T> InstorUserLevel_list;
typedef vector<InstrutorUpLevelTable_T> InstrutorUpLevelTable_list;
typedef vector<PowerMechineUpLevelTable_T> PowerMechineUpLevelTable_list;
typedef vector<User_Experiment_Mechine_t> UserBuyMechineTable_list;
typedef vector<ReturnMechineInfo> ReturnMechineInfoTable_list;
typedef vector<MechineUpLevelComsumTable_T> MechineUpLevelComsumTable_list;
typedef vector<InfoMechineCollectTable_T> InfoMechineCollectTable_list;
typedef vector<UserInfoMechineCollect_T> UserInfoMechineCollectTable_list;



class CPlayer : public TimerOutEvent
{
private:
    CPlayer(void);
    CPlayer(const CPlayer &);
    CPlayer & operator=(const CPlayer &);

public:
    explicit CPlayer(uint32 uuid, TcpHandler * pHandler ,CGameServer *pServer);
    virtual ~CPlayer(void);
    // 角色ID
    uint32  getID()const ;
    TcpHandler* GetSocketHandler();
    // 发送消息
    void    SendMsg(NETOutputPacket* outPacket);
	// 角色状态
    uint32  getStatus() ;
    void    setStatus( uint32 nStatus ) ;
    // 等级 
	void	CalculateLevel();
	int		CalcLevel( int nPoint );
    void    SetLevel(uint32 nLv){ m_nPlayerLevel = nLv;}
    uint32  GetLevel(){ return m_nPlayerLevel;}
    // 名字
    void    SetName(string & strName){ m_sUserName = strName;}
    string &GetName(){ return m_sUserName;}
	// 密码
	void    SetPassword(string & strPassword){ m_sPassWord = strPassword;}
	string &GetPassword(){ return m_sPassWord;}
    // 玩家登录平台
    uint32  GetPlatForm(){ return m_PlatForm ;}
    void    SetPlatForm(uint32 platform){ m_PlatForm = platform; }

	void	StartCheckFunc(int second);
	void	StartDBWriteTimer(int second);
	void	StopTimer();
	void	StopDBWTimer();	// 停止写数据库定时器
	void	ProcessMechineRequire();
	void	OnUserLogoutServer();
	void	ProcessMechineWriteDB();
	
	void	SetUserInfo(UserGameInfo_Struct_t userData);

	// 实验机器
	User_Experiment_Mechine_t & getUserExperimentMechine()
	{
		return m_userExperimentMechine;
	}
	virtual int ProcessOnTimerOut(int Timerid);

	// 主角相关
	// 能源 函数
	void	SetPow(int ptype, int pvalue);		//pyte 1 -- a,2 --- b,3 --- c,4 --- d,5 ---e
	uint32	GetPow(int ptype);					//pyte 1 -- a,2 --- b,3 --- c,4 --- d,5 ---e

public:
	//获取用户资源生产信息
	bool getUserResourceProducInfo(int gid, int usid, UserResourceProducTable_list & userResProList, ResourceProducTable_T & userResInfo);

public:
	int m_istatus;		// status	// base 基地信息
	int m_ibasew;		// base width
	int m_ibaseh;		// base height
	User_Experiment_Mechine_t m_userExperimentMechine;		// 用户实验机器
	User_Experiment_Mechine_t m_userInstorMechine;			// 用户指导员机器
	UserBuyMechineTable_list m_userBuyMechineList;			// 用户购买的机器
	UserInfoMechineCollectTable_list m_userInfoCollectList;	// 用户收集表
	UserResourceProducTable_list m_UserResourceProducTable_list; //用户能源机生产表

public:
	vector<string>  m_sfriends;          // 好友名称    
	int			m_money;				 // 玩家高级货币
	int			m_coin;					 // 玩家普通货币

	TimerEvent	m_UserTimer;			 //用户定时器例如检查是否扩张完成
	TimerEvent	m_UserDBTimer;			 //用户定时器定时将脏数据回写到数据库

private:
    uint32      m_PlatForm;              // 用户登录平台
    uint32      m_nStatus;               // 用户的状态信息 emUSER_STATE
    uint32      m_nMid;                  // 用户ID
    uint32      m_nPlayerLevel;          // 玩家等级
	uint32		m_nPlayerPoint;			 // 玩家经验
    CGameServer*    m_pServer;
    TcpHandler*  m_pSocketHandler;
	string		m_sUserName;             // 玩家名字
	string		m_sPassWord;			 // 玩家密码

	// 主角信息
	uint32		m_PowA;					 // 能源A
	uint32		m_PowB;					 // 能源B
	uint32		m_PowC;					 // 能源C
	uint32		m_PowD;					 // 能源D
	uint32		m_PowE;					 // 能源E
};

#endif //_PLAYER_H_

