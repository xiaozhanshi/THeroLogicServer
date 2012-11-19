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

// ������Ϣ��
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

typedef struct User_Experiment_Mechine_s  //�û��������
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
	int moneyTime;		// һ���߼����Ҽ��ٵ�ʱ��
	int needTime;		// ��Ҫʱ��
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
	int MoneyTime;		// һ���߼����Ҽ��ٵ�ʱ��
}InstrutorUpLevelTable_T;

typedef struct MechineUpLevelComsumTable_S
{
	int pcate;
	int pframe;
	int Level;
	int Time;
	int Coin;
	int MoneyTime;		// һ���߼����Ҽ��ٵ�ʱ��
}MechineUpLevelComsumTable_T;

typedef struct InfoMechineCollectTable_S
{
	int pcate;
	int pframe;
	int Level;
	int Time;
	int EarCoin;		// ��ȡ��Ǯ
	int MoneyTime;		// һ���߼����Ҽ��ٵ�ʱ��
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
	int MoneyTime;		// һ���߼����Ҽ��ٵ�ʱ��
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
    // ��ɫID
    uint32  getID()const ;
    TcpHandler* GetSocketHandler();
    // ������Ϣ
    void    SendMsg(NETOutputPacket* outPacket);
	// ��ɫ״̬
    uint32  getStatus() ;
    void    setStatus( uint32 nStatus ) ;
    // �ȼ� 
	void	CalculateLevel();
	int		CalcLevel( int nPoint );
    void    SetLevel(uint32 nLv){ m_nPlayerLevel = nLv;}
    uint32  GetLevel(){ return m_nPlayerLevel;}
    // ����
    void    SetName(string & strName){ m_sUserName = strName;}
    string &GetName(){ return m_sUserName;}
	// ����
	void    SetPassword(string & strPassword){ m_sPassWord = strPassword;}
	string &GetPassword(){ return m_sPassWord;}
    // ��ҵ�¼ƽ̨
    uint32  GetPlatForm(){ return m_PlatForm ;}
    void    SetPlatForm(uint32 platform){ m_PlatForm = platform; }

	void	StartCheckFunc(int second);
	void	StartDBWriteTimer(int second);
	void	StopTimer();
	void	StopDBWTimer();	// ֹͣд���ݿⶨʱ��
	void	ProcessMechineRequire();
	void	OnUserLogoutServer();
	void	ProcessMechineWriteDB();
	
	void	SetUserInfo(UserGameInfo_Struct_t userData);

	// ʵ�����
	User_Experiment_Mechine_t & getUserExperimentMechine()
	{
		return m_userExperimentMechine;
	}
	virtual int ProcessOnTimerOut(int Timerid);

	// �������
	// ��Դ ����
	void	SetPow(int ptype, int pvalue);		//pyte 1 -- a,2 --- b,3 --- c,4 --- d,5 ---e
	uint32	GetPow(int ptype);					//pyte 1 -- a,2 --- b,3 --- c,4 --- d,5 ---e

public:
	//��ȡ�û���Դ������Ϣ
	bool getUserResourceProducInfo(int gid, int usid, UserResourceProducTable_list & userResProList, ResourceProducTable_T & userResInfo);

public:
	int m_istatus;		// status	// base ������Ϣ
	int m_ibasew;		// base width
	int m_ibaseh;		// base height
	User_Experiment_Mechine_t m_userExperimentMechine;		// �û�ʵ�����
	User_Experiment_Mechine_t m_userInstorMechine;			// �û�ָ��Ա����
	UserBuyMechineTable_list m_userBuyMechineList;			// �û�����Ļ���
	UserInfoMechineCollectTable_list m_userInfoCollectList;	// �û��ռ���
	UserResourceProducTable_list m_UserResourceProducTable_list; //�û���Դ��������

public:
	vector<string>  m_sfriends;          // ��������    
	int			m_money;				 // ��Ҹ߼�����
	int			m_coin;					 // �����ͨ����

	TimerEvent	m_UserTimer;			 //�û���ʱ���������Ƿ��������
	TimerEvent	m_UserDBTimer;			 //�û���ʱ����ʱ�������ݻ�д�����ݿ�

private:
    uint32      m_PlatForm;              // �û���¼ƽ̨
    uint32      m_nStatus;               // �û���״̬��Ϣ emUSER_STATE
    uint32      m_nMid;                  // �û�ID
    uint32      m_nPlayerLevel;          // ��ҵȼ�
	uint32		m_nPlayerPoint;			 // ��Ҿ���
    CGameServer*    m_pServer;
    TcpHandler*  m_pSocketHandler;
	string		m_sUserName;             // �������
	string		m_sPassWord;			 // �������

	// ������Ϣ
	uint32		m_PowA;					 // ��ԴA
	uint32		m_PowB;					 // ��ԴB
	uint32		m_PowC;					 // ��ԴC
	uint32		m_PowD;					 // ��ԴD
	uint32		m_PowE;					 // ��ԴE
};

#endif //_PLAYER_H_

