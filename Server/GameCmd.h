#ifndef GAME_CMD_H__20110530
#define GAME_CMD_H__20110530

namespace AntWars
{
	namespace Client
	{
		//�ͻ��������÷������б� ���ڲ��ҽ��뷿��
		const static int CLIENT_REQUEST_SERVER_LIST			= 1;
		// PACKET { CMD:CLIENT_REQUEST_SERVER_LIST, MID:UINT }

		//�ͻ��������ù�������ս�����б�
		const static int CLIENT_REQUEST_FREEWAR_LIST		= 2;
		// PACKET { CMD:CLIENT_REQUEST_FREEWAR_LIST, MID:UINT, ҳ��:INT }

		//�ͻ��������ø��������б�
		const static int CLIENT_REQUEST_COPY_LIST			= 3;
		// PACKET { CMD:CLIENT_REQUEST_COPY_LIST, MID:UINT, ҳ��:INT }

		//�ͻ����������һ��������, allocate.����ط�Ҫ�Ż����ͻ��˻���һ���ϴ�����Ľ�������ǽ��������������ٴε��ñ�����
		const static int CLIENT_REQUEST_ALLOCATE_SERVER		= 4;
		// PACKET { CMD:CLIENT_REQUEST_ALLOCATE_SERVER, MID:UINT, ����ķ���������:INT }
	};

	namespace Mgr
	{
		const static int CLIENT_COMMADN_SYSTEM_UPDATE = 1013; //������·���������������
	}

	namespace Server
	{
		//��������Ӧ���÷������б�
		//response
		const static int SERVER_RESPONSE_SERVER_LIST		= 1;
		// PACKET { CMD:SERVER_RESPONSE_SERVER_LIST, ��������¼����:INT, [������ID:INT, ����:INT,  IP:STRING, PORT:INT,]}

		//�������˷��ع�������ս�����б�
		const static int SERVER_RESPONSE_FREEWAR_LIST		= 2;
		// PACKET { CMD:SERVER_RESPONSE_FREEWAR_LIST, ��������:INT [������Ϣ, ...] }

		//�ͻ��������ø��������б�
		const static int SERVER_RESPONSE_COPY_LIST			= 3;
		// PACKET { CMD:CLIENT_REQUEST_COPY_LIST, MID:UINT, ҳ��:INT }

		//�ͻ����������һ��������, allocate.����ط�Ҫ�Ż����ͻ��˻���һ���ϴ�����Ľ�������ǽ��������������ٴε��ñ�����
		const static int SERVER_RESPONSE_ALLOCATE_SERVER	= 4;
		// PACKET { CMD:CLIENT_REQUEST_ALLOCATE_SERVER, MID:UINT, ����ķ���������:INT }
	};
};


// �����ķ���������cmd
enum emNETMSGCOMMOND
{
    CHATSERVER_INFO         = 10010,            // ���������ע����Ϣ
    CHATSERVER_ROUTE        = 10011,            // �����������·�ɹ㲥��Ϣ
    CHATSERVER_LOUDSPEAK    = 10012,            // ������Ϣ

    CENTER_ROUTE            = 10013,            // ���ķ�����-�����������·����Ϣ
    CENTER_DOUBLE           = 10014,            // �Ƿ�˫������
    CENTER_USER_MSG         = 10015,            // ���ķ���������ҵ���Ϣ
    CENTER_UNION_MSG        = 10016,           // ���ķ���������ҵ���Ϣ
};

enum
{
    CLIENT_COMMAND_LOGON = 0x100,					//�û���½
    //PACKET {CLIENT_COMMAND_LOGON, MID:INT, KEY:STRING }

    CLIENT_COMMAND_CHAT_TO_FRIEND,					//������Ϣ������.
    //PACKET {CLIENT_COMMAND_CHAT_TO_FRIEND, ����MID:INT, ��Ϣ����:INT, ��Ϣ:STRING }

    CLIENT_COMMAND_CHAT_TO_FRIENDS,					//������Ϣ�����к���
    //PACKET {CLIENT_COMMAND_CHAT_TO_FRIENDS, ��Ϣ����:INT, ��Ϣ:STRING }
    CLIENT_COMMAND_CHAT_HIDE,                                        //������
    //PACKET {CLIENT_COMMAND_CHAT_HIDE, ��Ϣ���ͣ�INT }
    CLIENT_COMMAND_ENTER_ROOM,					 // ���뷿��
    //PACKET {CLIENT_COMMAND_ENTER_ROOM } 
    CLIENT_COMMAND_LEAVE_ROOM,					 // �뿪����

    CLIENT_COMMAND_BROADCAST_USE_MONEY,			 // �����ȣ���Ǯ�˵Ķ���	
    
    CLIENT_COMMAND_UNIONTALK,		             // ��������

    //PACKET {CLIENT_COMMAND_LEAVE_ROOM }
    CLIENT_COMMAND_INVITE_LIST_REQUEST			= 0x2000,  //������������б�

    CLIENT_COMMAND_INVITE_PLAYER,					//=2001����ĳ�˽��뷿����Ϸ

    CLIENT_COMMAND_INVITE_RESPONE_REJECT,               		//=2002�ܾ�ĳ�˵�����

    CLIENT_COMMAND_INVITE_RESPONE_AGREE,			//=2003ͬ��ĳ�˵�����

    CLIENT_COMMAND_PLAYER_DATA,					    //=2004������Ϣ

    CLIENT_COMMAND_PLAYER_PLACE,					//=2005������������״̬���ܽ�������ʱ���͵���Ϣ	

    CLIENT_CHALLENGE_INVITE_PLAYER,                 //������븱��

    CLIENT_REQUEST_USE_SYS_MESSAGE, 				//=2007	

    CLIENT_RELOAD_ADMIN_MSG,

    CLIENT_COMMAND_INVITE_UNION,                    // �����޹����б�

	CMD_ECHO,										// ECHO����//0X200A

};

enum
{
    SERVER_COMMAND_CHAT_TO_FRIEND = 0x600,			//������Ϣ������.
    //PACKET {SERVER_COMMAND_CHAT_TO_FRIEND, ����MID:INT, ��Ϣ����:INT, ��Ϣ:STRING }

    SERVER_COMMAND_CHAT_TO_FRIENDS,					//������Ϣ�����к���
    //PACKET {SERVER_COMMAND_CHAT_TO_FRIENDS, ����MID:INT,  ��Ϣ����:INT, ��Ϣ:STRING }

    SERVER_COMMAND_FRIEND_LOGON,					//��������
    //PACKET {SERVER_COMMAND_FRIEND_LOGON, MID:INT }

    SERVER_COMMAND_FRIEND_LOGOUT,					//��������
    //PACKET {SERVER_COMMAND_FRIEND_LOGOUT, MID:INT }

    SERVER_COMMAND_SYSTEM_NOTIFY,					//ϵͳ֪ͨ��Ϣ

    SERVER_COMMAND_SYSTEM_KICK,					    //�û��ظ���½
    //PACKET {SERVER_COMMAND_RELOGON, �������:INT }

    SERVER_COMMAND_LOGIN_SUCCESSFUL,                //�û���¼�ɹ���Ϣ
    //PACKET {SERVER_COMMAND_LOGIN_SUCCESSFUL, ���ߺ�������INT������ID��INT.... }

    SERVER_COMMAND_UNIONTALK,		                // ��������

    SERVER_COMMAND_UNION_LOGON,                     // �����Ա����

    SERVER_COMMAND_UNION_LOGOUT,                    // �����Ա����

    SERVER_COMMAND_UNION_ONLINES,                   // �������߳�Ա                

    SERVER_RESPONE_REQUEST_INVITE_LIST = 0x2020, 	// ������������б�

    SERVER_RESPONE_REQUEST_AGREE,					// ���ͬ������

    SERVER_RESPONE_REQUEST_REJECT,					// ��Ҿܾ�����

    SERVER_RESPONE_OFFLINE_NOW,						// ��Ҹպ�����

    SERVER_COMMAND_INVITE_PALYER,					// ������Ҽ�����Ϸ

    SERVER_COMMAND_INVITED_PLAYER_BUSY,				// ֪ͨ�����˱�����������æ

    SERVER_COMMAND_DOUBLE_EXP_FIVE_MINUTE_LEFT,		// ��������ӿ�ʼ˫������

    SERVER_COMMAND_DOUBLE_EXP_BEGIN,				// ˫�����鿪ʼ

    SERVER_COMMAND_DOUBLE_EXP_END,					// ˫���������

    SERVER_COMMAND_REQUEST_INVITE_LIST_END,			// ......

    SERVER_COMMAND_IN_DOUBLE_EXP,					// ��ʾ˫���������ڽ�����

    SERVER_REQUEST_INVITE_UNION_LIST,               // �����޹����б�

    SERVER_REQUEST_INVITE_UNION_LIST_END,           // �������빫���б����

	SERVER_COMMAND_LOGIN_FAILED	= 8239,				//�û���½ʧ��

    SERVER_COMMAND_BROADCAST_TO_ALL	= 0x2500,		// �����ȹ㲥��������

    SERVER_COMMAND_SEND_PCHAT_FAILURE,				// ˽��ʧ�� 

    SERVER_COMMAND_SEND_INVITE_PLAYER,				// ��������

    SERVER_COMMAND_SEND_SYS_MESSAGE = 0x2505,

    SERVER_COMMAND_SEND_SYS_PROMOTE_FIVE_MIN_BGBIN = 0x2506,

    SERVER_COMMAND_SEND_SYS_PROMOTE_FIVE_MIN_END = 0x2507,

    SERVER_COMMAND_SEND_REFLUSH_ASSURE           = 0x2508,  // ˢ������

    SERVER_COMMAND_SEND_OUT_UNION                = 0x2509,  // �˳�����

    SERVER_COMMAND_SEND_IN_UNION                 ,  // ���빫��

	GAME_SERVER_ASK_TO_SEND_SYS_MSG,
};

enum
{
	HERO_COMMAND_LOGON = 0x100,					//�û���½
	HERO_COMMAND_GET_GOODS = 0x101,				//��ȡ����
	HERO_COMMAND_USER_REGIST = 0x603,			//�û�ע�� 
	HERO_COMMAND_USER_COPYDATA = 0X604,			//��ע�����ҵ���Ϸ״̬
	HERO_COMMAND_USER_FB_REGIST = 0x605,		//�û�FB��½
	HERO_COMMAND_USER_FB_LOGIN = 0x606,			//�û�FBע��


	HERO_COMMAND_USER_GET_REWARD = 0x701,		//��ȡ�齱Э�� 

	HERO_COMMAND_ECHO  = 0x20A,				//��ӦЭ��.
};

enum
{
	REDIS_TIMER = 3,
};

// ���������ͷ���
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
	CLIENT_REGIST_GAMEID			= 5002,			//�����ע��gameid 
	CLIENT_REGIST_NAME				= 5003,			//�����ע��NAME
	CLIENT_ADDFRIEND_MSG			= 5004,			//�����������Ӻ�����Ϣ

	// regist
	HERO_COMMAND_IPHONE_LOGIN		= 10001,		//IPHONE��½��Ҫ���ע����Ϣ�� 
	COMMAND_USER_REGIST				= 10002,		//ע�����˺� 
	COMMAND_PROFROM_USER_LOGIN		= 10003,		//user login game 
	COMMAND_CHNANGE_USER_PASSWORD	= 10004,		//change password 
	COMMAND_CHNANGE_USER_MAIL		= 10005,		//change mail 
	COMMAND_FB_USER_LOGIN			= 10006,		//FB�˺ŵ�½ 
	COMMAND_FB_USER_BIND			= 10007,		//FB�˺Ű�
	COMMAND_DELETE_FRIEND			= 10008,		//�h������
	COMMAND_USER_CHANGE_HEADER		= 10009,		//�޸�ͷ��
	COMMAND_USER_CHANGE_HEADER_REPLY= 1024,			//�޸�ͷ�񷵻�
	COMMAND_DELETE_FRIEND_REPLY		= 1014,			//�h�����ѷ���
	COMMAND_ACCEPT_FRIEND_REPLY		= 1011,
	COMMAND_COMLOGIN_REPLY			= 1003,			//��½ͨ�÷�����Ϣ 
	COMMAND_REGIST_REPLY			= 1005,			//ע�᷵����Ϣ 
	COMMAND_CPASSWD_REPLY			= 1006,			//�޸����뷵��
	COMMAND_CEMAIL_REPLY			= 1007,			//�޸����䷵�� 
	COMMAND_FBBIND_REPLY			= 1009,			//FB�󶨷��� 
	COMMAND_FBLOGIN_REPLY			= 1016,			//FB��½����

	// logic 
	COMMAND_USER_LOGIN				= 10402,		//��½logic������ 
	COMMAND_USER_LOGOUT				= 10403,		//�Ƴ�logic������
	COMMAND_USER_REGISTLOGIN		= 10404,		//ע����½logic������
	COMMAND_USER_FBLOGIN			= 10406,		//FB��½logic������
	COMMAND_USER_GETFRIEND			= 10405,		//�û���ȡ�����б�
	COMMAND_GET_FRIEND_LIST_REPLY	= 1013,			//��ȡ�����б���
	COMMAND_RETURN_BASE_DATA		= 1028,			//��ȡ������Ϣ����
	COMMAND_RETURN_PRODUCTION_DATA	= 1029,			//��ȡ�û���Ʒ��Ϣ����
	COMMAND_GET_GOODS_PCATE			= 10407,		//��ȡ��Ʒ
	COMMAND_GET_GOODS_PCATE_REPLY	= 1030,			//��ȡ��Ʒ����
	COMMAND_BUY_GOODS_PCATE			= 10408,		//���������ת����Ʒ
	COMMAND_BUY_GOODS_PCATE_REPLY	= 1031,			//������Ʒ����
	COMMAND_EDIT_GOODS_PID			= 10409,		//�༭������Ʒ
	COMMAND_EDIT_GOODS_PID_REPLY	= 1032,			//�༭������Ʒ����
	COMMAND_GET_ENLARGE_TABLE		= 10410,		//��ȡ�����
	COMMAND_GET_ENLARGE_TABLE_REPLY	= 1033,			//��ȡ�������
	COMMAND_BUY_ENLARGE_TABLE		= 10411,		//���������
	COMMAND_BUY_ENLARGE_TABLE_REPLY	= 1034,			//�����������
	COMMAND_ENLARGE_COMPLEY_REPLY	= 1035,			//������ɷ���
	COMMAND_SEND_USERMECHINE_REPLY	= 1036,			//�����û�����
	COMMAND_MECHINE_COMSTRUCT_REPLY	= 1037,				//����������ɵķ���

	COMMAND_MECHINE_UPLEVEL_REQUIRE	= 10412,			//��ȡ��������������
	COMMAND_MECHINE_UPLEVEL_REPLY	= 1038,				//��ȡ���������������ķ���
	COMMAND_MECHINE_UPNEXTLEVEL_REQUIRE	 = 10413,		//����������
	COMMAND_MECHINE_UPNEXTLEVEL_REPLY	 = 1039,		//�����������ķ���
	COMMAND_MECHINE_UPNEXTLEVEL_COMPLETE = 1040,		//������������ɵķ���
	COMMAND_MECHINE_REQUIRE_MECHINE_STATUS			 = 10414,		//��ȡ����״̬
	COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY	 = 1041,		//��ȡ����״̬�ķ���
	COMMAND_MECHINE_INFO_CANCOLLECT_REPLY			 = 1042,		//�鱨���������ռ��ķ���
	COMMAND_MECHINE_COLLECT_INFO_REQURITE			 = 10416,		//�����ռ�
	COMMAND_MECHINE__COLLECT_INFO_REPLY				 = 1043,		//�鱨�����ռ��ķ���

	COMMAND_MECHINE_REQUIRE_SPEED		 = 10422,		//��������
	COMMAND_MECHINE_REQUIRE_SPEED_REPLY	 = 1050,		//�������ٵķ���
	COMMAND_MECHINE_REQUIRE_COMFOR		 = 10423,		//�����������ȷ��
	COMMAND_MECHINE_REQUIRE_COMFOR_REPLY = 1051,		//�����������ȷ�ϵķ���
	COMMAND_MECHINE_REQUIRE_INFO_COLLECT_SPEED		 = 10424,		//�鱨�����ռ���������
	COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY		 = 1052,		//�鱨�����ռ���������ķ���

	COMMAND_GET_PROPSSYNTHESIS_CONDITION       = 10420,        //��ȡ���ߺϳ���������
	COMMAND_GET_PROPSSYNTHESIS_CONDITION_REPLY = 1048,       //��ȡ���ߺϳ��������󷵻�

	COMMAND_GET_RESOURCEPRODUC_CONDITION       = 10425,     //��ȡ��Դ��������������
    COMMAND_GET_RESOURCEPRODUC_CONDITION_REPLY = 1053,     //��ȡ��Դ��������������

	COMMAND_MECHINE_RESOURCEPRODUC        = 10417,         //��Դ������
	COMMAND_MECHINE_RESOURCEPRODUC_REPLAY = 1044,          //��Դ����������


	// mail server
	COMMAND_FIND_USER_PASSWORD		= 11401,		//��������,������������ 
	COMMAND_FINDPASS_REPLY			= 1008,			//�һ����뷵�� 

	// apn server
	COMMAND_WANT_ADD_FRIEND			= 11102,		//�������
	COMMAND_GET_SYSTEM_MESSAGE		= 11105,		//ϵͳ��Ϣ
	COMMAND_GET_SYSTEM_MSG_REPLY	= 1023,			//ϵͳ��Ϣ����
	COMMAND_GET_USER_MESSAGE		= 11108,		//�û���Ϣ����
	COMMAND_GET_USER_MSG_REPLY		= 1021,			//�û���Ϣ
	COMMAND_ADDFRIEND_REPLY			= 1010,			//���ѵ�½
	COMMAND_GET_MOREGAMELIST_MESSAGE= 11106,		//��ȡ������Ϸ
	COMMAND_CHANGE_MESSAGE_STATE	= 11107,		//��׃��Ϣ��B
	COMMAND_CHANGE_MESSAGE_REPLY	= 1012,			//��׃��Ϣ��B���� 
	COMMAND_SEND_MESSAGE_TO_SERVER  = 11110,		//������Ϣ��������
	COMMAND_ACCEPT_ADD_FRIEND		= 11103,		//���ܺ���Ո��
	COMMAND_GET_MOREGAME_REPLY		= 1022,			//��ȡ������Ϸ����
};

enum
{
	TIMER_CHECK_SOMETHING = 1,
	TIMER_WRITEDB_SOMETHING = 2,
};

#endif //GAME_CMD_H__20110530

