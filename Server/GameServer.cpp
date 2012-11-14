#include <time.h>
#include <sstream>
#include <algorithm>

#include "GameCmd.h"
#include "log.h"
#include "GameServer.h"
#include "SocketHandler.h"
#include "DBOperator.h"
#include "PlatFunc.h"
#include "ConfigMgr.h"
#include "Player.h"
#include "PlayerMgr.h"
#include "CentServer.h"
#include "json/json.h"
#include "TaskHandler.h"
#include "RegistSystem.h"
#include "MD5.h"
#include "LuckySystem.h"
#include "StoreSystem.h"
#include "RankingSystem.h"
#include "MailSystem.h"

namespace
{
	static  const string s_strAdminKey = "123456789!@@##$$%%%^^";
};

const string CGameServer::m_cSecretKey = "123456789";	// 万能KEY

CGameServer::CGameServer(void):m_pCentServer(NULL)
{

}

CGameServer::~CGameServer(void)
{
    if(m_pCentServer)
    {
        delete m_pCentServer;
        m_pCentServer = NULL;
    }
}
bool CGameServer::InitServer()
{
	// get the db date
	//g_store().load_price_from_db();
	//g_GetCDBOperator().GetGoodsBySid(ID_JIANZHU, g_Goodslist);
	//g_rank.Load_RankTable();

    // 连接中心服务器
    m_pCentServer = new CentServer(this);
    NetAddr CentAddr ; 
    CentServerConfig cfg = g_GetConfigMgr().getCentServerConfig();
    CentAddr.host = cfg.host;
    CentAddr.port = atoi(cfg.port.c_str());
    
	// connect to hall
    return m_pCentServer->InitConnect(CentAddr);
}

void CGameServer::ShutDown()
{

}

bool myfunction (UserMessageInfo i,UserMessageInfo j) { return (i.msgID<j.msgID); }

bool myfunction2 (SystemMessageInfo i,SystemMessageInfo j) { return (i.msgID<j.msgID); }

int CGameServer::ProcessPacket(NETInputPacket *pPacket, SocketHandler *pHandler, DWORD dwSessionID)
{
    short nMsgType = pPacket->GetCmdType();
	
	// regist
	if (HERO_COMMAND_USER_REGIST == nMsgType)
	{
		return Handle_Regist_User( pHandler, pPacket );
	}

	// new login
	if (HERO_COMMAND_IPHONE_LOGIN == nMsgType)
	{
		if (HERO_COMMAND_IPHONE_LOGIN == nMsgType)
		{
			return Handle_Iphone_Login( (TcpHandler *)pHandler, pPacket );
		}
		//if (HERO_COMMAND_USER_LOGIN == nMsgType)
		//{
		//	return Handle_User_Login( pHandler, pPacket );
		//}
	}

    if(pHandler->GetUserData() == NULL)
        return -1;

	switch( nMsgType )
	{  
	case HERO_COMMAND_ECHO:                              
		Handle_Echo( pHandler, pPacket );
		break;
	case HERO_COMMAND_GET_GOODS:
		Handle_GetGoods( pHandler, pPacket );
		break;
	case HERO_COMMAND_USER_REGIST:
		Handle_Regist_User( pHandler, pPacket );
		break;
	case HERO_COMMAND_USER_GET_REWARD:
		Handle_User_GetReward( pHandler, pPacket );
		break;
	default:
		log_error("收到未知数据包 %d \n",nMsgType);
		//m_pSMgr->ProcessPacket(pPacket, pHandler);
		break;
	}
	return 0;
}

void  CGameServer::OnConnect(SocketHandler *pHandler )
{
    SocketServer::OnConnect(pHandler);
}

void  CGameServer::OnDisconnect(SocketHandler *pHandler )
{ 
   ProcessClose(pHandler);
   SocketServer::OnDisconnect(pHandler);
}

//处理用户掉线
int CGameServer::ProcessClose(TcpHandler *pHandler)
{
   CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );

   if( pUser != NULL )
    {
		log_debug( "ProcessClose user one, user id is %d\n", pUser->getID());
        OnLogout( pUser );
		log_debug( "ProcessClose user two, user id is %d\n", pUser->getID());
        pHandler->SetUserData( NULL );
		log_debug( "free user date, user id is %d\n", pUser->getID());
        delete pUser;
        pUser = NULL;		
    }

	log_debug( "ProcessClose user three\n");

    return 0;
}

void CGameServer::Handle_Echo(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string msg 	= pPacket->ReadString();

	NETOutputPacket packet;
	packet.Begin( HERO_COMMAND_ECHO );
	packet.WriteString( msg );		
	packet.End();

	pHandler->Send(&packet);
}

void CGameServer::Handle_GetGoods(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int Sid = pPacket->ReadInt();
	log_debug("the sid is %d", Sid);

	GoodsStruct_t_list Goodslist;

	if (Sid == ID_JIANZHU)
	{
		Goodslist = g_Goodslist;
	}
	else
	{
		log_error("Get the wrong Sid Goods");
	}

	// send to user
	int len = Goodslist.size();
	NETOutputPacket packet;
	packet.Begin( HERO_COMMAND_GET_GOODS );
	packet.WriteShort( len );	
	GoodsStruct_t s;

	for (int i = 0; i < len; i++)
	{
		s = Goodslist[i];
		packet.WriteShort( s.cid );
		packet.WriteString(s.sname);
		packet.WriteString(s.size);
		packet.WriteString(s.position);
		packet.WriteShort(s.isupgrate);
		packet.WriteShort(s.nums);
		packet.WriteShort(s.issell);
		packet.WriteShort(s.ishouse);
		packet.WriteShort(s.people);
		packet.WriteShort(s.consumeTime);
		packet.WriteShort(s.buildResource);
		packet.WriteShort(s.requireBaseLevel);
	}

	packet.End();

	pHandler->Send(&packet);
}

int CGameServer::Handle_Iphone_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	//1. get game id
	log_debug("handle iphone login, the proxyid is %d\n", ProxyId);

	int gameMid = 0;
	string IphoneMac = pPacket->ReadString();
	string NickName = pPacket->ReadString();
 	bool ret = g_GetRegistSystem().Iphone_Login_Regist(IphoneMac, NickName, gameMid);
	if (!ret)
	{
		log_error("hand iphone login error");
		SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
		return -1;
	} 
	log_debug("the game id is %d\n", gameMid);

	if (ProxyId != 0)
	{
		// 帮助客户端向大厅注册游戏ID
		NETOutputPacket registpacket;
		registpacket.Begin(CLIENT_REGIST_GAMEID);
		registpacket.WriteInt(ProxyId);//0失败1成功
		registpacket.WriteInt(gameMid);//0失败1成功
		registpacket.End();
		pHandler->Send(&registpacket);
	}

	//2. get user info and return
	NETOutputPacket ploginPacket;
	g_GetCDBOperator().GetLRegistReply(gameMid, ploginPacket);
	//是否有邮箱 0没有邮箱，1有邮箱
	ploginPacket.WriteShort(0);
	ploginPacket.End();
	pHandler->Send(&ploginPacket);

	return 0;
}

int CGameServer::Handle_FB_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	//1. get game id
	log_debug("\n\nhandle fb login, the proxyid is %d, ENTER\n", ProxyId);

	int gameMid = 0;
	string FbID = pPacket->ReadString();
	string mail;
	string name;
	string password;
	bool ret = g_GetCDBOperator().CheckFBUser(FbID, gameMid, mail, password, name);
	if (!ret || gameMid == 0)
	{
		log_error("hand fb login error, not bind yet! need to bind! game id is %d\n", gameMid);
		SendCommonFlagMsgReply( COMMAND_FBLOGIN_REPLY, 2, pHandler);
		return 0;
	} 
	log_debug("fb the game id is %d\n", gameMid);

	if (ProxyId != 0)
	{
		// 帮助客户端向大厅注册游戏ID
		NETOutputPacket registpacket;
		registpacket.Begin(CLIENT_REGIST_GAMEID);
		registpacket.WriteInt(ProxyId);
		registpacket.WriteInt(gameMid);
		registpacket.End();
		pHandler->Send(&registpacket);

		Handle_User_Regist_UserName(pHandler, name, gameMid, ProxyId);
	}

	//2. get user info and return
	NETOutputPacket ploginPacket;
	g_GetCDBOperator().GetLRegistReply(gameMid, ploginPacket);
	//是否有邮箱 0没有邮箱，1有邮箱
	ploginPacket.WriteShort(1);
	ploginPacket.WriteString(mail);
	ploginPacket.WriteString(name);
	ploginPacket.WriteString(password);
	ploginPacket.End();

	ploginPacket.SetBegin(COMMAND_USER_FBLOGIN);

	pHandler->Send(&ploginPacket);

	log_debug("\n\nhandle fb login, the proxyid is %d, LEAVE\n", ProxyId);
	return 0;
}

int CGameServer::Handle_User_Login(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string UserName = pPacket->ReadString();
	string PassWord = pPacket->ReadString();
	string md5string = g_GetRegistSystem().Get_String_Md5(PassWord);

	int gameMid = 0;
	if (!g_GetCDBOperator().CheckUser(UserName, md5string, gameMid))
	{
		SendCommonFlagMsgReply(COMMAND_COMLOGIN_REPLY, 1, pHandler);
		return 0;
	}
	bool ret = g_GetCDBOperator().GetUSerGid(UserName, gameMid);
	// update user login time
	// return username logintime email 
	if (!ret)
	{
		SendCommonFlagMsgReply(COMMAND_COMLOGIN_REPLY, 1, pHandler);
		return 0;
	}
	if (ProxyId != 0)
	{
		// 帮助客户端向大厅注册游戏ID
		NETOutputPacket registpacket;
		registpacket.Begin(CLIENT_REGIST_GAMEID);
		registpacket.WriteInt(ProxyId);//0失败1成功
		registpacket.WriteInt(gameMid);//0失败1成功
		registpacket.End();
		pHandler->Send(&registpacket);
		Handle_User_Regist_UserName(pHandler, UserName, gameMid, ProxyId);
	}

	//2. get user info and return
	NETOutputPacket ploginPacket;
	g_GetCDBOperator().GetLRegistReply(gameMid, ploginPacket);
	//是否有邮箱 0没有邮箱，1有邮箱
	// get user mail
	ploginPacket.WriteShort(0);
	ploginPacket.End();
	pHandler->Send(&ploginPacket);
	return 0;
}

int CGameServer::Handle_User_ChangePassWord(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string UserName = pPacket->ReadString();
	string OldPassWord = pPacket->ReadString();
	string NewPassWord = pPacket->ReadString();

	// check the user 
	string md5string = g_GetRegistSystem().Get_String_Md5(OldPassWord);
	int gameMid = 0;
	if (!g_GetCDBOperator().CheckUser(UserName, md5string, gameMid))
	{
		SendCommonFlagMsgReply(COMMAND_CPASSWD_REPLY, 1, pHandler);
		return 0;
	}

	// change the db, two table change
	string newmd5string = g_GetRegistSystem().Get_String_Md5(NewPassWord);
	if (!g_GetCDBOperator().ChangeUserPassword(UserName, newmd5string, NewPassWord))
	{
		SendCommonFlagMsgReply(COMMAND_CPASSWD_REPLY, 1, pHandler);
		return 0;
	}

	SendCommonFlagMsgReply(COMMAND_CPASSWD_REPLY, 0, pHandler);
	return 0;
}

int CGameServer::Handle_User_ChangeMail(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string UserName = pPacket->ReadString();
	string PassWord = pPacket->ReadString();
	string email = pPacket->ReadString();
	int isPub = pPacket->ReadInt();

	// check the user 
	string md5string = g_GetRegistSystem().Get_String_Md5(PassWord);
	int gameMid = 0;
	if (!g_GetCDBOperator().CheckUser(UserName, md5string, gameMid))
	{
		SendCommonFlagMsgReply(COMMAND_CEMAIL_REPLY, 1, pHandler);
		return 0;
	}

	// change the db, two table change
	if (!g_GetCDBOperator().ChangeUserEmail(UserName, email, isPub))
	{
		SendCommonFlagMsgReply(COMMAND_CEMAIL_REPLY, 1, pHandler);
		return 0;
	}

	SendCommonFlagMsgReply(COMMAND_CEMAIL_REPLY, 0, pHandler);
	return 0;
}

int CGameServer::Handle_User_FBBINGDING(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string FBID = pPacket->ReadString();
	string UserName = pPacket->ReadString();
	string PassWord = pPacket->ReadString();
	int flag = pPacket->ReadInt();

	log_debug("the flag is %d\n", flag);

	string userName;
	string password;

	bool canbind = false;
	// check
	int gameMid = 0;
	string md5string = g_GetRegistSystem().Get_String_Md5(PassWord);

	if (flag == 2)
	{
		log_debug("enter unbind\n");
		if (!g_GetCDBOperator().CheckFBUserBinding(FBID, gameMid, userName, password))
		{
			log_debug("can't find the user %s\n", FBID.c_str());
			SendCommonFlagMsgReply(COMMAND_FBBIND_REPLY, 2, pHandler);	// 找不到
			return 0;
		}

		if (userName == UserName && md5string == password)
		{
			canbind = true;
		}
		else
		{
			canbind = false;
		}
		
		if (canbind)
		{
			log_debug("fb can unbind\n");
			if (!g_GetCDBOperator().BindFBuser("0", gameMid, UserName, md5string))
			{
				// bind error
				SendCommonFlagMsgReply(COMMAND_FBBIND_REPLY, 1, pHandler);
				log_debug("leave unbind\n");
				return 0;
			}
			SendCommonFlagMsgReply(COMMAND_FBBIND_REPLY, 0, pHandler);
			log_debug("leave unbind\n");
			return 0;
		}
		log_debug("unbind fb id error, can't unbind\n");
		SendCommonFlagMsgReply(COMMAND_FBBIND_REPLY, 1, pHandler);
		log_debug("leave unbind\n");
		return 0;
	}

	log_debug("enter bind\n");

	// bind
	if (!g_GetCDBOperator().CheckFBUserBinding2(FBID, gameMid, UserName, password))
	{
		// not bind yet, so bind
		canbind = true;
		if (md5string == password)
		{
			canbind = true;
		}
		else
		{
			canbind = false;
		}
	}
	else
	{
		SendCommonFlagMsgReply(COMMAND_FBBIND_REPLY, 3, pHandler);
		return 0;
	}

	if (canbind)
	{
		if (!g_GetCDBOperator().BindFBuser(FBID, gameMid, UserName, md5string))
		{
			// bind error
			SendCommonFlagMsgReply(COMMAND_FBBIND_REPLY, 1, pHandler);
			return 0;
		}
		SendCommonFlagMsgReply(COMMAND_FBBIND_REPLY, 0, pHandler);
		return 0;
	}

	SendCommonFlagMsgReply(COMMAND_FBBIND_REPLY, 1, pHandler);
	log_debug("leave bind\n");
	return 0;
}

int CGameServer::Handle_User_Change_Header_Type(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string userNmae = pPacket->ReadString();
	int type = pPacket->ReadInt();

	if (type < 0)
	{
		SendCommonFlagMsgReply(COMMAND_USER_CHANGE_HEADER_REPLY, 1, pHandler);
		return 0;
	}
	if (!g_GetCDBOperator().UserChangeHeader(userNmae, type))
	{
		SendCommonFlagMsgReply(COMMAND_USER_CHANGE_HEADER_REPLY, 1, pHandler);
		return 0;
	}
	SendCommonFlagMsgReply(COMMAND_USER_CHANGE_HEADER_REPLY, 0, pHandler);
	return 0;
}

int CGameServer::Handle_User_FindPassWord(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string UserName = pPacket->ReadString();
	// get address and password
	string mail, password;
	if (!g_GetCDBOperator().GetUserMailPassword(UserName, mail, password))
	{
		// error 3 , no such user
		SendCommonFlagMsgReply(COMMAND_FINDPASS_REPLY, 3, pHandler);
		return 0;
	}
	int ret = g_mail.SendEmail(mail, password);
	SendCommonFlagMsgReply(COMMAND_FINDPASS_REPLY, ret, pHandler);
	return 0;
}

int CGameServer::Handle_User_Change_Message_State(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string UserName = pPacket->ReadString();
	string FriendName = pPacket->ReadString();
	int flag = pPacket->ReadInt();

	g_GetCDBOperator().UpdateMessage( FriendName, UserName, flag);
	SendCommonFlagMsgReply(COMMAND_CHANGE_MESSAGE_REPLY, 0, pHandler);
	return 0;
}

int CGameServer::Handle_User_Add_FriendAccept(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string FriendName = pPacket->ReadString();
	string SelfName = pPacket->ReadString();

	// check
	if (!g_GetCDBOperator().CheckAddFriend(FriendName))
	{
		// user no exist
		SendCommonFlagMsgReply(COMMAND_ACCEPT_FRIEND_REPLY, 1, pHandler);
		return 0;
	}

	// check two
	if (g_GetCDBOperator().CheckIsAFriend(FriendName, SelfName) && g_GetCDBOperator().CheckIsAFriend(FriendName, SelfName))
	{
		SendCommonFlagMsgReply(COMMAND_ACCEPT_FRIEND_REPLY, 2, pHandler);
		return 0;
	}

	if (!g_GetCDBOperator().AddFriend(FriendName, SelfName))
	{
		// user no exist
		SendCommonFlagMsgReply(COMMAND_ACCEPT_FRIEND_REPLY, 1, pHandler);
		return 0;
	}

	// success 
	SendCommonFlagMsgReply(COMMAND_ACCEPT_FRIEND_REPLY, 0, pHandler);
	return 0;
}

int CGameServer::Handle_User_Delete_Friend(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string SelfName = pPacket->ReadString();
	string FriendName = pPacket->ReadString();

	if (g_GetCDBOperator().DelFriend(FriendName, SelfName) && g_GetCDBOperator().DelFriend(SelfName, FriendName))
	{
		// success 
		SendCommonFlagMsgReply(COMMAND_DELETE_FRIEND_REPLY, 0, pHandler);
		return 0;
	}

	SendCommonFlagMsgReply(COMMAND_DELETE_FRIEND_REPLY, 1, pHandler);
	return 0;
}

int CGameServer::Handle_User_Get_Friend_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	log_debug("Enter friend list \n");

	int userGameID = pPacket->ReadInt();
	string MyName = pPacket->ReadString();
	int index = pPacket->ReadInt();

	//vector<string> FriendNameList;
	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );
	if (pUser == NULL)
	{
		log_debug("user error\n");
		SendCommonFlagMsgReply(COMMAND_GET_FRIEND_LIST_REPLY, 0, pHandler);
		return 0;
	}
	
	pUser->m_sfriends.clear();

	if (!g_GetCDBOperator().GetFriendList(MyName, pUser->m_sfriends))
	{
		// success 
		log_debug("get friend error\n");
		SendCommonFlagMsgReply(COMMAND_GET_FRIEND_LIST_REPLY, 0, pHandler);
		return 0;
	}

	// sort the vector
	sort(pUser->m_sfriends.begin(), pUser->m_sfriends.end());
	int nlen = pUser->m_sfriends.size();
	int pageConst = 15;
	int pageNums = nlen / pageConst;
	if (nlen % pageConst != 0)
	{
		pageNums++;
	}

	log_debug("the user friend list len is %d, page num is %d\n", nlen, pageNums);
	if (index > pageNums || index <= 0)
	{
		SendCommonFlagMsgReply(COMMAND_GET_FRIEND_LIST_REPLY, 0, pHandler);
		return 0;
	}
	
	vector<string> replyfriendName;
	int limit = pageConst * index;
	for (int i = pageConst; i > 0; i--) {
		replyfriendName.push_back(pUser->m_sfriends[limit - i]);
		if (limit - i >= nlen - 1) {
			break;
		}
	}

	log_debug("shit mssage\n");

	// get info from registtable
	int listLen = replyfriendName.size();
	vector<FriendInfo> FriendInfoList;
	for (int i = 0; i < listLen; i++)
	{
		FriendInfo oneFriendInfo;
		if (g_GetCDBOperator().GetFriendInfo(replyfriendName[i], oneFriendInfo))
		{
			FriendInfoList.push_back(oneFriendInfo);
		}
	}

	// reply to user
	NETOutputPacket outpacket;
	outpacket.Begin(COMMAND_GET_FRIEND_LIST_REPLY);
	outpacket.WriteInt(nlen);
	int rflen = FriendInfoList.size();
	outpacket.WriteShort(rflen);
	for (int i = 0; i < rflen; i++)
	{
		outpacket.WriteString(FriendInfoList[i].name);
		outpacket.WriteString(FriendInfoList[i].mail);
		outpacket.WriteInt(FriendInfoList[i].LastLoginTime);
		outpacket.WriteShort(FriendInfoList[i].headimage);
		if (FriendInfoList[i].headimage == 100)
		{
			// get user FBID by name
			//string FBID = "100001094443807";
			//g_GetCDBOperator().GetFBID_by_userName(FriendInfoList[i].name, FBID);
			outpacket.WriteString(FriendInfoList[i].fbid);
		}
		// 判断是否游戏玩家
		outpacket.WriteShort(1);
	}
	outpacket.End();
	log_debug("send the mssage\n");
	pHandler->Send(&outpacket);

	return 0;
}

int CGameServer::Handle_User_Get_Message_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string MyName = pPacket->ReadString();
	int index = pPacket->ReadInt();

	vector<UserMessageInfo> UserMessageList;

	if (!g_GetCDBOperator().GetMessageInfoList(MyName, UserMessageList))
	{
		log_debug("get message zero\n");
		SendCommonFlagMsgReply(COMMAND_GET_USER_MSG_REPLY, 0, pHandler);
		return 0;
	}
	
	// sort
	sort(UserMessageList.begin(), UserMessageList.end(), myfunction);

	int nlen = UserMessageList.size();
	int pageConst = 15;
	int pageNums = nlen / pageConst;
	if (nlen % pageConst != 0)
	{
		pageNums++;
	}

	log_debug("Handle_User_Get_Message_List len is %d, page num is %d\n", nlen, pageNums);
	if (index > pageNums)
	{
		SendCommonFlagMsgReply(COMMAND_GET_USER_MSG_REPLY, 0, pHandler);
		return 0;
	}

	vector<UserMessageInfo> ReplyUserMessageList;
	int limit = pageConst * index;
	for (int i = pageConst; i > 0; i--) {
		ReplyUserMessageList.push_back(UserMessageList[limit - i]);
		if (limit - i >= nlen - 1) {
			break;
		}
	}

	// reply to user
	NETOutputPacket outpacket;
	outpacket.Begin(COMMAND_GET_USER_MSG_REPLY);
	outpacket.WriteInt(nlen);
	int rflen = ReplyUserMessageList.size();
	outpacket.WriteShort(rflen);
	for (int i = 0; i < rflen; i++)
	{
		outpacket.WriteString(ReplyUserMessageList[i].senderName);
		outpacket.WriteInt(ReplyUserMessageList[i].msgID);
		outpacket.WriteShort(ReplyUserMessageList[i].messagetype);
		outpacket.WriteString(ReplyUserMessageList[i].content);
		outpacket.WriteInt(ReplyUserMessageList[i].time);
		outpacket.WriteShort(ReplyUserMessageList[i].state);
	}
	outpacket.End();

	log_debug("friend message , the packet size is %d\n", outpacket.packet_size());

	pHandler->Send(&outpacket);

	return 0;
}

void CGameServer::get_production_by_pcate(OrmenGoodsStruct_t_list& pplist, int pcate)
{
	int len = m_ormenGoodslist.size();
	for (int i = 0; i < len; i++)
	{
		OrmenGoodsStruct_t onep = m_ormenGoodslist[i];
		if (pcate == onep.pcate)
		{
			pplist.push_back(onep);
		}
	}
}

bool CGameServer::get_production_price_by_pcate(int & coin, int & money, int pcate, int pframe)
{
	int len = m_ormenGoodslist.size();
	bool isfound = false;
	for (int i = 0; i < len; i++)
	{
		OrmenGoodsStruct_t onep = m_ormenGoodslist[i];
		if (pcate == onep.pcate && pframe == onep.pframe)
		{
			isfound = true;
			coin = onep.coin;
			money = onep.money;
			break;
		}
	}

	return isfound;
}

bool CGameServer::get_production_sellprice_by_pcate(int & iscansell, int & coin, int pcate, int pframe)
{
	int len = m_ormenGoodslist.size();
	bool isfound = false;
	for (int i = 0; i < len; i++)
	{
		OrmenGoodsStruct_t onep = m_ormenGoodslist[i];
		if (pcate == onep.pcate && pframe == onep.pframe)
		{
			isfound = true;
			coin = onep.sellCoin;
			iscansell = onep.isCanSell;		//0 ok, 1 no
			break;
		}
	}

	return isfound;
}

bool CGameServer::getMechineInfoByPcatePframe(int pcate, int pframe, SysMechineInfo & Mechine)
{
	int len = m_systemMechineList.size();
	bool isFound = false;
	for (int i = 0; i < len; i++)
	{
		if (m_systemMechineList[i].pcate == pcate && m_systemMechineList[i].pframe == pframe)
		{
			log_debug("found the mechine , the pcate is %d, the pframe is %d\n", pcate, pframe);
			isFound = true;
			Mechine = m_systemMechineList[i];
			break;
		}
	}

	return isFound;
}

// 购买产品
int  CGameServer::Handle_User_Buy_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int curDir = pPacket->ReadInt();
	int locationx = pPacket->ReadInt();
	int locationy = pPacket->ReadInt();

	// get price 
	int needcoin = 0;
	int needmoney = 0;

	if (!get_production_price_by_pcate(needcoin, needmoney, pcate, pframe))
	{
		log_debug("can't get the price");
		SendBuyFlagMsgReply( COMMAND_BUY_GOODS_PCATE_REPLY, 1, 0, pcate, pframe, locationx, locationy, pHandler);
		return 0;
	}
	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );
	// check user money
	if (pUser == NULL)
	{
		log_debug("can't get the user");
		SendBuyFlagMsgReply( COMMAND_BUY_GOODS_PCATE_REPLY, 1, 0, pcate, pframe, locationx, locationy, pHandler);
		return 0;
	}
	
	if (pUser->m_coin < needcoin || pUser->m_money < needmoney)
	{
		log_debug("user money is not enougth, need money is %d, need coin is %d, user money is %d, user coin is %d\n", needmoney, needcoin, pUser->m_money, pUser->m_coin);
		SendBuyFlagMsgReply( COMMAND_BUY_GOODS_PCATE_REPLY, 1, 0, pcate, pframe, locationx, locationy, pHandler);
		return 0;
	}

	// insert a production into user production db
	UserOrmenGoodsStruct_t userproduction;
	userproduction.position = 1;	// 正常位置
	userproduction.mid = pUser->getID();
	userproduction.pcate = pcate;
	userproduction.pframe = pframe;
	userproduction.curDir = curDir;
	userproduction.locationx = locationx;
	userproduction.locationy = locationy;

	int gid;
	if (!g_GetCDBOperator().UserBuyProduction(userproduction, gid))
	{
		log_debug("buy db operate error\n");
		SendBuyFlagMsgReply( COMMAND_BUY_GOODS_PCATE_REPLY, 1, 0, pcate, pframe, locationx, locationy, pHandler);
		return 0;
	}

	int finalcoin = pUser->m_coin - needcoin;
	int finalmoney = pUser->m_money - needmoney;

	// delete and update user money
	if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
	{
		log_debug("money db operate error\n");
		SendBuyFlagMsgReply( COMMAND_BUY_GOODS_PCATE_REPLY, 1, 0, pcate, pframe, locationx, locationy, pHandler);
		return 0;
	}

	pUser->m_coin = finalcoin;
	pUser->m_money = finalmoney;
	log_debug("user buy success, the gid is %d\n", gid);

	NETOutputPacket outpacket;
	outpacket.Begin(COMMAND_BUY_GOODS_PCATE_REPLY);
	outpacket.WriteShort(0);
	outpacket.WriteInt(gid);
	outpacket.WriteInt(pcate);
	outpacket.WriteInt(pframe);
	outpacket.WriteInt(locationx);
	outpacket.WriteInt(locationy);
	outpacket.End();
	pHandler->Send(&outpacket);

	// 加载购买的机器
	MechineType_PcatePframe_T InfoMechinePcate = g_GetConfigMgr().getMechine_Info_Type();
	MechineType_PcatePframe_T ResourceMechinePcate = g_GetConfigMgr().getMechine_Resource_Type();
	MechineType_PcatePframe_T PowerMechinePcate = g_GetConfigMgr().getMechine_PowerMechine_Type();
	if (pcate == InfoMechinePcate.pcate || pcate == ResourceMechinePcate.pcate)
	{
		int time = g_GetConfigMgr().getComStructTimeBy_Pcate_Pframe(pcate, pframe) + GetUnixTime();
		int status = 2;

		User_Experiment_Mechine_t buyMechine;
		buyMechine.Gid = gid;
		buyMechine.curDir = curDir;
		buyMechine.endtime =  time;
		buyMechine.isDirty = false;
		buyMechine.level = 1;
		buyMechine.pcate = pcate;
		buyMechine.pframe = pframe;
		buyMechine.position = 1;
		buyMechine.positionx = locationx;
		buyMechine.positiony = locationy;
		buyMechine.status = status;

		pUser->m_userBuyMechineList.push_back(buyMechine);
	}
	else if (pcate == PowerMechinePcate.pcate)	// 特殊机器
	{
		User_Experiment_Mechine_t buyMechine;
		buyMechine.Gid = gid;
		buyMechine.curDir = curDir;
		buyMechine.endtime =  0;
		buyMechine.isDirty = false;
		buyMechine.level = 1;
		buyMechine.pcate = pcate;
		buyMechine.pframe = pframe;
		buyMechine.position = 1;
		buyMechine.positionx = locationx;
		buyMechine.positiony = locationy;
		buyMechine.status = 0;

		pUser->m_userBuyMechineList.push_back(buyMechine);
	}

	return 0;
}

int  CGameServer::Handle_User_Edit_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pid = pPacket->ReadInt();	// 物品id
	int useroperator = pPacket->ReadShort();

	if (useroperator == 1)
	{
		// 修改朝向
		int curDir = pPacket->ReadInt();
		int locationx = pPacket->ReadInt();
		int locationy = pPacket->ReadInt();

		int issuccess = 0;
		if (!g_GetCDBOperator().ChangeUserProductionDirLocation(pid, curDir, locationx, locationy))
		{
			// 失败
			log_debug("Edit_Production error\n");
			issuccess = 1;
		}
		SendCommonEditFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, pid, issuccess, pHandler);
	}
	else if (useroperator == 2)
	{
		// 出售
		int pcate = 0, pframe = 0;
		if (!g_GetCDBOperator().GetProduction_Type_by_Pid(pid, pcate, pframe))
		{
			SendCommonEditFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, pid, 1, pHandler);
			return 0;
		}		

		// 1 get the sell price
		int iscansell = 0;
		int coin = 0;
		bool ret = get_production_sellprice_by_pcate(iscansell, coin, pcate, pframe);
		if (!ret)
		{
			SendCommonEditFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, pid, 1, pHandler);
			return 0;
		}

		// 2 add the user money
		CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );
		// check user money
		if (pUser == NULL)
		{
			log_debug("Handle_User_Edit_Production, can't get the user");
			SendCommonEditFlagMsgReply( COMMAND_EDIT_GOODS_PID_REPLY, pid, 1, pHandler);
			return 0;
		}
		int finalcoin = pUser->m_coin + coin;
		int finalmoney = pUser->m_money;
		if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
		{
			log_debug("Handle_User_Edit_Productionmoney db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_EDIT_GOODS_PID_REPLY, pid, 1, pHandler);
			return 0;
		}
		pUser->m_coin = finalcoin;
		pUser->m_money = finalmoney;
		log_debug("user edit success\n");

		// 3 delete the production
		if (!g_GetCDBOperator().DelUserProduction(pid))
		{
			log_debug("DelUserProduction db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_EDIT_GOODS_PID_REPLY, pid, 1, pHandler);
			return 0;
		}
		SendCommonEditFlagMsgReply( COMMAND_EDIT_GOODS_PID_REPLY, pid, 0, pHandler); 
	}
	else if (useroperator == 3)
	{
		// 放进创库
		int issuccess = 0;
		if (!g_GetCDBOperator().ChangeUserProductionHouse(pid, 3))
		{
			// 失败
			log_debug("Edit_Production house error\n");
			issuccess = 1;
		}
		SendCommonEditFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, pid, issuccess, pHandler);
	}
	return 0;
}

int  CGameServer::Handle_User_Get_Enlarge_Table(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );
	int sizex = 0;
	int sizey = 0;
	int coin = 0;
	int money = 0;
	int nlevel = 0;
	bool isfound = false;

	int enums = m_enlargelist.size();
	for (int i = 0; i < enums; i++)
	{
		if (m_enlargelist[i].sizex == pUser->m_ibasew+1)
		{
			sizex = m_enlargelist[i].sizex;
			sizey = m_enlargelist[i].sizey;
			coin = m_enlargelist[i].coin;
			money = m_enlargelist[i].money;
			nlevel = m_enlargelist[i].nlevel;
			isfound = true;
			break;
		}
	}

	if (!isfound)
	{
		sizex = m_enlargelist[enums-1].sizex;
		sizey = m_enlargelist[enums-1].sizey;
		coin = m_enlargelist[enums-1].coin;
		money = m_enlargelist[enums-1].money;
		nlevel = m_enlargelist[enums-1].nlevel;
	}


	NETOutputPacket outpacket;
	outpacket.Begin(COMMAND_GET_ENLARGE_TABLE_REPLY);
	outpacket.WriteInt(sizex);
	outpacket.WriteInt(sizey);
	outpacket.WriteInt(coin);
	outpacket.WriteInt(money);
	outpacket.WriteInt(nlevel);
	outpacket.End();

	pHandler->Send(&outpacket);

	return 0;
}

int CGameServer::Handle_User_Info_Mechine_CollectSpeed(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int pid = pPacket->ReadInt();
	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );

	if (pUser == NULL)
	{
		// 失败
		log_debug("Handle_User_Info_Mechine_CollectSpeed error, pUser == NULL\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY, pid, 1, pHandler);
		return 0;
	}

	User_Experiment_Mechine_t MechineItem;
	bool ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
	if (!ret)
	{
		log_debug("Handle_User_Require_Mechine_Status, can't find the production pid is %d\n", pid);
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY, pid, 1, pHandler);
		return 0;
	}

	UserInfoMechineCollect_T UserInfoCollect;
	ret = getUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect, pid);
	if (!ret)
	{
		// 失败
		log_debug("Handle_User_Info_Mechine_CollectCoin error, pUser == NULL\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY, pid, 1, pHandler);
		return 0;
	}

	int lefttime = UserInfoCollect.NeedTime - UserInfoCollect.LeftTime;

	InfoMechineCollectTable_T item;
	ret = getCollectItemByPcateLevel(item, pcate, pframe, MechineItem.level);
	if (!ret)
	{
		// 失败
		log_debug("Handle_User_Info_Mechine_CollectCoin error, pUser == NULL\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY, pid, 1, pHandler);
		return 0;
	}

	UserInfoCollect.Status = 2;
	UserInfoCollect.LeftTime = UserInfoCollect.NeedTime;
	ChangeUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect);
	// 完成
	NETOutputPacket outPacket;
	outPacket.Begin( COMMAND_MECHINE_INFO_CANCOLLECT_REPLY );
	outPacket.WriteInt(UserInfoCollect.Gid);
	outPacket.WriteShort(0);
	outPacket.End();
	pUser->SendMsg( &outPacket );

	// 发送成功
	if (lefttime <= 0)
	{
		lefttime = 0;
	}

	int money = lefttime / item.MoneyTime;
	int rem = lefttime % item.MoneyTime;
	if (rem != 0)
	{
		money += 1;
	}
	int finalcoin = pUser->m_coin;
	int finalmoney = pUser->m_money - money;

	if (finalmoney < 0)
	{
		log_debug("Handle_User_Info_Mechine_CollectCoin money is not enougth\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY, pid, 1, pHandler);
		return 0;
	}

	// delete and update user money
	if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
	{
		log_debug("Handle_User_Info_Mechine_CollectCoin money db operate error\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY, pid, 1, pHandler);
		return 0;
	}

	pUser->m_coin = finalcoin;
	pUser->m_money = finalmoney;

	log_debug("Handle_User_Info_Mechine_CollectCoin money db operate success\n");
	SendCommonEditFlagMsgReply( COMMAND_MECHINE_INFO_COLLECT_SPEED_REPLY, pid, 0, pHandler);
	return 0;
}

int CGameServer::Handle_User_Info_Mechine_CollectCoin(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int pid = pPacket->ReadInt();

	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );

	if (pUser == NULL)
	{
		// 失败
		log_debug("Handle_User_Info_Mechine_CollectCoin error, pUser == NULL\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE__COLLECT_INFO_REPLY, pid, 1, pHandler);
		return 0;
	}
	
	UserInfoMechineCollect_T UserInfoCollect;
	bool ret = getUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect, pid);
	if (!ret)
	{
		// 失败
		log_debug("Handle_User_Info_Mechine_CollectCoin error, pUser == NULL\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE__COLLECT_INFO_REPLY, pid, 1, pHandler);
		return 0;
	}

	User_Experiment_Mechine_t MechineItem;
	ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
	if (!ret)
	{
		// 失败
		log_debug("Handle_User_Info_Mechine_CollectCoin error, pUser == NULL\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE__COLLECT_INFO_REPLY, pid, 1, pHandler);
		return 0;
	}

	InfoMechineCollectTable_T item;
	ret = getCollectItemByPcateLevel(item, pcate, pframe, MechineItem.level);
	if (!ret)
	{
		// 失败
		log_debug("Handle_User_Info_Mechine_CollectCoin error, pUser == NULL\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE__COLLECT_INFO_REPLY, pid, 1, pHandler);
		return 0;
	}

	int ecoin = item.EarCoin;
	UserInfoCollect.Status = 1;
	UserInfoCollect.LeftTime = 0;
	getCollectTimeByCollItem(UserInfoCollect, MechineItem.level);
	ChangeUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect);

	// 添加钱
	int finalcoin = pUser->m_coin + ecoin;
	int finalmoney = pUser->m_money;

	if (finalcoin < 0)
	{
		log_debug("Handle_User_Info_Mechine_CollectCoin money is not enougth\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE__COLLECT_INFO_REPLY, pid, 1, pHandler);
		return 0;
	}

	// delete and update user money
	if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
	{
		log_debug("Handle_User_Info_Mechine_CollectCoin money db operate error\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE__COLLECT_INFO_REPLY, pid, 1, pHandler);
		return 0;
	}

	pUser->m_coin = finalcoin;
	pUser->m_money = finalmoney;

	log_debug("Handle_User_Info_Mechine_CollectCoin money db operate success\n");
	SendCommonEditFlagMsgReply( COMMAND_MECHINE__COLLECT_INFO_REPLY, pid, 0, pHandler);
	return 0;
}

int CGameServer::Handle_User_Require_Mechine_Comfor(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int pid = pPacket->ReadInt();

	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );

	if (pUser == NULL)
	{
		// 失败
		log_debug("Handle_User_Require_Mechine_Comfor error, pUser == NULL\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 1, pHandler);
		return 0;
	}

	//实验机器
	MechineType_PcatePframe_T ExperimentMechinePcate = g_GetConfigMgr().getMechine_Experiment_Type();
	if (pcate == ExperimentMechinePcate.pcate && pframe == ExperimentMechinePcate.pframe)
	{
		log_debug("Handle_User_Require_Mechine_Comfor experiment mechine is can't get status \n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 1, pHandler);
		return 0;
	}

	MechineType_PcatePframe_T InstorMechinePcate = g_GetConfigMgr().getMechine_Instor_Type();

	if (pcate == InstorMechinePcate.pcate && pframe == InstorMechinePcate.pframe)
	{
		// 指导员升级确认
		if (pUser->m_userInstorMechine.status != 4)
		{
			log_debug("Handle_User_Require_Mechine_Comfor, status is equase 4\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 1, pHandler);
			return 0;
		}
		pUser->m_userInstorMechine.level += 1;
		pUser->m_userInstorMechine.status = 0;

		int len = m_instorUpLevelList.size();//m_instorUpLevelList 要先排序
		int MaxLevel = m_instorUpLevelList[len -1].Level;
		
		if (MaxLevel == pUser->m_userInstorMechine.level)
		{
			pUser->m_userInstorMechine.status = 11;
		}
			
		bool ret = g_GetCDBOperator().UpdateUserMechineTable(pUser->m_userInstorMechine);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_Comfor mechine db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 1, pHandler);
			return 0;
		}

		log_debug("Handle_User_Require_Mechine_Comfor success\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 0, pHandler);
		return 0;
	}
	// 其他机器
	else
	{
		User_Experiment_Mechine_t MechineItem;
		bool ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_Comfor, can't find the production pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 1, pHandler);
			return 0;
		}

		if (MechineItem.status != 4)
		{
			log_debug("Handle_User_Require_Mechine_Comfor require status is not equate 3\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 1, pHandler);
			return 0;
		}

		MechineItem.level += 1;
		MechineItem.status = 0;

		//int MaxLevel = 10; // TODO 从数据库来读
		//if (MaxLevel == MechineItem.level)
		//{
		//	MechineItem.status = 11;
		//}

		ret = g_GetCDBOperator().UpdateUserMechineTable(MechineItem);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_Comfor mechine db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 1, pHandler);
			return 0;
		}

		// update mechineitem
		ChangeUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem);

		log_debug("Handle_User_Require_Mechine_Comfor success\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_COMFOR_REPLY, pid, 0, pHandler);

		// 如果是情报员机器则继续收集
		MechineType_PcatePframe_T InfoMechinePcate = g_GetConfigMgr().getMechine_Info_Type();
		if (InfoMechinePcate.pcate == MechineItem.pcate && InfoMechinePcate.pframe == MechineItem.pframe)
		{
			UserInfoMechineCollect_T UserInfoCollect;
			bool ret = getUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect, MechineItem.Gid);
			if (ret)
			{
				UserInfoCollect.Status = 1;
				ChangeUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect);
				g_GetCDBOperator().UpdateUserInfoMechineCollectTable(UserInfoCollect);
			}
		}
		return 0;
	}
	return 0;
}

int CGameServer::Handle_User_Require_Mechine_Speed(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int pid = pPacket->ReadInt();
	
	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );

	if (pUser == NULL)
	{
		// 失败
		log_debug("Handle_User_Require_Mechine_Speed error, user == null\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
		return 0;
	}

	//实验机器
	MechineType_PcatePframe_T ExperimentMechinePcate = g_GetConfigMgr().getMechine_Experiment_Type();
	if (pcate == ExperimentMechinePcate.pcate && pframe == ExperimentMechinePcate.pframe)
	{
		log_debug("Handle_User_Require_Mechine_Speed experiment mechine is can't get status \n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
		return 0;
	}

	//指导员机器
	MechineType_PcatePframe_T InstorMechinePcate = g_GetConfigMgr().getMechine_Instor_Type();
	if (pcate == InstorMechinePcate.pcate && pframe == InstorMechinePcate.pframe)
	{
		if (pUser->m_userInstorMechine.status != 3)
		{
			log_debug("Handle_User_Require_Mechine_Speed, status isnot equate 3\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		int lefttime = GetUnixTime() - pUser->m_userInstorMechine.endtime;

		InstrutorUpLevelTable_T oneitem;
		bool isfound = getInstorUplevelComsum(pUser->m_userInstorMechine.level + 1, oneitem);
		if (!isfound || lefttime <= 0)
		{
			// 失败
			log_debug("Handle_User_Require_Mechine_Speed, left time == 0\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		int money = lefttime / oneitem.MoneyTime;
		int rem = lefttime % oneitem.MoneyTime;
		if (rem != 0)
		{
			money += 1;
		}

		// 扣钱
		int finalcoin = pUser->m_coin;
		int finalmoney = pUser->m_money - money;

		if (finalmoney < 0)
		{
			log_debug("Handle_User_Require_Mechine_Speed money is not enougth\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		// delete and update user money
		if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
		{
			log_debug("Handle_User_Require_Mechine_Speed money db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		pUser->m_coin = finalcoin;
		pUser->m_money = finalmoney;

		// 更新指导员状态
		// pUser->m_userInstorMechine.level = nextLevel;	// 要完成时才升级
		pUser->m_userInstorMechine.endtime = 0;
		pUser->m_userInstorMechine.status = 4;
		// 更新到数据库
		bool ret = g_GetCDBOperator().UpdateUserMechineTable(pUser->m_userInstorMechine);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_Speed mechine db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		log_debug("speed success\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 0, pHandler);
		return 0;
	
	}
	else
	{
		User_Experiment_Mechine_t MechineItem;
		bool ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_Speed, can't find the production pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		if (MechineItem.status != 3)
		{
			log_debug("Handle_User_Require_Mechine_Speed require status is not equate 3\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		int lefttime = GetUnixTime() - MechineItem.endtime;
		MechineUpLevelComsumTable_T oneitem;
		ret = getMechineUplevelComsum(pcate, pframe, MechineItem.level + 1, oneitem);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_Speed, can't find the production pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}
		
		int money = lefttime / oneitem.MoneyTime;
		int rem = lefttime % oneitem.MoneyTime;
		if (rem != 0)
		{
			money += 1;
		}

		// 扣钱
		int finalcoin = pUser->m_coin;
		int finalmoney = pUser->m_money - money;

		if (finalmoney < 0)
		{
			log_debug("Handle_User_Require_Mechine_Speed money is not enougth\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		// delete and update user money
		if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
		{
			log_debug("Handle_User_Require_Mechine_Speed money db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}

		pUser->m_coin = finalcoin;
		pUser->m_money = finalmoney;

		// 更新指导员状态
		MechineItem.endtime = 0;
		MechineItem.status = 4;
		// 更新到数据库
		ret = g_GetCDBOperator().UpdateUserMechineTable(MechineItem);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_Speed mechine db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 1, pHandler);
			return 0;
		}
		// update mechineitem
		ChangeUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem);

		log_debug("speed success\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_SPEED_REPLY, pid, 0, pHandler);
		return 0;
	}
	return 0;
}

int CGameServer::Handle_User_Require_Mechine_Status(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int pid = pPacket->ReadInt();
	int flag = pPacket->ReadShort();
	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );

	if (pUser == NULL)
	{
		// 失败
		log_debug("Handle_User_Require_Mechine_Status user null\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
		return 0;
	}

	if (flag == 1)
	{
		//实验机器
		MechineType_PcatePframe_T ExperimentMechinePcate = g_GetConfigMgr().getMechine_Experiment_Type();
		if (pcate == ExperimentMechinePcate.pcate && pframe == ExperimentMechinePcate.pframe)
		{
			log_debug("Handle_User_Require_Mechine_Status experiment mechine is can't get status \n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
			return 0;
		}

		//指导员机升级
		MechineType_PcatePframe_T InstorMechinePcate = g_GetConfigMgr().getMechine_Instor_Type();

		if (pcate == InstorMechinePcate.pcate && pframe == InstorMechinePcate.pframe)
		{
			if (pUser->m_userInstorMechine.status != 3)
			{
				log_debug("Handle_User_Require_Mechine_Status require status is not equate 3\n");
				SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
				return 0;
			}
			int lefttime = GetUnixTime() - pUser->m_userInstorMechine.endtime;
			InstrutorUpLevelTable_T oneitem;
			bool isfound = getInstorUplevelComsum(pUser->m_userInstorMechine.level + 1, oneitem);
			if (!isfound)
			{
				// 失败
				log_debug("Handle_User_Require_Mechine_Status can't found the item\n");
				SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
				return 0;
			}

			if (lefttime <= 0)
			{
				lefttime = 0;
			}

			int money = lefttime / oneitem.MoneyTime;
			int rem = lefttime % oneitem.MoneyTime;
			if (rem != 0)
			{
				money += 1;
			}

			log_debug("Handle_User_Require_Mechine_Status success\n");
			SendCommonMechineStatusReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 0, lefttime, money, pHandler);
			return 0;
		}
		// 其他特殊机器
		else
		{
			User_Experiment_Mechine_t MechineItem;
			bool ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
			if (!ret)
			{
				log_debug("Handle_User_Require_Mechine_Status, can't find the production pid is %d\n", pid);
				SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
				return 0;
			}

			if (MechineItem.status != 3)
			{
				log_debug("Handle_User_Require_Mechine_Status require status is not equate 3\n");
				SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
				return 0;
			}
			int lefttime = GetUnixTime() - MechineItem.endtime;
			MechineUpLevelComsumTable_T oneitem;

			ret = getMechineUplevelComsum(pcate, pframe, MechineItem.level + 1, oneitem);
			if (!ret)
			{
				log_debug("Handle_User_Require_Mechine_Status, can't find the production pid is %d\n", pid);
				SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
				return 0;
			}

			if (lefttime <= 0)
			{
				lefttime = 0;
			}

			int money = lefttime / oneitem.MoneyTime;
			int rem = lefttime % oneitem.MoneyTime;
			if (rem != 0)
			{
				money += 1;
			}

			log_debug("Handle_User_Require_Mechine_Status success\n");
			SendCommonMechineStatusReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 0, lefttime, money, pHandler);
			return 0;
		}
	}
	else if (flag == 2)	//情报机收集加速
	{
		User_Experiment_Mechine_t MechineItem;
		bool ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_Status, can't find the production pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
			return 0;
		}

		UserInfoMechineCollect_T UserInfoCollect;
		ret = getUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect, pid);
		if (!ret)
		{
			// 失败
			log_debug("Handle_User_Info_Mechine_CollectCoin error, pUser == NULL\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
			return 0;
		}

		int lefttime = UserInfoCollect.NeedTime - UserInfoCollect.LeftTime;

		InfoMechineCollectTable_T item;
		ret = getCollectItemByPcateLevel(item, pcate, pframe, MechineItem.level);
		if (!ret)
		{
			// 失败
			log_debug("Handle_User_Info_Mechine_CollectCoin error, pUser == NULL\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 1, pHandler);
			return 0;
		}

		if (lefttime <= 0)
		{
			lefttime = 0;
		}

		int money = lefttime / item.MoneyTime;
		int rem = lefttime % item.MoneyTime;
		if (rem != 0)
		{
			money += 1;
		}

		log_debug("Handle_User_Require_Mechine_Status success\n");
		SendCommonMechineStatusReply( COMMAND_MECHINE_REQUIRE_MECHINE_STATUS_REPLY, pid, 0, lefttime, money, pHandler);
		return 0;
	}

	return 0;
}

int CGameServer::Handle_User_Require_Mechine_UpNextLevel(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int pid = pPacket->ReadInt();

	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );

	if (pUser == NULL)
	{
		// 失败
		log_debug("Handle_User_Require_Mechine_UpNextLevel user == null\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
		return 0;
	}

	//实验机器
	MechineType_PcatePframe_T ExperimentMechinePcate = g_GetConfigMgr().getMechine_Experiment_Type();
	if (pcate == ExperimentMechinePcate.pcate && pframe == ExperimentMechinePcate.pframe)
	{
		log_debug("Handle_User_Require_Mechine_UpLevel experiment mechine is can't uplevel \n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
		return 0;
	}

	//指导员机升级
	MechineType_PcatePframe_T InstorMechinePcate = g_GetConfigMgr().getMechine_Instor_Type();
	if (pcate == InstorMechinePcate.pcate && pframe == InstorMechinePcate.pframe)
	{
		// 等级大小
		int len = m_instorUpLevelList.size();//m_instorUpLevelList 要先排序
		int MaxLevel = m_instorUpLevelList[len -1].Level;
		int instorLevel = pUser->m_userInstorMechine.level;
		if (instorLevel >= MaxLevel)
		{
			// 到达最大值了，不能升级了
			// 失败
			log_debug("Handle_User_Require_Mechine_UpNextLevel, the level is max\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		int nextLevel = instorLevel + 1;

		// 获取相关消耗，扣分成功进行相关的操作
		int needUserLevel = getNeedUserLevel(nextLevel);
		InstrutorUpLevelTable_T oneitem;
		bool isfound = getInstorUplevelComsum(nextLevel, oneitem);
		
		if (!isfound || needUserLevel == 0 || needUserLevel > pUser->GetLevel())
		{
			log_debug("Handle_User_Require_Mechine_UpNextLevel, level is too less\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		// 扣除普通金币
		int finalcoin = pUser->m_coin - oneitem.Coin;
		if (finalcoin < 0)
		{
			log_debug("Handle_User_Require_Mechine_UpNextLevel money is not enougth\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}
		int finalmoney = pUser->m_money;

		// delete and update user money
		if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
		{
			log_debug("Handle_User_Require_Mechine_UpNextLevel money db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		pUser->m_coin = finalcoin;
		pUser->m_money = finalmoney;

		// 更新指导员状态
		// pUser->m_userInstorMechine.level = nextLevel;	// 要完成时才升级
		pUser->m_userInstorMechine.endtime = GetUnixTime() + oneitem.Time;
		pUser->m_userInstorMechine.status = 3;

		// 更新到数据库
		bool ret = g_GetCDBOperator().UpdateUserMechineTable(pUser->m_userInstorMechine);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_UpNextLevel mechine db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}
		log_debug("Handle_User_Require_Mechine_UpNextLevel success\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 0, pHandler);
		return 0;
	}
	// 其他机器升级
	else
	{
		User_Experiment_Mechine_t MechineItem;
		bool ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_UpLevel, can't find the production pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		int instorMechineLevel = pUser->m_userInstorMechine.level;
		MechineUpLevelComsumTable_T oneitem;

		ret = getMechineUplevelComsum(pcate, pframe, MechineItem.level + 1, oneitem);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_UpLevel, can't find the production pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}
		// 不能超过指导员等级
		if (MechineItem.level > instorMechineLevel)
		{
			log_debug("Handle_User_Require_Mechine_UpLevel, level error pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		// 扣除普通金币
		int finalcoin = pUser->m_coin - oneitem.Coin;
		if (finalcoin < 0)
		{
			log_debug("Handle_User_Require_Mechine_UpNextLevel money is not enougth\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}
		int finalmoney = pUser->m_money;

		// delete and update user money
		if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
		{
			log_debug("Handle_User_Require_Mechine_UpNextLevel money db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		pUser->m_coin = finalcoin;
		pUser->m_money = finalmoney;

		// 更新指导员状态
		MechineItem.endtime = GetUnixTime() + oneitem.Time;
		MechineItem.status = 3;

		// 更新到数据库
		ret = g_GetCDBOperator().UpdateUserMechineTable(MechineItem);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_UpNextLevel mechine db operate error\n");
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		// update mechineitem
		ChangeUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem);

		log_debug("Handle_User_Require_Mechine_UpNextLevel success\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPNEXTLEVEL_REPLY, pid, 0, pHandler);

		// 如果是情报员机器则停止收集
		MechineType_PcatePframe_T InfoMechinePcate = g_GetConfigMgr().getMechine_Info_Type();
		if (InfoMechinePcate.pcate == MechineItem.pcate && InfoMechinePcate.pframe == MechineItem.pframe)
		{
			UserInfoMechineCollect_T UserInfoCollect;
			bool ret = getUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect, MechineItem.Gid);
			if (ret)
			{
				UserInfoCollect.Status = 0;
				ChangeUserInfoMechineCollect_By_Pid(pUser->m_userInfoCollectList, UserInfoCollect);
				g_GetCDBOperator().UpdateUserInfoMechineCollectTable(UserInfoCollect);
			}
		}
		return 0;
	}

	return 0;
}

int CGameServer::Handle_User_Require_Mechine_UpLevel(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int pid = pPacket->ReadInt();

	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );

	if (pUser == NULL)
	{
		// 失败
		log_debug("Handle_User_Require_Mechine_UpLevel user is null\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPLEVEL_REPLY, pid, 1, pHandler);
		return 0;
	}

	//实验机器
	MechineType_PcatePframe_T ExperimentMechinePcate = g_GetConfigMgr().getMechine_Experiment_Type();
	if (pcate == ExperimentMechinePcate.pcate && pframe == ExperimentMechinePcate.pframe)
	{
		log_debug("Handle_User_Require_Mechine_UpLevel experiment mechine is can't update\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPLEVEL_REPLY, pid, 1, pHandler);
		return 0;
	}

	//指导员机升级条件
	MechineType_PcatePframe_T InstorMechinePcate = g_GetConfigMgr().getMechine_Instor_Type();

	if (pcate == InstorMechinePcate.pcate && pframe == InstorMechinePcate.pframe)
	{
		int nextLevel = pUser->m_userInstorMechine.level + 1;
		int needUserLevel = getNeedUserLevel(nextLevel);
		InstrutorUpLevelTable_T oneitem;
		bool isfound = getInstorUplevelComsum(nextLevel, oneitem);
		if (!isfound || needUserLevel == 0)
		{
			log_debug("Handle_User_Require_Mechine_UpLevel, the level is %d\n", needUserLevel);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		// 返回给玩家
		ReturnInstorUpLevelTJ InstorUpLevelTiaoJian;
		InstorUpLevelTiaoJian.otherNeed = 0;
		InstorUpLevelTiaoJian.needLevel = needUserLevel;
		InstorUpLevelTiaoJian.coin = oneitem.Coin;
		InstorUpLevelTiaoJian.moneyTime = oneitem.MoneyTime;
		InstorUpLevelTiaoJian.needTime = oneitem.Time;

		log_debug("Handle_User_Require_Mechine_UpLevel success\n");
		SendInstorUpLevelReply( COMMAND_MECHINE_UPLEVEL_REPLY, 0, pid, InstorUpLevelTiaoJian, pHandler);
		return 0;
	}
	// 其他机器
	else
	{
		User_Experiment_Mechine_t MechineItem;
		bool ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_UpLevel, can't find the production pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		int otherNeed = 0;
		int instorMechineLevel = pUser->m_userInstorMechine.level;
		MechineUpLevelComsumTable_T oneitem;

		ret = getMechineUplevelComsum(pcate, pframe, MechineItem.level + 1, oneitem);

		if (!ret)
		{
			log_debug("Handle_User_Require_Mechine_UpLevel, can't find the production pid is %d\n", pid);
			SendCommonEditFlagMsgReply( COMMAND_MECHINE_UPLEVEL_REPLY, pid, 1, pHandler);
			return 0;
		}

		// 返回给玩家
		ReturnInstorUpLevelTJ InstorUpLevelTiaoJian;
		InstorUpLevelTiaoJian.otherNeed = otherNeed;
		InstorUpLevelTiaoJian.needLevel = instorMechineLevel;
		InstorUpLevelTiaoJian.coin = oneitem.Coin;
		InstorUpLevelTiaoJian.moneyTime = oneitem.MoneyTime;
		InstorUpLevelTiaoJian.needTime = oneitem.Time;

		log_debug("Handle_User_Require_Mechine_UpLevel success\n");
		SendInstorUpLevelReply( COMMAND_MECHINE_UPLEVEL_REPLY, 0, pid, InstorUpLevelTiaoJian, pHandler);
		return 0;
	}

	return 0;
}

int  CGameServer::Handle_User_Buy_Enlarge_Table(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int playtype = pPacket->ReadInt();

	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );
	// check user money
	if (pUser == NULL)
	{
		log_debug("can't get the user");
		SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 4, pHandler);
		return 0;
	}

	int enums = m_enlargelist.size();
	bool isfount = false;
	int coin = 0, money = 0;
	int nlevel = 0;
	int sizex = 0, sizey = 0;
	int needtime = 0;
	for (int i = 0; i < enums; i++)
	{
		if (pUser->m_ibasew + 1 == m_enlargelist[i].sizex)
		{
			isfount = true;
			coin = m_enlargelist[i].coin;
			money = m_enlargelist[i].money;
			nlevel = m_enlargelist[i].nlevel;
			sizex = m_enlargelist[i].sizex;
			sizey = m_enlargelist[i].sizey;
			needtime = m_enlargelist[i].ctime;
			break;
		}
	}
	if (!isfount)
	{
		log_debug("can't find the item\n");
		SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 4, pHandler);
		return 0;
	}

	int needcoin = 0;
	int needmoney = 0;
	if (playtype == 1)	//普通货币
	{
		if (pUser->m_coin < coin)
		{
			log_debug("user money not enougth\n");
			SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 1, pHandler);
			return 0;
		}
		needcoin = coin;
	}
	else if (playtype == 2)	//金币
	{
		if (pUser->m_money < money)
		{
			log_debug("user money not enougth\n");
			SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 2, pHandler);
			return 0;
		}
		needmoney = money;
	}
	else
	{
		log_debug("user money not enougth\n");
		SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 7, pHandler);
		return 0;
	}

	int finalcoin = pUser->m_coin - needcoin;
	int finalmoney = pUser->m_money - needmoney;

	if (pUser->GetLevel() < nlevel)
	{
		log_debug("user level not enougth\n");
		SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 5, pHandler);
		return 0;
	}

	BaseInfo_Struct_t Baseinfo;
	if (!g_GetCDBOperator().CheckUserBase(Baseinfo, pUser->getID()))
	{
		log_debug("can't find user base info\n");
		SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 6, pHandler);
		return 0;
	}

	// 加载基地信息成功
	if (Baseinfo.istatus == 1)	// 正在扩张
	{
		log_debug("user base is enlarging\n");
		SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 3, pHandler);
		return 0;
	}

	Baseinfo.istatus = 1;
	Baseinfo.iebasew = sizex;
	Baseinfo.iebaseh = sizey;
	Baseinfo.lefttime = GetUnixTime() + needtime * 60;

	if (!g_GetCDBOperator().UpdateUserBase(Baseinfo, pUser->getID()))
	{
		log_debug("user base is enlarging\n");
		SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 3, pHandler);
		return 0;
	}

	// 扣除相应的钱
	// delete and update user money
	if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, finalmoney))
	{
		log_debug("money db operate error\n");
		SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 8, pHandler);
		return 0;
	}

	pUser->m_coin = finalcoin;
	pUser->m_money = finalmoney;

	log_debug("user buy enlarge success enlarging\n");
	SendCommonFlagMsgReply(COMMAND_EDIT_GOODS_PID_REPLY, 0, pHandler);
	return 0;
}

// 获取产品
int  CGameServer::Handle_User_Get_Production(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int index = 1; 
	OrmenGoodsStruct_t_list UserMessageList;
	get_production_by_pcate(UserMessageList, pcate);

	int nlen = UserMessageList.size();
	int pageConst = 15;
	int pageNums = nlen / pageConst;
	if (nlen % pageConst != 0)
	{
		pageNums++;
	}

	for (index = 1; index <= pageNums; index++)
	{
		OrmenGoodsStruct_t_list ReplyUserMessageList;
		int limit = pageConst * index;
		for (int i = pageConst; i > 0; i--) {
			ReplyUserMessageList.push_back(UserMessageList[limit - i]);
			if (limit - i >= nlen - 1) {
				break;
			}
		}

		short is_end = 0;
		if (index == pageNums)
		{
			is_end = -1;
		}
		// reply to user
		NETOutputPacket outpacket;
		outpacket.Begin(COMMAND_GET_GOODS_PCATE_REPLY);
		int rflen = ReplyUserMessageList.size();
		outpacket.WriteShort(is_end);	// 是否最后一页
		outpacket.WriteInt(pcate);		// 商品总分类
		outpacket.WriteInt(nlen);		// 该类商品的总数
		outpacket.WriteShort(rflen);	// 当前页所拥有的条目数量
		outpacket.WriteShort(index);	// 当前第几页

		for (int i = 0; i < rflen; i++)
		{
			outpacket.WriteInt(ReplyUserMessageList[i].pframe);
			outpacket.WriteString(ReplyUserMessageList[i].name);
			outpacket.WriteInt(ReplyUserMessageList[i].coin);
			outpacket.WriteInt(ReplyUserMessageList[i].money);
			outpacket.WriteInt(ReplyUserMessageList[i].sellCoin);
			outpacket.WriteInt(ReplyUserMessageList[i].nlevel);
			outpacket.WriteString(ReplyUserMessageList[i].info);
			outpacket.WriteInt(ReplyUserMessageList[i].sizex);
			outpacket.WriteInt(ReplyUserMessageList[i].sizey);
			outpacket.WriteInt(ReplyUserMessageList[i].crossflag);
		}
		outpacket.End();

		log_debug("Handle_User_Get_Production , the packet size is %d\n", outpacket.packet_size());

		pHandler->Send(&outpacket);
	}

	log_debug("Handle_User_Get_Production len is %d, page num is %d\n", nlen, pageNums);
	return 0;
}	

void CGameServer::LoadSystemMsg()
{
	// 加载系统消息列表
	g_GetCDBOperator().GetSystemMessageInfoList(m_systemMsg);
	sort(m_systemMsg.begin(), m_systemMsg.end(), myfunction2);

	// 加载装饰品表
	g_GetCDBOperator().GetSystemOrmenGoodsList(m_ormenGoodslist);
	log_debug("the ormengoods size is %d\n", m_ormenGoodslist.size());

	// 加载扩张表
	g_GetCDBOperator().GetSystemEnlargeTableList(m_enlargelist);
	log_debug("the enlargelist size is %d\n", m_enlargelist.size());

	// 加载获取更多游戏列表
	g_GetCDBOperator().GetMoreGameInfoList(m_gameList);

	// 加载机器总表
	g_GetCDBOperator().GetMechineList(m_systemMechineList, m_ormenGoodslist);
	log_debug("the mechine size is %d\n", m_systemMechineList.size());

	// 加载指导员表和用户等级限制表
	g_GetCDBOperator().GetInstorUserLevelList(m_instorUserlist);
	log_debug("the instoruserlevel size is %d\n", m_instorUserlist.size());

	// 加载指导员升级消耗表
	g_GetCDBOperator().GetInstorUpLevelList(m_instorUpLevelList);
	log_debug("the instoruserlevel size is %d\n", m_instorUpLevelList.size());

	// 加载能源上限升级消耗表
	g_GetCDBOperator().GetPowerMechineUpLevelList(m_powerMechineUpLevelList);
	log_debug("the m_powerMechineUpLevelList size is %d\n", m_powerMechineUpLevelList.size());

	// 加载机器升级消耗表
	g_GetCDBOperator().GetMechineUpLevelComsumList(m_MechineUpLevelComsumList);
	log_debug("the m_MechineUpLevelComsumList size is %d\n", m_MechineUpLevelComsumList.size());

	// 加载情报机收集消耗表
	g_GetCDBOperator().GetInfoMechineCollectTableList(m_infoMechineCollectList);
	log_debug("the m_infoMechineCollectList size is %d\n", m_infoMechineCollectList.size());

	//加载道具合成原材料表
	g_GetCDBOperator().GetMergeResourseTablelist(m_MergeResourseTablelist);
	log_debug("the m_MergeResourseTablelist size is %d\n", m_MergeResourseTablelist.size());

	//加载道具需要材料及数量表
	g_GetCDBOperator().GetMergePKTablelist(m_MergePKTablelist);
	log_debug("the m_MergePKTablelist size is %d\n", m_MergePKTablelist.size());

	//加载能源机生产类型表
	g_GetCDBOperator().GetResourceMechineProductypeList(m_ResourceMechineProducTypeList);
	log_debug("the m_ResourceMechineProducTypeList size is %d\n", m_ResourceMechineProducTypeList.size());


	// 加载机器建筑时间
	g_GetConfigMgr().LoadCfgFromDB();
}	

int CGameServer::Handle_User_System_Message_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int index = pPacket->ReadInt();
	int nlen = m_systemMsg.size();

	if (nlen <= 0)
	{
		SendCommonFlagMsgReply(COMMAND_GET_SYSTEM_MSG_REPLY, 0, pHandler);
		return 0;
	}

	// reply to user
	NETOutputPacket outpacket;
	outpacket.Begin(COMMAND_GET_SYSTEM_MSG_REPLY);
	outpacket.WriteInt(nlen);
	int rflen = m_systemMsg.size();
	outpacket.WriteShort(rflen);
	for (int i = 0; i < rflen; i++)
	{
		outpacket.WriteInt(m_systemMsg[i].msgID);
		outpacket.WriteShort(m_systemMsg[i].messagetype);
		outpacket.WriteString(m_systemMsg[i].title);
		outpacket.WriteString(m_systemMsg[i].content);
		outpacket.WriteInt(m_systemMsg[i].time);
	}
	outpacket.End();

	pHandler->Send(&outpacket);
}

//获取道具合成条件请求返回
int CGameServer::Handle_Get_PropsSynthesis_Condition(TcpHandler * pHandler, NETInputPacket *pPacket, int ProxyId)
{
	int nlen = m_MergePKTablelist.size();
	int rflen = m_MergeResourseTablelist.size();
	Props_list vprops; 

	if (nlen <= 0 || rflen <= 0)
	{
		SendCommonFlagMsgReply(COMMAND_GET_PROPSSYNTHESIS_CONDITION_REPLY, 1, pHandler);
		return 0;
	}

	//reply to user
	NETOutputPacket outpacket;
	outpacket.Begin(COMMAND_GET_PROPSSYNTHESIS_CONDITION_REPLY);
	outpacket.WriteShort(0);
	outpacket.WriteInt(nlen);
	for (int i = 0; i < nlen; i++)
	{
		outpacket.WriteInt(m_MergePKTablelist[i].Gid);
		outpacket.WriteInt(m_MergePKTablelist[i].Pcate);
		outpacket.WriteInt(m_MergePKTablelist[i].Pframe);
		outpacket.WriteInt(m_MergePKTablelist[i].Level);
		
		Props_T resprop;
		int numOfItemNeeded = 0;
		if (m_MergePKTablelist[i].Tid1 != 0)
		{
			numOfItemNeeded += 1;
			if (getTidEqualGid(m_MergePKTablelist[i].Tid1,resprop))
			{
				//vprops.push_back(resprop.pcate);
				//vprops.push_back(resprop.pframe);
				//vprops.push_back(m_MergePKTablelist[i].Nums1);
				resprop.num = m_MergePKTablelist[i].Nums1;
				vprops.push_back(resprop);
			}
		}
		if (m_MergePKTablelist[i].Tid2 != 0)
		{
			numOfItemNeeded += 1;
			if (getTidEqualGid(m_MergePKTablelist[i].Tid2,resprop))
			{
				//vprops.push_back(resprop.pcate);
				//vprops.push_back(resprop.pframe);
				//vprops.push_back(m_MergePKTablelist[i].Nums2);
				resprop.num = m_MergePKTablelist[i].Nums2;
				vprops.push_back(resprop);
			}
		}
		if (m_MergePKTablelist[i].Tid3 != 0)
		{
			numOfItemNeeded += 1;
			if (getTidEqualGid(m_MergePKTablelist[i].Tid3,resprop))
			{
				//vprops.push_back(resprop.pcate);
				//vprops.push_back(resprop.pframe);
				//vprops.push_back(m_MergePKTablelist[i].Nums3);
				resprop.num = m_MergePKTablelist[i].Nums3;
				vprops.push_back(resprop);
			}
		}
		if (m_MergePKTablelist[i].Tid4 != 0)
		{
			numOfItemNeeded += 1;
			if (getTidEqualGid(m_MergePKTablelist[i].Tid4,resprop))
			{
				//vprops.push_back(resprop.pcate);
				//vprops.push_back(resprop.pframe);
				//vprops.push_back(m_MergePKTablelist[i].Nums4);
				resprop.num = m_MergePKTablelist[i].Nums4;
				vprops.push_back(resprop);
			}
		}
		outpacket.WriteInt(numOfItemNeeded);

		for (int j = 0; j < vprops.size(); j++)
		{
			outpacket.WriteInt(vprops[j].pcate);
			outpacket.WriteInt(vprops[j].pframe);
			outpacket.WriteInt(vprops[j].num);
		}

	}
	outpacket.End();

	pHandler->Send(&outpacket);	
}

//返回能源机生产条件请求
int CGameServer::Handle_Get_ResourceProduc_Condition(TcpHandler * pHandler, NETInputPacket *pPacket, int ProxyId)
{
	int pcate = pPacket->ReadInt();
	int pframe = pPacket->ReadInt();
	int pid = pPacket->ReadInt();
	int nlen = m_ResourceMechineProducTypeList.size();
	ResourceProduc_list  ResourceProduc;

	if (nlen <= 0)
	{
		SendCommonFlagMsgReply(COMMAND_GET_RESOURCEPRODUC_CONDITION_REPLY, 1, pHandler);
		return 0;
	}

	NETOutputPacket outpacket;
	outpacket.Begin(COMMAND_GET_RESOURCEPRODUC_CONDITION_REPLY);
	outpacket.WriteInt(pid);
    
	int NumMode = 0;
	for (int i = 0; i < nlen; i++)
	{
		ResourceProduc_T ResProduce;
		if (pcate == m_ResourceMechineProducTypeList[i].Pcate && pframe == m_ResourceMechineProducTypeList[i].Pframe)
		{
			NumMode += 1;
		}
		ResProduce.Level = m_ResourceMechineProducTypeList[i].Level;
		ResProduce.NeedTime = m_ResourceMechineProducTypeList[i].NeedTime;
		ResProduce.Coin = m_ResourceMechineProducTypeList[i].Coin;
		ResProduce.Power = m_ResourceMechineProducTypeList[i].Power;
		ResProduce.UserLevel = m_ResourceMechineProducTypeList[i].UserLevel;

		ResourceProduc.push_back(ResProduce);
	}

	outpacket.WriteInt(NumMode);
	outpacket.WriteString("test");  //返回生产方式名称，暂时写死，因为表还没定，到时候再修改

	for (int j = 0; j < ResourceProduc.size(); j++)
	{
		outpacket.WriteInt(ResourceProduc[j].Level);
		outpacket.WriteInt(ResourceProduc[j].NeedTime);
		outpacket.WriteInt(ResourceProduc[j].Coin);
		outpacket.WriteInt(ResourceProduc[j].Power);
		outpacket.WriteInt(ResourceProduc[j].UserLevel);
	}
	outpacket.End();

	pHandler->Send(&outpacket);
}

// 能源机生产
int CGameServer:: Handle_Mechine_ResourceProduc(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int pid = pPacket->ReadInt();
	int mode = pPacket->ReadInt();

	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );
	if (pUser == NULL)
	{
		// 失败
		log_debug("Handle_Mechine_ResourceProduc user is null\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 1, pHandler);
		return 0;
	}

	int PlayMoney = pUser->m_coin;     //获取用户的普通货币

	ResourceMechineProductype_T ResMePro;
	ResourceProducTable_T UserResProInfo; 
	User_Experiment_Mechine_t MechineItem;

	bool ret = getUserMechine_By_Pid(pUser->m_userBuyMechineList, MechineItem, pid);
	if (!ret)
	{
		//log_debug("getUserMechine_By_Pid user is null\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 1, pHandler);
		return 0;
	}

	//判断选择的生产方式是否满足能源机当前等级，能源机的当前状态是否满能进行生产（0-正常状态)
	if (mode > MechineItem.level || 0 != MechineItem.status)
	{
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 1, pHandler);
		return 0;	
	}

	//通过用户购买的机器表去查找资源机生产类型对应的机器
	bool rest = getResourceMechine_By_pacte_pfram_level(MechineItem.pcate, MechineItem.pframe, MechineItem.level, ResMePro);
	if (!rest)
	{
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 1, pHandler);
		return 0;
	}

	if (PlayMoney < ResMePro.Coin)
	{
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 1, pHandler);
		return 0;
	}

	int finalcoin = PlayMoney - ResMePro.Coin; // 扣取用户普通货币
	if (finalcoin < 0)
	{
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 1, pHandler);
		return 0;
	}
	
	//更新用户普通货币数据
	if (!g_GetCDBOperator().UpdateUserMoney(pUser->getID(), finalcoin, pUser->m_money))
	{
		log_debug("Handle_User_Res_Mechine_Produc money db operate error\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 1, pHandler);
		return 0;
	}

	pUser->m_coin = PlayMoney - ResMePro.Coin; //更新用户系统内存中中的普通货币


	//保存用户能源生产数据
	UserResProInfo.Pcate = ResMePro.Pcate;
	UserResProInfo.Pframe = ResMePro.Pframe;
	UserResProInfo.Pmode = mode;
	UserResProInfo.Level = ResMePro.Level;
	UserResProInfo.State = 12;   //正在生产
	UserResProInfo.Usetime = 0;  //刚开始生产使用时间为0

	//更新用户能源机生产表
	bool result = g_GetCDBOperator().UpdateUserProducTable(UserResProInfo);
	if (!result)
	{
		log_debug("UpdateUserProducTable record db operate error\n");
		SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 1, pHandler);
		return 0;
	}

	log_debug("Handle_Mechine_ResourceProduc db operate success\n");
	SendCommonEditFlagMsgReply( COMMAND_MECHINE_RESOURCEPRODUC_REPLAY, pid, 0, pHandler);
	return 0;
}


// get list
int CGameServer::Handle_User_Get_More_Game_List(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	int nlen = m_gameList.size();

	if (nlen <= 0)
	{
		SendCommonFlagMsgReply(COMMAND_GET_MOREGAMELIST_MESSAGE, 0, pHandler);
		return 0;
	}

	// reply to user
	NETOutputPacket outpacket;
	outpacket.Begin(COMMAND_GET_MOREGAMELIST_MESSAGE);
	outpacket.WriteInt(nlen);
	for (int i = 0; i < nlen; i++)
	{
		outpacket.WriteString(m_gameList[i].url);
		outpacket.WriteString(m_gameList[i].iden);
		outpacket.WriteString(m_gameList[i].icon);
	}
	outpacket.End();

	pHandler->Send(&outpacket);
}

int CGameServer::Handle_User_Send_Message_To_APN(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string senderName = pPacket->ReadString();
	string receiveName = pPacket->ReadString();
	string message = pPacket->ReadString();
	int time = pPacket->ReadInt();

	int messagetype = 1;
	int msgid = 0;
	if (!g_GetCDBOperator().AddMessage(msgid, receiveName, senderName, message, messagetype, time))
	{
		return 0;
	}

	return 0;
}

int CGameServer::Handle_User_Add_FriendMessage(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	string FriendName = pPacket->ReadString();
	string SelfName = pPacket->ReadString();

	// check
	if (g_GetCDBOperator().CheckIsAFriend(FriendName, SelfName))
	{
		// user no exist
		SendCommonFlagMsgReply(COMMAND_ADDFRIEND_REPLY, 3, pHandler);
		return 0;
	}
	if (!g_GetCDBOperator().CheckAddFriend(FriendName))
	{
		// user no exist
		SendCommonFlagMsgReply(COMMAND_ADDFRIEND_REPLY, 2, pHandler);
		return 0;
	}
	
	// insert a message to db
	int msgid = 0;
	string message = SelfName + " want to add you as a friend";
	int messagetype = 2;	// add friend type
	int unixtime = GetUnixTime();

	if (!g_GetCDBOperator().AddMessage(msgid, FriendName, SelfName, message, messagetype, unixtime))
	{
		// user no exist
		SendCommonFlagMsgReply(COMMAND_ADDFRIEND_REPLY, 1, pHandler);
		return 0;
	}

	// success 
	SendCommonFlagMsgReply(COMMAND_ADDFRIEND_REPLY, 0, pHandler);

	// send message to hall
	NETOutputPacket outpacket;
	outpacket.Begin(CLIENT_ADDFRIEND_MSG);
	outpacket.WriteString(FriendName);
	outpacket.WriteString(SelfName);
	outpacket.WriteInt(msgid);
	outpacket.WriteString(message);
	outpacket.WriteInt(unixtime);
	outpacket.WriteShort(messagetype);
	outpacket.End();

	pHandler->Send(&outpacket);

	return 0;
}

int CGameServer::Handle_Regist_User(TcpHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	log_debug("\n\nEnter Handle_Regist_User\n");
	string UserName = pPacket->ReadString();
	string PassWord = pPacket->ReadString();
	string Mail = pPacket->ReadString();
	int isPub = pPacket->ReadInt();			//是否推广
	int isNewData = pPacket->ReadInt();
	string iphoneMac;
	if (isNewData == 1)
	{
		iphoneMac = pPacket->ReadString();
	}
	int gameMid = 0;
	log_debug("regist user %s\n", UserName.c_str());
	bool ret = g_GetRegistSystem().Check_User_Regist(UserName);
	if (!ret)
	{
		SendCommonFlagMsgReply(COMMAND_REGIST_REPLY, 1, pHandler);
		return 0;
	}
	// 创建一个新玩家,此时获取gameid
	CreateNewUser(UserName, gameMid);
	int flag = 1; // 0成功，1失败
	if (gameMid > 0)
	{
		flag = 0;
	}
	// 返回创建的情况
	SendCommonFlagMsgReply(COMMAND_REGIST_REPLY, flag, pHandler);
	if (flag == 1)
	{
		return 0; // 注册不成功
	}
	// create user success
	bool cret = g_GetRegistSystem().Iphone_User_Regist(UserName, PassWord, Mail, isPub, gameMid);
	if (!cret)
	{
		SendCommonFlagMsgReply(COMMAND_REGIST_REPLY, 1, pHandler);
		return 0;
	}
	if (ProxyId != 0)
	{
		// 帮助客户端向大厅注册游戏ID
		NETOutputPacket registpacket;
		registpacket.Begin(CLIENT_REGIST_GAMEID);
		registpacket.WriteInt(ProxyId);
		registpacket.WriteInt(gameMid);
		registpacket.End();
		pHandler->Send(&registpacket);
		// regist name 
		Handle_User_Regist_UserName(pHandler, UserName, gameMid, ProxyId);
	}

	// 根据配置返回信息, 然后登陆
	SendConfigMsg(isNewData, iphoneMac, gameMid, flag, pHandler);
	log_debug("\nLeave Handle_Regist_User\n\n");
	return 0;
}

int CGameServer::Handle_User_GetReward(SocketHandler * pHandler, NETInputPacket * pPacket, int ProxyId)
{
	CPlayer * pUser = reinterpret_cast<CPlayer *>( pHandler->GetUserData() );
	if( pUser != NULL )
	{
		NETOutputPacket outPacket;

		//todo
		//int reward_id = pPacket->ReadInt();
		int mid = pUser->getID();
		bool ret = g_lcuky.GetReward(mid);
		
		if (!ret)
		{
			// todo 返回金币
			outPacket.Begin( HERO_COMMAND_USER_GET_REWARD );
			outPacket.WriteInt(0); //成功标志
			outPacket.End();
			pUser->SendMsg( &outPacket );
		}
		else
		{
			// todo 返回金币
			outPacket.Begin( HERO_COMMAND_USER_GET_REWARD );
			outPacket.WriteInt(1); //成功标志
			outPacket.WriteInt(mid);
			outPacket.WriteInt(1000);
			outPacket.End();
			pUser->SendMsg( &outPacket );
		}


	}
	return 0;
}

// 用户登录时服务器发送在线好友列表给用户
void CGameServer::OnLogon(CPlayer * pUser, int haveemail, string email, int isFb)
{
	NETOutputPacket ploginPacket;
	g_GetCDBOperator().GetLoginReply(pUser->getID(), ploginPacket);
	ploginPacket.WriteShort(haveemail);
	if (haveemail != 0)
	{
		ploginPacket.WriteString(email.c_str());
	}
	// add isbingfb, headimgindex, fbid
	short isbingfb = 0;
	short headimageindex = 0;
	if (haveemail == 1)
	{
		string fbid;
		g_GetCDBOperator().GetUserHeadimageIndex(pUser->getID(), headimageindex, fbid);
		if (fbid.size() > 3)
		{
			isbingfb = 1;
		}
		ploginPacket.WriteShort(isbingfb);
		ploginPacket.WriteShort(headimageindex);
		if (headimageindex == 100)
		{
			ploginPacket.WriteString(fbid);
		}
	}
	else
	{
		isbingfb = 0;
		g_GetCDBOperator().GetUserHeadimageIndex_notRegist(pUser->getID(), headimageindex);
		if (headimageindex == 100)
		{
			headimageindex = 0;
		}
		ploginPacket.WriteShort(isbingfb);
		ploginPacket.WriteShort(headimageindex);
	}

	if (isFb)	//fb登陆需要修改协议
	{
		ploginPacket.SetBegin(COMMAND_FBLOGIN_REPLY);
		ploginPacket.WriteString(pUser->GetName());
		ploginPacket.WriteString(pUser->GetPassword());
	}
	ploginPacket.End();

	pUser->SendMsg( &ploginPacket );

	//	return base info
	BaseInfo_Struct_t Baseinfo;
	if (g_GetCDBOperator().CheckUserBase(Baseinfo, pUser->getID()))
	{
		// enlarge over left time
		int leftime = 0;
		// 加载基地信息成功
		if (Baseinfo.istatus == 1)	// 正在扩张
		{
			leftime = Baseinfo.lefttime - GetUnixTime();
			if (leftime <= 0)
			{
				// 扩张结束，更新状态 数据库状态和baseinfo
				// 防止失败的情况
				int ebasew = Baseinfo.iebasew;
				int ebaseh = Baseinfo.iebaseh; 
				int sbasew = Baseinfo.ibasew;
				int sbaseh = Baseinfo.ibaseh;

				Baseinfo.istatus = 0;
				Baseinfo.lefttime = 0;
				Baseinfo.ibasew = Baseinfo.iebasew;
				Baseinfo.ibaseh = Baseinfo.iebaseh;
				Baseinfo.iebasew = 0;
				Baseinfo.iebaseh = 0;
				Baseinfo.iwallw = Baseinfo.ibasew;

				// update db

				int issuccess = 0;
				if (!g_GetCDBOperator().UpdateUserBase(Baseinfo, pUser->getID()))
				{
					// 更新数据库失败
					Baseinfo.istatus = 1;
					Baseinfo.lefttime = leftime;
					Baseinfo.ibasew = sbasew;
					Baseinfo.ibaseh = sbaseh;
					Baseinfo.iebasew = ebasew;
					Baseinfo.iebaseh = ebaseh;
					Baseinfo.iwallw = Baseinfo.ibasew;
					issuccess = 1;
				}

				// 发送扩建完成
				NETOutputPacket outpacket;
				outpacket.Begin(COMMAND_ENLARGE_COMPLEY_REPLY);
				outpacket.WriteInt(issuccess);//0成功1失败
				outpacket.End();
				pUser->SendMsg( &outpacket );
			}
			else
			{
				Baseinfo.lefttime = leftime;
			}
		}
		// send back the data to client
		NETOutputPacket ploginBasePacket;
		ploginBasePacket.Begin(COMMAND_RETURN_BASE_DATA);
		ploginBasePacket.WriteInt(Baseinfo.ibasew);
		ploginBasePacket.WriteInt(Baseinfo.ibaseh);
		ploginBasePacket.WriteInt(Baseinfo.iwallw);
		ploginBasePacket.WriteInt(Baseinfo.iwallh);
		ploginBasePacket.WriteInt(Baseinfo.istatus);
		ploginBasePacket.WriteInt(Baseinfo.lefttime);
		ploginBasePacket.End();
		pUser->SendMsg( &ploginBasePacket );

		pUser->m_istatus = Baseinfo.istatus;
		pUser->m_ibasew = Baseinfo.ibasew;
		pUser->m_ibaseh = Baseinfo.ibaseh;

		log_debug("load base success, user id is %d, left time is %d\n", pUser->getID(), leftime);	
	}
	
	// get user production and return
	vector<UserOrmenGoodsStruct_t> Productionlist;
	MechineType_PcatePframe_T ProductionType = g_GetConfigMgr().getOrm_Floor_Type();
	g_GetCDBOperator().GetUserProduction(Productionlist, ProductionType, pUser->getID(), 1);
	ProductionType = g_GetConfigMgr().getOrm_Floor_Goods_Type();
	g_GetCDBOperator().GetUserProduction(Productionlist, ProductionType, pUser->getID(), 1);
	ProductionType = g_GetConfigMgr().getOrm_Wall_Type();
	g_GetCDBOperator().GetUserProduction(Productionlist, ProductionType, pUser->getID(), 1);
	ProductionType = g_GetConfigMgr().getOrm_WallGoods_Type();
	g_GetCDBOperator().GetUserProduction(Productionlist, ProductionType, pUser->getID(), 1);

	if (Productionlist.size() > 0)
	{
		ReUserOrmenGoodsStruct_t_list ReturnProduction;

		int len = m_ormenGoodslist.size();
		int uplen = Productionlist.size();
		
		for (int j = 0; j < uplen; j++)
		{
			int rpcate = Productionlist[j].pcate;
			int rpframe = Productionlist[j].pframe;

			for (int i = 0; i < len; i++)
			{
				OrmenGoodsStruct_t onep = m_ormenGoodslist[i];
				if (rpcate == onep.pcate && rpframe == onep.pframe)
				{
					// 获取所需信息
					ReUserOrmenGoodsStruct_t oneReproduction;
					oneReproduction.pid = Productionlist[j].pid;
					oneReproduction.pcate = rpcate;
					oneReproduction.pframe = rpframe;
					oneReproduction.name = m_ormenGoodslist[i].name;
					oneReproduction.sellCoin = m_ormenGoodslist[i].sellCoin;
					oneReproduction.sizex = m_ormenGoodslist[i].sizex;
					oneReproduction.sizey = m_ormenGoodslist[i].sizey;
					oneReproduction.crossFlag = m_ormenGoodslist[i].crossflag;
					oneReproduction.curDir = Productionlist[j].curDir;
					oneReproduction.locationx = Productionlist[j].locationx;
					oneReproduction.locationy = Productionlist[j].locationy;

					ReturnProduction.push_back(oneReproduction);
					break;
				}
			}
		}

		log_debug("user production count is %d\n", ReturnProduction.size());

		int index = 1; 

		int nlen = ReturnProduction.size();
		int pageConst = 15;
		int pageNums = nlen / pageConst;
		if (nlen % pageConst != 0)
		{
			pageNums++;
		}

		for (index = 1; index <= pageNums; index++)
		{
			ReUserOrmenGoodsStruct_t_list ReplyUserMessageList;
			int limit = pageConst * index;
			for (int i = pageConst; i > 0; i--) {
				ReplyUserMessageList.push_back(ReturnProduction[limit - i]);
				if (limit - i >= nlen - 1) {
					break;
				}
			}

			short is_end = 0;
			if (index == pageNums)
			{
				is_end = -1;
			}
			// reply to user
			NETOutputPacket outpacket;
			outpacket.Begin(COMMAND_RETURN_PRODUCTION_DATA);
			int rflen = ReplyUserMessageList.size();
			outpacket.WriteShort(is_end);	// 是否最后一页
			outpacket.WriteInt(rflen);	// 当前页所拥有的条目数量
			for (int i = 0; i < rflen; i++)
			{
				outpacket.WriteInt(ReplyUserMessageList[i].pid);
				outpacket.WriteInt(ReplyUserMessageList[i].pcate);
				outpacket.WriteInt(ReplyUserMessageList[i].pframe);
				outpacket.WriteString(ReplyUserMessageList[i].name);
				outpacket.WriteInt(ReplyUserMessageList[i].sellCoin);
				outpacket.WriteInt(ReplyUserMessageList[i].sizex);
				outpacket.WriteInt(ReplyUserMessageList[i].sizey);
				outpacket.WriteInt(ReplyUserMessageList[i].crossFlag);
				outpacket.WriteInt(ReplyUserMessageList[i].curDir);
				outpacket.WriteInt(ReplyUserMessageList[i].locationx);
				outpacket.WriteInt(ReplyUserMessageList[i].locationy);
			}
			outpacket.End();
			log_debug("GetUserProduction , the packet size is %d\n", outpacket.packet_size());
			pUser->SendMsg(&outpacket);
		}
	}
	else
	{
		log_debug("user have no productions\n");
		NETOutputPacket ploginProPacket;
		ploginProPacket.Begin(COMMAND_RETURN_PRODUCTION_DATA);
		ploginProPacket.WriteShort(-1);
		ploginProPacket.WriteInt(0);
		ploginProPacket.End();
		pUser->SendMsg( &ploginProPacket );
	}

	//  获取用户机器返回
	vector<ReturnMechineInfo>  returnMechineList;
	// 实验机机器
	MechineType_PcatePframe_T ExperimentMechinePcate = g_GetConfigMgr().getMechine_Experiment_Type();
	packet_mechine_for_user(returnMechineList, ExperimentMechinePcate, pUser->getID(), pUser->m_userExperimentMechine);
	// 指导机器
	MechineType_PcatePframe_T InstorMechinePcate = g_GetConfigMgr().getMechine_Instor_Type();
	packet_mechine_for_user(returnMechineList, InstorMechinePcate, pUser->getID(), pUser->m_userInstorMechine);
	// 特殊机器
	MechineType_PcatePframe_T PowerMechinePcate = g_GetConfigMgr().getMechine_PowerMechine_Type();
	packet_mechine_for_user(returnMechineList, PowerMechinePcate, pUser->getID(), pUser->m_userBuyMechineList);
	// 情报机器
	MechineType_PcatePframe_T InfoMechinePcate = g_GetConfigMgr().getMechine_Info_Type();
	packet_mechine_for_user(returnMechineList, InfoMechinePcate, pUser->getID(), pUser->m_userBuyMechineList);
	// 资源机
	MechineType_PcatePframe_T ResourceMechinePcate = g_GetConfigMgr().getMechine_Resource_Type();
	packet_mechine_for_user(returnMechineList, ResourceMechinePcate, pUser->getID(), pUser->m_userBuyMechineList);

	sent_user_mechine(returnMechineList, pUser);

	// 启动用户定时器来检查任务

	pUser->StartCheckFunc(g_GetConfigMgr().getCheckTime());
	pUser->StartDBWriteTimer(g_GetConfigMgr().getWriteDBTime());
	//  check the base is exist
	//int date = g_lcuky.CheckReward(pUser->getID());
	//NETOutputPacket outPacket;
	//outPacket.Begin( HERO_COMMAND_IPHONE_LOGIN );
	//outPacket.WriteInt(1); //成功标志
	//outPacket.WriteInt(pUser->getID());
	//outPacket.WriteInt(date);
	//outPacket.End();
	//pUser->SendMsg( &outPacket );
}

// 用户离线时将SERVER_COMMAND_FRIEND_LOGOUT消息发送给其好友，如果自已是隐身状态的话，则不发送。
void CGameServer::OnLogout( CPlayer * pUser )
{
    log_debug("OnLogout : %d \n",pUser->getID());
	pUser->OnUserLogoutServer();
    g_GetPlayerMgr().DelPlayer(pUser->getID()); 
}

void CGameServer::packet_mechine_for_user(ReturnMechineInfoTable_list  & returnMechineList, MechineType_PcatePframe_T & MechinePcate, int gameid, UserBuyMechineTable_list & m_userMechineList)
{
	SysMechineInfo MechineItem;
	bool isfound = getMechineInfoByPcatePframe(MechinePcate.pcate, MechinePcate.pframe, MechineItem);
	if (isfound)
	{
		g_GetCDBOperator().GetUserBuyMechineList(m_userMechineList, returnMechineList, MechineItem, gameid);
	}
}

void CGameServer::packet_mechine_for_user(ReturnMechineInfoTable_list  & returnMechineList, MechineType_PcatePframe_T & MechinePcate, int gameid, User_Experiment_Mechine_t & m_userMechine)
{
	SysMechineInfo MechineItem;
	bool isfound = getMechineInfoByPcatePframe(MechinePcate.pcate, MechinePcate.pframe, MechineItem);
	if (isfound)
	{
		User_Experiment_Mechine_t UserExperimentMechine;
		g_GetCDBOperator().CheckUserExperimentMechine(UserExperimentMechine, MechineItem, gameid);
		ReturnMechineInfo returnMechine;
		returnMechine.gid = UserExperimentMechine.Gid;
		returnMechine.pcate = UserExperimentMechine.pcate;
		returnMechine.pframe = UserExperimentMechine.pframe;
		returnMechine.name = MechineItem.name;
		returnMechine.sellprice = MechineItem.sellprice;
		returnMechine.sizex = MechineItem.sizex;
		returnMechine.sizey = MechineItem.sizey;
		returnMechine.level = UserExperimentMechine.level;
		returnMechine.curDir = UserExperimentMechine.curDir;
		returnMechine.mpositionx = UserExperimentMechine.positionx;
		returnMechine.mpositiony = UserExperimentMechine.positiony;
		returnMechine.status = UserExperimentMechine.status;

		m_userMechine = UserExperimentMechine;

		returnMechineList.push_back(returnMechine);
	}
	else 
	{
		log_error("sysmechine table error! can't found the special mechine\n");
	}
}

bool CGameServer::getUserMechine_By_Pid(UserBuyMechineTable_list & m_userMechineList, User_Experiment_Mechine_t & m_userMechine, int gid)
{
	int len = m_userMechineList.size();
	bool isfound = false;

	for (int i = 0; i < len; i++)
	{
		User_Experiment_Mechine_t oneItem = m_userMechineList[i];
		if (oneItem.Gid == gid)
		{
			m_userMechine = oneItem;
			isfound = true;
			break;
		}
	}

	return isfound;
}

bool CGameServer::ChangeUserMechine_By_Pid(UserBuyMechineTable_list & m_userMechineList, User_Experiment_Mechine_t & m_userMechine)
{
	int len = m_userMechineList.size();
	bool isfound = false;

	for (int i = 0; i < len; i++)
	{
		User_Experiment_Mechine_t oneItem = m_userMechineList[i];
		if (oneItem.Gid == m_userMechine.Gid)
		{
			m_userMechineList[i].curDir = m_userMechine.curDir;
			m_userMechineList[i].endtime = m_userMechine.endtime;
			m_userMechineList[i].isDirty = m_userMechine.isDirty;
			m_userMechineList[i].level = m_userMechine.level;
			m_userMechineList[i].position = m_userMechine.position;
			m_userMechineList[i].positionx = m_userMechine.positionx;
			m_userMechineList[i].positiony = m_userMechine.positiony;
			m_userMechineList[i].status = m_userMechine.status;
			m_userMechineList[i].isDirty = true;
			isfound = true;
			break;
		}
	}

	return isfound;
}

bool CGameServer::ChangUserResMechineProduc(UserResourceProducTable_list & userResMeProduc, ResourceProducTable_T & UserResMechineProduc)
{
	int len = userResMeProduc.size();
	bool isfound = false;

	for (int i = 0; i < len; i++)
	{
		ResourceProducTable_T oneItem = userResMeProduc[i];
		if (oneItem.Gid == UserResMechineProduc.Gid && oneItem.Mid == UserResMechineProduc.Mid)
		{
			userResMeProduc[i] = UserResMechineProduc;
			userResMeProduc[i].isDirty = true;
			isfound = true;
			break;
		}
	}

	return isfound;
}

bool CGameServer::getUserInfoMechineCollect_By_Pid(UserInfoMechineCollectTable_list & userInfoCollectList, UserInfoMechineCollect_T & UserInfoCollect, int gid)
{
	int len = userInfoCollectList.size();
	bool isfound = false;

	for (int i = 0; i < len; i++)
	{
		UserInfoMechineCollect_T oneItem = userInfoCollectList[i];
		if (oneItem.Gid == gid)
		{
			UserInfoCollect = oneItem;
			isfound = true;
			break;
		}
	}
 
	return isfound;
}

bool CGameServer::getUserResMechineCollect_By_Pid(UserResourceProducTable_list & userResCollectList, ResourceProducTable_T & userResCollect, int gid)
{
	int len = userResCollectList.size();
	bool isfound = false;

	for (int i = 0; i < len; i++)
	{
		ResourceProducTable_T oneItem = userResCollectList[i];
		if (oneItem.Gid == gid)
		{
			userResCollect = oneItem;
			isfound = true;
			break;
		}
	}
	return isfound;
}

bool CGameServer::ChangeUserInfoMechineCollect_By_Pid(UserInfoMechineCollectTable_list & userInfoCollectList, UserInfoMechineCollect_T & UserInfoCollect)
{
	int len = userInfoCollectList.size();
	bool isfound = false;

	for (int i = 0; i < len; i++)
	{
		UserInfoMechineCollect_T oneItem = userInfoCollectList[i];
		if (oneItem.Gid == UserInfoCollect.Gid)
		{
			userInfoCollectList[i].LeftTime = UserInfoCollect.LeftTime;
			userInfoCollectList[i].Status = UserInfoCollect.Status;
			userInfoCollectList[i].isDirty = true;
			isfound = true;
			break;
		}
	}

	return isfound;
}

void CGameServer::sent_user_mechine(vector<ReturnMechineInfo> & returnMlist, CPlayer *pPlayer)
{
	int index = 1; 

	int nlen = returnMlist.size();
	int pageConst = 15;
	int pageNums = nlen / pageConst;
	if (nlen % pageConst != 0)
	{
		pageNums++;
	}

	for (index = 1; index <= pageNums; index++)
	{
		vector<ReturnMechineInfo> ReplyUserMessageList;
		int limit = pageConst * index;
		for (int i = pageConst; i > 0; i--) {
			ReplyUserMessageList.push_back(returnMlist[limit - i]);
			if (limit - i >= nlen - 1) {
				break;
			}
		}

		short is_end = 0;
		if (index == pageNums)
		{
			is_end = -1;
		}
		// reply to user
		NETOutputPacket outpacket;
		outpacket.Begin(COMMAND_SEND_USERMECHINE_REPLY);
		int rflen = ReplyUserMessageList.size();
		outpacket.WriteShort(is_end);	// 是否最后一页
		outpacket.WriteInt(rflen);		// 当前页所拥有的条目数量
		for (int i = 0; i < rflen; i++)
		{
			outpacket.WriteInt(ReplyUserMessageList[i].gid);
			outpacket.WriteInt(ReplyUserMessageList[i].pcate);
			outpacket.WriteInt(ReplyUserMessageList[i].pframe);
			outpacket.WriteString(ReplyUserMessageList[i].name);
			outpacket.WriteInt(ReplyUserMessageList[i].sellprice);
			outpacket.WriteInt(ReplyUserMessageList[i].sizex);
			outpacket.WriteInt(ReplyUserMessageList[i].sizey);
			outpacket.WriteShort(ReplyUserMessageList[i].level);
			outpacket.WriteShort(ReplyUserMessageList[i].curDir);
			outpacket.WriteInt(ReplyUserMessageList[i].mpositionx);
			outpacket.WriteInt(ReplyUserMessageList[i].mpositiony);
			outpacket.WriteShort(ReplyUserMessageList[i].status);
		}
		outpacket.End();
		log_debug("sent_user_mechine , the packet size is %d\n", outpacket.packet_size());
		pPlayer->SendMsg(&outpacket);
	}
}

int CGameServer::getNeedUserLevel(int instorLevel)
{
	int len = m_instorUserlist.size();
	int level = 0;
	for (int i = 0; i < len; i++)
	{
		if (instorLevel == m_instorUserlist[i].instorLevel)
		{
			level = m_instorUserlist[i].userLevel;
			break;
		}
	}
	return level;
}

bool CGameServer::getInstorUplevelComsum(int instorLevel, InstrutorUpLevelTable_T & oneitem)
{
	int len = m_instorUpLevelList.size();
	bool isfound = false;
	for (int i = 0; i < len; i++)
	{
		if (instorLevel == m_instorUpLevelList[i].Level)
		{
			oneitem = m_instorUpLevelList[i];
			isfound = true;
			break;
		}
	}

	return isfound;
}

bool CGameServer::getMechineUplevelComsum(int pcate, int pframe, int level, MechineUpLevelComsumTable_T & oneitem)
{
	int len = m_MechineUpLevelComsumList.size();
	bool isfound = false;
	for (int i = 0; i < len; i++)
	{
		if (level == m_MechineUpLevelComsumList[i].Level && pcate == m_MechineUpLevelComsumList[i].pcate && pframe == m_MechineUpLevelComsumList[i].pframe)
		{
			oneitem = m_MechineUpLevelComsumList[i];
			isfound = true;
			break;
		}
	}

	return isfound;
}

bool CGameServer::getCollectTimeByCollItem(UserInfoMechineCollect_T & item, int level)
{
	int len = m_infoMechineCollectList.size();
	bool isfound = false;
	item.NeedTime = 0;
	for (int i = 0; i < len; i++)
	{
		if (level == m_infoMechineCollectList[i].Level && item.pcate == m_infoMechineCollectList[i].pcate && item.pframe == m_infoMechineCollectList[i].pframe)
		{
			item.NeedTime = m_infoMechineCollectList[i].Time;
			isfound = true;
			break;
		}
	}

	return isfound;
}

bool CGameServer::getCollectItemByPcateLevel(InfoMechineCollectTable_T & item, int pcate, int pframe, int level)
{
	int len = m_infoMechineCollectList.size();
	bool isfound = false;
	for (int i = 0; i < len; i++)
	{
		if (level == m_infoMechineCollectList[i].Level && pcate == m_infoMechineCollectList[i].pcate && pframe == m_infoMechineCollectList[i].pframe)
		{
			item = m_infoMechineCollectList[i];
			isfound = true;
			break;
		}
	}

	return isfound;
}

bool CGameServer::getTidEqualGid(int tid, Props_T & result)
{
	int len = m_MergeResourseTablelist.size();
	bool isfound = false;

		for (int i = 0; i < len; i++)
		{
				if (tid == m_MergeResourseTablelist[i].Gid)
				{
					result.pcate = m_MergeResourseTablelist[i].pcate;
					result.pframe = m_MergeResourseTablelist[i].pframe;
					isfound = true;
					break;
				}	
		}
	
	return isfound;
}

//通过用户购买机器表的主分类、次分类、等级，查找能源机表对应的机器信息
bool CGameServer::getResourceMechine_By_pacte_pfram_level(int pacte, int pframe, int level, ResourceMechineProductype_T & ResMechineInfo)
{
	int len = m_ResourceMechineProducTypeList.size();
	bool isfound = false;

	for (int i = 0; i < len; i++)
	{
		//ResourceMechineProductype_T oneitem ;
		if (pacte == m_ResourceMechineProducTypeList[i].Pcate && pframe == m_ResourceMechineProducTypeList[i].Pframe && level == m_ResourceMechineProducTypeList[i].Level)
		{
			ResMechineInfo = m_ResourceMechineProducTypeList[i];
			isfound = true;
			break;
		}
	}
	
	return isfound;
}

void CGameServer::SendCommonEditFlagMsgReply( int CMD, int pid, int flag, TcpHandler * pHandler)
{
	NETOutputPacket outpacket;
	outpacket.Begin(CMD);
	outpacket.WriteInt(pid);
	outpacket.WriteShort(flag);//0成功1失败
	outpacket.End();

	pHandler->Send(&outpacket);
}

void CGameServer::SendCommonMechineStatusReply( int CMD, int pid, int flag, int leftTime, int money, TcpHandler * pHandler)
{
	NETOutputPacket outpacket;
	outpacket.Begin(CMD);
	outpacket.WriteInt(pid);
	outpacket.WriteShort(flag);//0成功1失败
	outpacket.WriteInt(leftTime);
	outpacket.WriteInt(money);
	outpacket.End();

	pHandler->Send(&outpacket);
}

void CGameServer::SendCommonFlagMsgReply( int CMD, int flag, TcpHandler * pHandler)
{
	NETOutputPacket outpacket;
	outpacket.Begin(CMD);
	outpacket.WriteInt(flag);//0成功1失败
	outpacket.End();

	pHandler->Send(&outpacket);
}

void CGameServer::SendInstorUpLevelReply( int CMD, int flag, int pid, ReturnInstorUpLevelTJ & InstorUpLevelTiaoJian, TcpHandler * pHandler)
{
	NETOutputPacket outpacket;

	outpacket.Begin(CMD);
	outpacket.WriteInt(pid);
	outpacket.WriteShort(flag);
	outpacket.WriteInt(InstorUpLevelTiaoJian.otherNeed);
	outpacket.WriteInt(InstorUpLevelTiaoJian.needLevel);
	outpacket.WriteInt(InstorUpLevelTiaoJian.coin);
	outpacket.WriteInt(InstorUpLevelTiaoJian.moneyTime);
	outpacket.WriteInt(InstorUpLevelTiaoJian.needTime);
	outpacket.End();
	pHandler->Send(&outpacket);
}

void CGameServer::SendBuyFlagMsgReply( int CMD, int flag, int pid, int pcate, int pframe, int locationx, int locationy, TcpHandler * pHandler)
{
	NETOutputPacket outpacket;
	outpacket.Begin(CMD);
	outpacket.WriteShort(flag);
	outpacket.WriteInt(pid);
	outpacket.WriteInt(pcate);
	outpacket.WriteInt(pframe);
	outpacket.WriteInt(locationx);
	outpacket.WriteInt(locationy);
	outpacket.End();

	pHandler->Send(&outpacket);
}

void CGameServer::Handle_User_Regist_UserName(TcpHandler * pHandler, string & UserName, int gameid, int ProxyId)
{
	NETOutputPacket registpacket;
	registpacket.Begin(CLIENT_REGIST_NAME);
	registpacket.WriteString(UserName);
	registpacket.WriteInt(gameid);
	registpacket.WriteInt(ProxyId);
	registpacket.End();
	pHandler->Send(&registpacket);
}

void CGameServer::SendConfigMsg( int isNewData, string iphoneMac, int gameMid, int flag, TcpHandler * pHandler )
{
	// 根据配置返回信息, 成功的情况下才返回
	if (flag == 0) // 0成功
	{
		ReplyType_login r = g_GetConfigMgr().getRegistRely();
		if (r.cmd > 0)
		{
			// login 
			NETOutputPacket ploginPacket;
			g_GetCDBOperator().GetRegistReply(gameMid, ploginPacket);
			ploginPacket.WriteShort(isNewData);
			if (isNewData == 1)
			{
				ploginPacket.WriteString(iphoneMac.c_str());
			}
			ploginPacket.End();
			pHandler->Send(&ploginPacket);
		}
	}
}

int	CGameServer::CreateNewUser(string& NickName, int & gameMid)
{
	gameMid = 0;
	bool ret = g_GetCDBOperator().CreateNewUser(NickName, gameMid);

	if (!ret)
	{
		return 0;
	}

	return gameMid;
}

// 用户登录信息
int CGameServer::Handle_Logon( TcpHandler * pHandler, NETInputPacket * pPacket, int gameMid, int isNewData, string iphoneMac, int isFb )
{
	if( pHandler->GetUserData() == NULL )
	{
		UserGameInfo_Struct_t UserGameInfo;
		if (!g_GetCDBOperator().GetUserGameInfo(gameMid, UserGameInfo))
		{
			SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
			return 0;
		}
		
 		uint32 mid = gameMid;
		string key = "123456789";
		UserGameInfo.platform = 1;
		int ishaveemail = pPacket->ReadShort();
		string email = "email";
		if (ishaveemail != 0)
		{
			email = pPacket->ReadString();
		}

		if (isFb)
		{
			UserGameInfo.nickName = pPacket->ReadString();
			UserGameInfo.password = pPacket->ReadString();
		}

		log_debug(" CGameServer::Logon. UUID:=%d, KEY:=%s. name:%s\n", mid, key.c_str(), UserGameInfo.nickName.c_str());

		// 验证key
		if (m_cSecretKey != key)
		{
			string mKey;
			if (g_GetCDBOperator().getUserKey(mid, mKey) == false)
			{
				SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
				return 0; // 验证失败,不存在
			}

			if (mKey != key)
			{
				log_error("CGameServer::Handle_Logon. CHECK KEY FAILED. MKEY:=%s. UKEY:=%s. UUID:=%d.", mKey.c_str(), key.c_str(), mid);
				SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
				return 0; // 验证失败,不一样
			}
		}
		
		CPlayer * pPlayer = addPlayer( mid, pHandler );
		if( pPlayer != NULL )
		{
			pHandler->SetUserData( pPlayer );
			pPlayer->SetUserInfo(UserGameInfo);
		}
        else
        {
			log_error("CGameServer::Handle_Logon. INIT PLAYER FAILED. UUID:=%d.", mid);
			SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
            return 0;
        }

		if (isNewData == 1)
		{
			//TODO copy iphone data
		}

		OnLogon( pPlayer, ishaveemail, email, isFb);	
	}
    else
    {
		CPlayer * pPlayer = reinterpret_cast<CPlayer *>( pHandler->GetUserData() ); 
		log_debug(" CGameServer::ELSE_Logon. UUID:=%d.\n",pPlayer->getID());
		OnLogon( pPlayer, 0, "email\n", isFb);
	}

    return 0;
}

// 将登录的用户信息加入到在线好友列表里
CPlayer * CGameServer::addPlayer( const uint32 uuid, TcpHandler * pSession )
{
	CPlayer * pUser = NULL;
    CPlayer *pOldUser = g_GetPlayerMgr().GetPlayer(uuid);
    if(pOldUser != NULL)
    {
        log_debug("KickPlayer userID :%d \n",pOldUser->getID());
		// TODO:如果逻辑服务器与大厅掉线了，大厅要保存掉线的人,连接成功后发送消息，如果逻辑服务器挂掉，则通知大厅把所有的用户都断开连接
        //KickPlayer(pOldUser);
		return pOldUser;
    }

	pUser = new CPlayer( uuid, pSession,this );
    bool Ret = g_GetPlayerMgr().AddPlayer(pUser);
    assert(Ret);
	return pUser;
}

// 踢出用户
bool CGameServer::KickPlayer(CPlayer *pPlayer)
{
    SocketHandler* pHandler = (SocketHandler*)pPlayer->GetSocketHandler();
    if( pHandler != NULL )
    {
        log_debug(" KICK = %u.\n", pPlayer->getID() );
        NETOutputPacket packet;
        packet.Begin(SERVER_COMMAND_SYSTEM_KICK);
        packet.WriteInt(0);
        packet.End();

        pHandler->Send(&packet); 
        DisConnect( pHandler );       
    }

    return true;
}

// 发送信息到中心服务器
void  CGameServer::SendMsg2Center(NETOutputPacket *pPacket)
{
    m_pCentServer->SendMsg(pPacket);
}
