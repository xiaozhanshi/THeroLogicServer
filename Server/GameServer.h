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
	int     ProcessClose(TcpHandler *pHandler);	// 连接断开

public:
    virtual int     ProcessPacket(NETInputPacket *pPacket, SocketHandler *pHandler, DWORD dwSessionID);
    virtual void    OnConnect(SocketHandler *pHandler );
    virtual void    OnDisconnect(SocketHandler *pHandler );

public:
	int             Handle_Iphone_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId=0);			// 用户iphone用户登陆
	int             Handle_FB_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId=0);				// 用户FB用户登陆
	int             Handle_Regist_User(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId=0);				// 用户注册
	int             Handle_User_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);			// 用户游戏账号登陆
	int             Handle_User_ChangePassWord(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 用户修改密码
	int             Handle_User_ChangeMail(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 用户修改邮箱
	int             Handle_User_FBBINGDING(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 用户FB账号绑定
	int				Handle_User_FindPassWord(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 找回密码
	int             Handle_User_Add_FriendMessage(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);// 用户发送好友邀请消息
	int             Handle_User_Change_Message_State(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);//改变消息状态
	int             Handle_User_Add_FriendAccept(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 接受添加好友請求
	int             Handle_User_Delete_Friend(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 删除好友請求

	int             Handle_Logon(TcpHandler * pHandler, NETInputPacket * pPacket, int gameMid, int isNewData, string iphoneMac, int isFb=0);	// 用户登录
	int             Handle_User_Get_Friend_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 获好友列表
	int             Handle_User_Get_Message_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 获取用户消息
	int             Handle_User_System_Message_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);  // 获取系统消息
	int             Handle_User_Get_More_Game_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);   // 获取更多游戏列表
	int             Handle_User_Send_Message_To_APN(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);  // 发送消息到apn服务器
	int             Handle_User_Change_Header_Type(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 用户修改头像
	int             Handle_User_Get_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 获取产品并返回
	int             Handle_User_Buy_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 获取产品并返回
	int             Handle_User_Edit_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 编辑产品并返回
	int             Handle_User_Get_Enlarge_Table(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 获取扩充表并返回
	int             Handle_User_Buy_Enlarge_Table(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 获取扩充表并返回
	int             Handle_User_Require_Mechine_UpLevel(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 请求机器升级的条件
	int             Handle_User_Require_Mechine_UpNextLevel(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 请求机器升级
	int             Handle_User_Require_Mechine_Status(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 请求机器升级
	int             Handle_User_Require_Mechine_Speed(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 请求机器加速
	int             Handle_User_Require_Mechine_Comfor(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 升级确认
	int             Handle_User_Info_Mechine_CollectCoin(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);	// 情报机收集
	int             Handle_User_Info_Mechine_CollectSpeed(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);// 情报机收集加速
	int             Handle_Get_PropsSynthesis_Condition(TcpHandler * pHandler, NETInputPacket *pPacket, int ProxyId = 0);   // 获取道具合成条件
	int             Handle_Get_PropsSynthesis_Request(TcpHandler * pHandler, NETInputPacket *pPacket, int ProxyId = 0);     // 道具合成请求
	int             Handle_Get_ResourceProduc_Condition(TcpHandler * pHandler, NETInputPacket *pPacket, int ProxyId = 0);   // 获取能源机生产条件
	int             Handle_Mechine_ResourceProduc(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);        // 能源机生产
	int             Handdle_Mechine_ResourceCollect(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);      // 能源收集


	void            Handle_Echo(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);				// 用户echo消息
	void            Handle_GetGoods(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);			// 用户获取道具
	int				Handle_User_GetReward(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId = 0);		// 获取连续登陆奖励

private:
    void            OnLogon(CPlayer * pUser, int haveemail, string email, int isFb=0);	 // 用户登录
    void            OnLogout(CPlayer * pUser);											 // 用户退出, 清理门户
    CPlayer *       addPlayer(const uint32 uuid, TcpHandler * pSession);				 // 添加玩家
    bool            KickPlayer(CPlayer *pPlayer);										 // 踢出玩家
	int				CreateNewUser(string& NickName, int & gameMid);						 // 创建新玩家
	void			get_production_by_pcate(OrmenGoodsStruct_t_list& pplist, int pcate); // 获取特定产品
	bool			get_production_price_by_pcate(int & price, int & money, int pcate, int pframe);	// 获取特定产品的价格
	bool			get_production_sellprice_by_pcate(int & iscansell, int & coin, int pcate, int pframe);				// 获取销售价格
	bool			getMechineInfoByPcatePframe(int pcate, int pframe, SysMechineInfo & Mechine);					   // 获取特定类型的机器
	bool            getUserMechineInfoByPcatePframe(CPlayer *UserName, int pcate, int pframe, User_Experiment_Mechine_t & Mechine);    //获取用户特定类型机器
	void			sent_user_mechine(vector<ReturnMechineInfo> & returnMlist, CPlayer *pPlayer);
	void			packet_mechine_for_user(ReturnMechineInfoTable_list  & returnMechineList, MechineType_PcatePframe_T & MechinePcate, int gameid, UserBuyMechineTable_list & m_userMechineList);
	void			packet_mechine_for_user(ReturnMechineInfoTable_list  & returnMechineList, MechineType_PcatePframe_T & MechinePcate, int gameid, User_Experiment_Mechine_t & m_userMechine);

public:
	void			Handle_User_Regist_UserName(TcpHandler * pHandler, string & UserName, int gameid, int ProxyId);		// 向大厅注册用户信息
	void			SendConfigMsg( int isNewData, string iphoneMac, int gameMid, int flag, TcpHandler * pHandler );		// 根据配置返回消息包
	void			SendCommonFlagMsgReply( int CMD, int flag, TcpHandler * pHandler);						// 通用回复
	void			SendInstorUpLevelReply( int CMD, int flag, int pid, ReturnInstorUpLevelTJ & InstorUpLevelTiaoJian, TcpHandler * pHandler);
	void			SendCommonEditFlagMsgReply( int CMD, int pid, int flag, TcpHandler * pHandler);
	void			SendCommonMechineStatusReply( int CMD, int pid, int flag, int leftTime, int money, TcpHandler * pHandler);
	void			SendBuyFlagMsgReply( int CMD, int flag, int pid, int pcate, int pframe, int locationx, int locationy, TcpHandler * pHandler);		// 购买失败的回复
    void            SendMsg2Center(NETOutputPacket *pPacket);							// 发送信息到中心服务器
	bool			ChangeUserMechine_By_Pid(UserBuyMechineTable_list & m_userMechineList, User_Experiment_Mechine_t & m_userMechine);
	bool			ChangeUserInfoMechineCollect_By_Pid(UserInfoMechineCollectTable_list & userInfoCollectList, UserInfoMechineCollect_T & UserInfoCollect);
	bool			getUserInfoMechineCollect_By_Pid(UserInfoMechineCollectTable_list & userInfoCollectList, UserInfoMechineCollect_T & UserInfoCollect, int gid);
	bool			getUserMechine_By_Pid(UserBuyMechineTable_list & m_userMechineList, User_Experiment_Mechine_t & m_userMechine, int gid);
	bool            getPowUpMechine_By_Pcate_Pframe(UserBuyMechineTable_list & m_userMechineList, User_Experiment_Mechine_t & PowerMechineItem, int pcate, int pframe);//获取能用上限机信息
	int				getNeedUserLevel(int instorLevel);//获取用户需要等级
	bool			getInstorUplevelComsum(int instorLevel, InstrutorUpLevelTable_T & oneitem);
	bool			getMechineUplevelComsum(int pcate, int pframe, int level, MechineUpLevelComsumTable_T & oneitem);
	bool			getCollectTimeByCollItem(UserInfoMechineCollect_T & item, int level);							// 获取收集需要的时间
	bool			getCollectItemByPcateLevel(InfoMechineCollectTable_T & item, int pcate, int pframe, int level);	// 获取收集特定item
	bool            getTidEqualGid(int tid, Props_T & result);     //获取tid和Gid相等的主分类和此分类值
	bool            getResourceMechine_By_pacte_pfram_level(int pacte, int pframe, int level, int mode, ResourceMechineProductype_T & ResMechineInfo); //通过用户购买机器表的主分类、次分类、等级，查找能源机表对应的机器信息
	bool            getUserResMechineCollect_By_Pid(UserResourceProducTable_list & userResCollectList, ResourceProducTable_T & userResCollect, int gid); //获取用户收集表信息
	bool            ChangUserResMechineProduc(UserResourceProducTable_list & userresmechineproduc, ResourceProducTable_T & UserResMechineProduc); //更改用户资源机信息
	bool            getResourceType(ResourceMechineProductype_list & ResProList, ResourceMechineProductype_T & ResList, int pcate, int pframe, int mode, int level);  //获取能源机生产能源的类型
	bool            getPowerUp_By_Level(PowerMechineUpLevelTable_list & PowerUpList, PowerMechineUpLevelTable_T & PowerUpType, int level);  //获取能源上限值信息
	int             getPowerUpValue(PowerMechineUpLevelTable_T & PowerUpType, int ResType); //获取某种能源上限值
	bool            getPropMergeTableInfo_By_Tid(MergePKTable_list & MergePKList, MergePKTable_T & MergeInfo, int tid);  //获取道具合成表信息



	void			LoadSystemMsg();													// 提前加载系统需要的表格

private:
    CentServer*             m_pCentServer;              // 中心服务器大厅
	static const  string	m_cSecretKey;				// 万能KEY
	vector<SystemMessageInfo> m_systemMsg;				// 系统消息
	vector<GameListInfo>	m_gameList;					// 游戏列表
	OrmenGoodsStruct_t_list	m_ormenGoodslist;			// 装饰物列表
	EnlargeTable_list		m_enlargelist;				// 扩充列表
	SysMechineTable_list	m_systemMechineList;		// 机器列表

	InstorUserLevel_list	m_instorUserlist;			// 指导员和主角的等级关系表
	InstrutorUpLevelTable_list m_instorUpLevelList;		// 指导员升级消耗表

	MechineUpLevelComsumTable_list m_MechineUpLevelComsumList;	// 机器升级消耗表
	InfoMechineCollectTable_list  m_infoMechineCollectList;		// 情报机收集机器表
	// 能源上限机
	PowerMechineUpLevelTable_list m_powerMechineUpLevelList;	// 能源上限机消耗表

	MergeResourseTable_list m_MergeResourseTablelist;     //道具合成原材料表
	MergePKTable_list m_MergePKTablelist;                 //道具合成需要材料及数量表

	ResourceMechineProductype_list m_ResourceMechineProducTypeList;  //能源机表
	
};

#endif //GAMESERVER_H__20110530


