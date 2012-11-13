#ifndef GAME_CMD_H__20110530
#define GAME_CMD_H__20110530

namespace AntWars
{
	namespace Client
	{
		//客户端请求获得服务器列表， 用于查找进入房间
		const static int CLIENT_REQUEST_SERVER_LIST			= 1;
		// PACKET { CMD:CLIENT_REQUEST_SERVER_LIST, MID:UINT }

		//客户端请求获得公会自由战房间列表
		const static int CLIENT_REQUEST_FREEWAR_LIST		= 2;
		// PACKET { CMD:CLIENT_REQUEST_FREEWAR_LIST, MID:UINT, 页码:INT }

		//客户端请求获得副本房间列表
		const static int CLIENT_REQUEST_COPY_LIST			= 3;
		// PACKET { CMD:CLIENT_REQUEST_COPY_LIST, MID:UINT, 页码:INT }

		//客户端请求分配一个服务器, allocate.这个地方要优化，客户端缓存一下上次请求的结果，除非进不服务器，才再次调用本调口
		const static int CLIENT_REQUEST_ALLOCATE_SERVER		= 4;
		// PACKET { CMD:CLIENT_REQUEST_ALLOCATE_SERVER, MID:UINT, 申请的服务器类型:INT }
	};

	namespace Mgr
	{
		const static int CLIENT_COMMADN_SYSTEM_UPDATE = 1013; //请求更新服务器端配置数据
	}

	namespace Server
	{
		//服务器端应响获得服务器列表
		//response
		const static int SERVER_RESPONSE_SERVER_LIST		= 1;
		// PACKET { CMD:SERVER_RESPONSE_SERVER_LIST, 服务器计录数量:INT, [服务器ID:INT, 类型:INT,  IP:STRING, PORT:INT,]}

		//服务器端返回公会自由战房间列表
		const static int SERVER_RESPONSE_FREEWAR_LIST		= 2;
		// PACKET { CMD:SERVER_RESPONSE_FREEWAR_LIST, 房间数量:INT [房间信息, ...] }

		//客户端请求获得副本房间列表
		const static int SERVER_RESPONSE_COPY_LIST			= 3;
		// PACKET { CMD:CLIENT_REQUEST_COPY_LIST, MID:UINT, 页码:INT }

		//客户端请求分配一个服务器, allocate.这个地方要优化，客户端缓存一下上次请求的结果，除非进不服务器，才再次调用本调口
		const static int SERVER_RESPONSE_ALLOCATE_SERVER	= 4;
		// PACKET { CMD:CLIENT_REQUEST_ALLOCATE_SERVER, MID:UINT, 申请的服务器类型:INT }
	};
};


// 与中心服务器交互cmd
enum emNETMSGCOMMOND
{
    CHATSERVER_INFO         = 10010,            // 聊天服务器注册信息
    CHATSERVER_ROUTE        = 10011,            // 聊天服务器的路由广播消息
    CHATSERVER_LOUDSPEAK    = 10012,            // 喇叭消息

    CENTER_ROUTE            = 10013,            // 中心服务器-聊天服务器的路由消息
    CENTER_DOUBLE           = 10014,            // 是否双倍经验
    CENTER_USER_MSG         = 10015,            // 中心服务器给玩家的消息
    CENTER_UNION_MSG        = 10016,           // 中心服务器给玩家的消息
};

enum
{
    CLIENT_COMMAND_LOGON = 0x100,					//用户登陆
    //PACKET {CLIENT_COMMAND_LOGON, MID:INT, KEY:STRING }

    CLIENT_COMMAND_CHAT_TO_FRIEND,					//发送信息给好友.
    //PACKET {CLIENT_COMMAND_CHAT_TO_FRIEND, 好友MID:INT, 消息类型:INT, 消息:STRING }

    CLIENT_COMMAND_CHAT_TO_FRIENDS,					//发送信息给所有好友
    //PACKET {CLIENT_COMMAND_CHAT_TO_FRIENDS, 消息类型:INT, 消息:STRING }
    CLIENT_COMMAND_CHAT_HIDE,                                        //隐身功能
    //PACKET {CLIENT_COMMAND_CHAT_HIDE, 消息类型：INT }
    CLIENT_COMMAND_ENTER_ROOM,					 // 进入房间
    //PACKET {CLIENT_COMMAND_ENTER_ROOM } 
    CLIENT_COMMAND_LEAVE_ROOM,					 // 离开房间

    CLIENT_COMMAND_BROADCAST_USE_MONEY,			 // 大喇叭，有钱人的东东	
    
    CLIENT_COMMAND_UNIONTALK,		             // 工会聊天

    //PACKET {CLIENT_COMMAND_LEAVE_ROOM }
    CLIENT_COMMAND_INVITE_LIST_REQUEST			= 0x2000,  //请求随机邀请列表

    CLIENT_COMMAND_INVITE_PLAYER,					//=2001邀请某人进入房间游戏

    CLIENT_COMMAND_INVITE_RESPONE_REJECT,               		//=2002拒绝某人的邀请

    CLIENT_COMMAND_INVITE_RESPONE_AGREE,			//=2003同意某人的邀请

    CLIENT_COMMAND_PLAYER_DATA,					    //=2004数据信息

    CLIENT_COMMAND_PLAYER_PLACE,					//=2005被邀请者所在状态不能接受邀请时发送的消息	

    CLIENT_CHALLENGE_INVITE_PLAYER,                 //邀请进入副本

    CLIENT_REQUEST_USE_SYS_MESSAGE, 				//=2007	

    CLIENT_RELOAD_ADMIN_MSG,

    CLIENT_COMMAND_INVITE_UNION,                    // 邀请无公会列表

	CMD_ECHO,										// ECHO命令//0X200A

};

enum
{
    SERVER_COMMAND_CHAT_TO_FRIEND = 0x600,			//发送信息给好友.
    //PACKET {SERVER_COMMAND_CHAT_TO_FRIEND, 好友MID:INT, 消息类型:INT, 消息:STRING }

    SERVER_COMMAND_CHAT_TO_FRIENDS,					//发送信息给所有好友
    //PACKET {SERVER_COMMAND_CHAT_TO_FRIENDS, 好友MID:INT,  消息类型:INT, 消息:STRING }

    SERVER_COMMAND_FRIEND_LOGON,					//好友上线
    //PACKET {SERVER_COMMAND_FRIEND_LOGON, MID:INT }

    SERVER_COMMAND_FRIEND_LOGOUT,					//好友下线
    //PACKET {SERVER_COMMAND_FRIEND_LOGOUT, MID:INT }

    SERVER_COMMAND_SYSTEM_NOTIFY,					//系统通知消息

    SERVER_COMMAND_SYSTEM_KICK,					    //用户重复登陆
    //PACKET {SERVER_COMMAND_RELOGON, 错误代码:INT }

    SERVER_COMMAND_LOGIN_SUCCESSFUL,                //用户登录成功消息
    //PACKET {SERVER_COMMAND_LOGIN_SUCCESSFUL, 在线好友数：INT，好友ID：INT.... }

    SERVER_COMMAND_UNIONTALK,		                // 工会聊天

    SERVER_COMMAND_UNION_LOGON,                     // 公会成员上线

    SERVER_COMMAND_UNION_LOGOUT,                    // 公会成员下线

    SERVER_COMMAND_UNION_ONLINES,                   // 公会在线成员                

    SERVER_RESPONE_REQUEST_INVITE_LIST = 0x2020, 	// 玩家请求邀请列表

    SERVER_RESPONE_REQUEST_AGREE,					// 玩家同意邀请

    SERVER_RESPONE_REQUEST_REJECT,					// 玩家拒绝邀请

    SERVER_RESPONE_OFFLINE_NOW,						// 玩家刚好下线

    SERVER_COMMAND_INVITE_PALYER,					// 邀请玩家加入游戏

    SERVER_COMMAND_INVITED_PLAYER_BUSY,				// 通知邀请人被邀请人现在忙

    SERVER_COMMAND_DOUBLE_EXP_FIVE_MINUTE_LEFT,		// 还有五分钟开始双倍经验

    SERVER_COMMAND_DOUBLE_EXP_BEGIN,				// 双倍经验开始

    SERVER_COMMAND_DOUBLE_EXP_END,					// 双倍经验结束

    SERVER_COMMAND_REQUEST_INVITE_LIST_END,			// ......

    SERVER_COMMAND_IN_DOUBLE_EXP,					// 提示双倍经验正在进行中

    SERVER_REQUEST_INVITE_UNION_LIST,               // 返回无公会列表

    SERVER_REQUEST_INVITE_UNION_LIST_END,           // 发送邀请公会列表结束

	SERVER_COMMAND_LOGIN_FAILED	= 8239,				//用户登陆失败

    SERVER_COMMAND_BROADCAST_TO_ALL	= 0x2500,		// 大喇叭广播给所有人

    SERVER_COMMAND_SEND_PCHAT_FAILURE,				// 私聊失败 

    SERVER_COMMAND_SEND_INVITE_PLAYER,				// 副本邀请

    SERVER_COMMAND_SEND_SYS_MESSAGE = 0x2505,

    SERVER_COMMAND_SEND_SYS_PROMOTE_FIVE_MIN_BGBIN = 0x2506,

    SERVER_COMMAND_SEND_SYS_PROMOTE_FIVE_MIN_END = 0x2507,

    SERVER_COMMAND_SEND_REFLUSH_ASSURE           = 0x2508,  // 刷新拍卖

    SERVER_COMMAND_SEND_OUT_UNION                = 0x2509,  // 退出公会

    SERVER_COMMAND_SEND_IN_UNION                 ,  // 加入公会

	GAME_SERVER_ASK_TO_SEND_SYS_MSG,
};

enum
{
	HERO_COMMAND_LOGON = 0x100,					//用户登陆
	HERO_COMMAND_GET_GOODS = 0x101,				//获取货物
	HERO_COMMAND_USER_REGIST = 0x603,			//用户注册 
	HERO_COMMAND_USER_COPYDATA = 0X604,			//新注册的玩家的游戏状态
	HERO_COMMAND_USER_FB_REGIST = 0x605,		//用户FB登陆
	HERO_COMMAND_USER_FB_LOGIN = 0x606,			//用户FB注册


	HERO_COMMAND_USER_GET_REWARD = 0x701,		//领取抽奖协议 

	HERO_COMMAND_ECHO  = 0x20A,				//回应协议.
};

enum
{
	REDIS_TIMER = 3,
};

// 服务器类型分类
enum
{
	SERVER_TYPE_REGIST = 14,
	SERVER_TYPE_MAIL = 24,
	SERVER_TYPE_LOGIC = 10,
	SERVER_TYPE_APN = 8,
};

// hall
enum
{
	SERVER_CMD_SYNC					= 0x001,
	CLIENT_CMD_SYNC					= 0x002,
	SERVER_TO_CENTSERVER_REGIST		= 10000,		// REGIST
	CLIENT_REGIST_GAMEID			= 5002,			//向大厅注册gameid 
	CLIENT_REGIST_NAME				= 5003,			//向大厅注册NAME
	CLIENT_ADDFRIEND_MSG			= 5004,			//向大厅发送添加好友消息

	// regist
	HERO_COMMAND_IPHONE_LOGIN		= 10001,		//IPHONE登陆（要检查注册信息） 
	COMMAND_USER_REGIST				= 10002,		//注册新账号 
	COMMAND_PROFROM_USER_LOGIN		= 10003,		//user login game 
	COMMAND_CHNANGE_USER_PASSWORD	= 10004,		//change password 
	COMMAND_CHNANGE_USER_MAIL		= 10005,		//change mail 
	COMMAND_FB_USER_LOGIN			= 10006,		//FB账号登陆 
	COMMAND_FB_USER_BIND			= 10007,		//FB账号绑定
	COMMAND_DELETE_FRIEND			= 10008,		//刪除好友
	COMMAND_USER_CHANGE_HEADER		= 10009,		//修改头像
	COMMAND_USER_CHANGE_HEADER_REPLY= 1024,			//修改头像返回
	COMMAND_DELETE_FRIEND_REPLY		= 1014,			//刪除好友返回
	COMMAND_ACCEPT_FRIEND_REPLY		= 1011,
	COMMAND_COMLOGIN_REPLY			= 1003,			//登陆通用返回信息 
	COMMAND_REGIST_REPLY			= 1005,			//注册返回信息 
	COMMAND_CPASSWD_REPLY			= 1006,			//修改密码返回
	COMMAND_CEMAIL_REPLY			= 1007,			//修改邮箱返回 
	COMMAND_FBBIND_REPLY			= 1009,			//FB绑定返回 
	COMMAND_FBLOGIN_REPLY			= 1016,			//FB登陆返回

	// logic 
	COMMAND_USER_LOGIN				= 10402,		//登陆logic服务器 
	COMMAND_USER_LOGOUT				= 10403,		//推出logic服务器
	COMMAND_USER_REGISTLOGIN		= 10404,		//注册后登陆logic服务器
	COMMAND_USER_FBLOGIN			= 10406,		//FB登陆logic服务器
	COMMAND_USER_GETFRIEND			= 10405,		//用户获取好友列表
	COMMAND_GET_FRIEND_LIST_REPLY	= 1013,			//获取好友列表返回
	COMMAND_RETURN_BASE_DATA		= 1028,			//获取基地信息返回
	COMMAND_RETURN_PRODUCTION_DATA	= 1029,			//获取用户产品信息返回
	COMMAND_GET_GOODS_PCATE			= 10407,		//获取商品
	COMMAND_GET_GOODS_PCATE_REPLY	= 1030,			//获取商品返回
	COMMAND_BUY_GOODS_PCATE			= 10408,		//购买可以旋转的商品
	COMMAND_BUY_GOODS_PCATE_REPLY	= 1031,			//购买商品返回
	COMMAND_EDIT_GOODS_PID			= 10409,		//编辑基地物品
	COMMAND_EDIT_GOODS_PID_REPLY	= 1032,			//编辑基地物品返回
	COMMAND_GET_ENLARGE_TABLE		= 10410,		//获取扩充表
	COMMAND_GET_ENLARGE_TABLE_REPLY	= 1033,			//获取扩充表返回
	COMMAND_BUY_ENLARGE_TABLE		= 10411,		//购买扩充表
	COMMAND_BUY_ENLARGE_TABLE_REPLY	= 1034,			//购买扩充表返回
	COMMAND_ENLARGE_COMPLEY_REPLY	= 1035,			//扩张完成返回
	COMMAND_SEND_USERMECHINE_REPLY	= 1036,			//返回用户机器
	COMMAND_MECHINE_COMSTRUCT_REPLY	= 1037,				//机器建筑完成的返回

	COMMAND_MECHINE_UPLEVEL_REQUIRE	= 10412,			//获取机器的升级条件
	COMMAND_MECHINE_UPLEVEL_REPLY	= 1038,				//获取机器的升级条件的返回
	COMMAND_MECHINE_UPNEXTLEVEL_REQUIRE	 = 10413,		//机器的升级
	COMMAND_MECHINE_UPNEXTLEVEL_REPLY	 = 1039,		//机器的升级的返回
	COMMAND_MECHINE_UPNEXTLEVEL_COMPLETE = 1040,		//机器的升级完成的返回
	COMMAND_MECHINE_REQUIRE_MECHINE_STATUS			 = 10414,		//获取机器状态
	COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY	 = 1041,		//获取机器状态的返回
	COMMAND_MECHINE_INFO_CANCOLLECT_REPLY			 = 1042,		//情报机器可以收集的返回
	COMMAND_MECHINE_COLLECT_INFO_REQURITE			 = 10416,		//机器收集
	COMMAND_MECHINE__COLLECT_INFO_REPLY				 = 1043,		//情报机器收集的返回

	COMMAND_MECHINE_REQUIRE_SPEED		 = 10422,		//机器加速
	COMMAND_MECHINE_REQUIRE_SPEED_REPLY	 = 1050,		//机器加速的返回
	COMMAND_MECHINE_REQUIRE_COMFOR		 = 10423,		//机器升级完成确认
	COMMAND_MECHINE_REQUIRE_COMFOR_REPLY = 1051,		//机器升级完成确认的返回
	COMMAND_MECHINE_REQUIRE_INFO_COLLECT_SPEED		 = 10424,		//情报机器收集加速请求
	COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY		 = 1052,		//情报机器收集加速请求的返回

	COMMAND_GET_PROPSSYNTHESIS_CONDITION       = 10420,        //获取道具合成条件请求
	COMMAND_GET_PROPSSYNTHESIS_CONDITION_REPLY = 1048,       //获取道具合成条件请求返回

	COMMAND_GET_RESOURCEPRODUC_CONDITION       = 10425,     //获取能源机生产条件请求
    COMMAND_GET_RESOURCEPRODUC_CONDITION_REPLY = 1053,     //获取能源机生产条件返回

	COMMAND_MECHINE_RESOURCEPRODUC        = 10417,         //能源机生产
	COMMAND_MECHINE_RESOURCEPRODUC_REPLAY = 1044,          //能源机生产返回


	// mail server
	COMMAND_FIND_USER_PASSWORD		= 11401,		//查找密码,发到邮箱里面 
	COMMAND_FINDPASS_REPLY			= 1008,			//找回密码返回 

	// apn server
	COMMAND_WANT_ADD_FRIEND			= 11102,		//邀请好友
	COMMAND_GET_SYSTEM_MESSAGE		= 11105,		//系统消息
	COMMAND_GET_SYSTEM_MSG_REPLY	= 1023,			//系统消息返回
	COMMAND_GET_USER_MESSAGE		= 11108,		//用户消息返回
	COMMAND_GET_USER_MSG_REPLY		= 1021,			//用户消息
	COMMAND_ADDFRIEND_REPLY			= 1010,			//好友登陆
	COMMAND_GET_MOREGAMELIST_MESSAGE= 11106,		//获取更多游戏
	COMMAND_CHANGE_MESSAGE_STATE	= 11107,		//改變消息狀態
	COMMAND_CHANGE_MESSAGE_REPLY	= 1012,			//改變消息狀態返回 
	COMMAND_SEND_MESSAGE_TO_SERVER  = 11110,		//发送消息给服务器
	COMMAND_ACCEPT_ADD_FRIEND		= 11103,		//接受好友請求
	COMMAND_GET_MOREGAME_REPLY		= 1022,			//获取更多游戏返回
};

enum
{
	TIMER_CHECK_SOMETHING = 1,
	TIMER_WRITEDB_SOMETHING = 2,
};

#endif //GAME_CMD_H__20110530

