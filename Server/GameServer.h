#ifndef GAMESERVER_H__20110530
#define GAMESERVER_H__20110530

#include "SocketServer.h"

#include "SingletonClass.h"
#include "TimerObj.h"
#include "Player.h"
#include "DBOperator.h"
#include "ConfigMgr.h"
#include "GameData.h"

class CentServer;
class CPlayer;

class CGameServer : public SocketServer
{
public:
    CGameServer(void);
    virtual ~CGameServer(void);
    bool    InitServer();
    void    ShutDown();
	int     ProcessClose(TcpHandler *pHandler);	// ���ӶϿ�

public:
    virtual int     ProcessPacket(NETInputPacket *pPacket, SocketHandler *pHandler, DWORD dwSessionID);
    virtual void    OnConnect(SocketHandler *pHandler );
    virtual void    OnDisconnect(SocketHandler *pHandler );

public:
	int             Handle_Iphone_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId=0);			// �û�iphone�û���½
	int             Handle_FB_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId=0);				// �û�FB�û���½
	int             Handle_Regist_User(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId=0);				// �û�ע��
	int             Handle_User_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);			// �û���Ϸ�˺ŵ�½
	int             Handle_User_ChangePassWord(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// �û��޸�����
	int             Handle_User_ChangeMail(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// �û��޸�����
	int             Handle_User_FBBINGDING(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// �û�FB�˺Ű�
	int				Handle_User_FindPassWord(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// �һ�����
	int             Handle_User_Add_FriendMessage(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);// �û����ͺ���������Ϣ
	int             Handle_User_Change_Message_State(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);//�ı���Ϣ״̬
	int             Handle_User_Add_FriendAccept(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// ������Ӻ���Ո��
	int             Handle_User_Delete_Friend(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// ɾ������Ո��

	int             Handle_Logon(TcpHandler * pHandler, NETInputPacket * pPacket, int gameMid, int isNewData, string iphoneMac, int isFb=0);	// �û���¼
	int             Handle_User_Get_Friend_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// ������б�
	int             Handle_User_Get_Message_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// ��ȡ�û���Ϣ
	int             Handle_User_System_Message_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);  // ��ȡϵͳ��Ϣ
	int             Handle_User_Get_More_Game_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);   // ��ȡ������Ϸ�б�
	int             Handle_User_Send_Message_To_APN(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);  // ������Ϣ��apn������
	int             Handle_User_Change_Header_Type(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// �û��޸�ͷ��
	int             Handle_User_Get_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// ��ȡ��Ʒ������
	int             Handle_User_Buy_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// ��ȡ��Ʒ������
	int             Handle_User_Edit_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// �༭��Ʒ������
	int             Handle_User_Get_Enlarge_Table(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// ��ȡ���������
	int             Handle_User_Buy_Enlarge_Table(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// ��ȡ���������
	int             Handle_User_Require_Mechine_UpLevel(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// �����������������
	int             Handle_User_Require_Mechine_UpNextLevel(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// �����������
	int             Handle_User_Require_Mechine_Status(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// �����������
	int             Handle_User_Require_Mechine_Speed(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// �����������
	int             Handle_User_Require_Mechine_Comfor(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// ����ȷ��
	int             Handle_User_Info_Mechine_CollectCoin(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// �鱨���ռ�
	int             Handle_User_Info_Mechine_CollectSpeed(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);// �鱨���ռ�����
	int             Handle_Get_PropsSynthesis_Condition(TcpHandler * pHandler, NETInputPacket *pPacket, int ProxyId = 0);   // ��ȡ���ߺϳ�����
	int             Handle_Get_PropsSynthesis_Request(TcpHandler * pHandler, NETInputPacket *pPacket, int ProxyId = 0);     // ���ߺϳ�����
	int             Handle_Get_ResourceProduc_Condition(TcpHandler * pHandler, NETInputPacket *pPacket, int ProxyId = 0);   // ��ȡ��Դ����������
	int             Handle_Mechine_ResourceProduc(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);        // ��Դ������
	int             Handdle_Mechine_ResourceCollect(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);      // ��Դ�ռ�


	void            Handle_Echo(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);				// �û�echo��Ϣ
	void            Handle_GetGoods(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);			// �û���ȡ����
	int				Handle_User_GetReward(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// ��ȡ������½����

private:
    void            OnLogon(CPlayer * pUser, int haveemail, string email, int isFb=0);	 // �û���¼
    void            OnLogout(CPlayer * pUser);											 // �û��˳�, �����Ż�
    CPlayer *       addPlayer(const uint32 uuid, TcpHandler * pSession);				 // ������
    bool            KickPlayer(CPlayer *pPlayer);										 // �߳����
	int				CreateNewUser(string& NickName, int & gameMid);						 // ���������
	void			get_production_by_pcate(OrmenGoodsStruct_t_list& pplist, int pcate); // ��ȡ�ض���Ʒ
	bool			get_production_price_by_pcate(int & price, int & money, int pcate, int pframe);	// ��ȡ�ض���Ʒ�ļ۸�
	bool			get_production_sellprice_by_pcate(int & iscansell, int & coin, int pcate, int pframe);				// ��ȡ���ۼ۸�
	bool			getMechineInfoByPcatePframe(int pcate, int pframe, SysMechineInfo & Mechine);					   // ��ȡ�ض����͵Ļ���
	bool            getUserMechineInfoByPcatePframe(CPlayer *UserName, int pcate, int pframe, User_Experiment_Mechine_t & Mechine);    //��ȡ�û��ض����ͻ���
	void			sent_user_mechine(vector<ReturnMechineInfo> & returnMlist, CPlayer *pPlayer);
	void			packet_mechine_for_user(ReturnMechineInfoTable_list  & returnMechineList, MechineType_PcatePframe_T & MechinePcate, int gameid, UserBuyMechineTable_list & m_userMechineList);
	void			packet_mechine_for_user(ReturnMechineInfoTable_list  & returnMechineList, MechineType_PcatePframe_T & MechinePcate, int gameid, User_Experiment_Mechine_t & m_userMechine);

public:
	void			Handle_User_Regist_UserName(TcpHandler * pHandler, string & UserName, int gameid, int ProxyId);		// �����ע���û���Ϣ
	void			SendConfigMsg( int isNewData, string iphoneMac, int gameMid, int flag, TcpHandler * pHandler );		// �������÷�����Ϣ��
	void			SendCommonFlagMsgReply( int CMD, int flag, TcpHandler * pHandler);						// ͨ�ûظ�
	void			SendInstorUpLevelReply( int CMD, int flag, int pid, ReturnInstorUpLevelTJ & InstorUpLevelTiaoJian, TcpHandler * pHandler);
	void			SendCommonEditFlagMsgReply( int CMD, int pid, int flag, TcpHandler * pHandler);
	void			SendCommonMechineStatusReply( int CMD, int pid, int flag, int leftTime, int money, TcpHandler * pHandler);
	void			SendBuyFlagMsgReply( int CMD, int flag, int pid, int pcate, int pframe, int locationx, int locationy, TcpHandler * pHandler);		// ����ʧ�ܵĻظ�
    void            SendMsg2Center(NETOutputPacket *pPacket);							// ������Ϣ�����ķ�����
	bool			ChangeUserMechine_By_Pid(UserBuyMechineTable_list & m_userMechineList, User_Experiment_Mechine_t & m_userMechine);
	bool			ChangeUserInfoMechineCollect_By_Pid(UserInfoMechineCollectTable_list & userInfoCollectList, UserInfoMechineCollect_T & UserInfoCollect);
	bool			getUserInfoMechineCollect_By_Pid(UserInfoMechineCollectTable_list & userInfoCollectList, UserInfoMechineCollect_T & UserInfoCollect, int gid);
	bool			getUserMechine_By_Pid(UserBuyMechineTable_list & m_userMechineList, User_Experiment_Mechine_t & m_userMechine, int gid);
	bool            getPowUpMechine_By_Pcate_Pframe(UserBuyMechineTable_list & m_userMechineList, User_Experiment_Mechine_t & PowerMechineItem, int pcate, int pframe);//��ȡ�������޻���Ϣ
	int				getNeedUserLevel(int instorLevel);//��ȡ�û���Ҫ�ȼ�
	bool			getInstorUplevelComsum(int instorLevel, InstrutorUpLevelTable_T & oneitem);
	bool			getMechineUplevelComsum(int pcate, int pframe, int level, MechineUpLevelComsumTable_T & oneitem);
	bool			getCollectTimeByCollItem(UserInfoMechineCollect_T & item, int level);							// ��ȡ�ռ���Ҫ��ʱ��
	bool			getCollectItemByPcateLevel(InfoMechineCollectTable_T & item, int pcate, int pframe, int level);	// ��ȡ�ռ��ض�item
	bool            getTidEqualGid(int tid, Props_T & result);     //��ȡtid��Gid��ȵ�������ʹ˷���ֵ
	bool            getResourceMechine_By_pacte_pfram_level(int pacte, int pframe, int level, int mode, ResourceMechineProductype_T & ResMechineInfo); //ͨ���û����������������ࡢ�η��ࡢ�ȼ���������Դ�����Ӧ�Ļ�����Ϣ
	bool            getUserResMechineCollect_By_Pid(UserResourceProducTable_list & userResCollectList, ResourceProducTable_T & userResCollect, int gid); //��ȡ�û��ռ�����Ϣ
	bool            ChangUserResMechineProduc(UserResourceProducTable_list & userresmechineproduc, ResourceProducTable_T & UserResMechineProduc); //�����û���Դ����Ϣ
	bool            getResourceType(ResourceMechineProductype_list & ResProList, ResourceMechineProductype_T & ResList, int pcate, int pframe, int mode, int level);  //��ȡ��Դ��������Դ������
	bool            getPowerUp_By_Level(PowerMechineUpLevelTable_list & PowerUpList, PowerMechineUpLevelTable_T & PowerUpType, int level);  //��ȡ��Դ����ֵ��Ϣ
	int             getPowerUpValue(PowerMechineUpLevelTable_T & PowerUpType, int ResType); //��ȡĳ����Դ����ֵ
	bool            getPropMergeTableInfo_By_Tid(MergePKTable_list & MergePKList, MergePKTable_T & MergeInfo, int tid);  //��ȡ���ߺϳɱ���Ϣ



	void			LoadSystemMsg();													// ��ǰ����ϵͳ��Ҫ�ı��

private:
    CentServer*             m_pCentServer;              // ���ķ���������
	static const  string	m_cSecretKey;				// ����KEY
	vector<SystemMessageInfo> m_systemMsg;				// ϵͳ��Ϣ
	vector<GameListInfo>	m_gameList;					// ��Ϸ�б�
	OrmenGoodsStruct_t_list	m_ormenGoodslist;			// װ�����б�
	EnlargeTable_list		m_enlargelist;				// �����б�
	SysMechineTable_list	m_systemMechineList;		// �����б�

	InstorUserLevel_list	m_instorUserlist;			// ָ��Ա�����ǵĵȼ���ϵ��
	InstrutorUpLevelTable_list m_instorUpLevelList;		// ָ��Ա�������ı�

	MechineUpLevelComsumTable_list m_MechineUpLevelComsumList;	// �����������ı�
	InfoMechineCollectTable_list  m_infoMechineCollectList;		// �鱨���ռ�������
	// ��Դ���޻�
	PowerMechineUpLevelTable_list m_powerMechineUpLevelList;	// ��Դ���޻����ı�

	MergeResourseTable_list m_MergeResourseTablelist;     //���ߺϳ�ԭ���ϱ�
	MergePKTable_list m_MergePKTablelist;                 //���ߺϳ���Ҫ���ϼ�������

	ResourceMechineProductype_list m_ResourceMechineProducTypeList;  //��Դ����
	
};

#endif //GAMESERVER_H__20110530


