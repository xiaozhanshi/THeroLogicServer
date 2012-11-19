#include "CentServer.h"
#include "GameServer.h"
#include "ConfigMgr.h"
#include "GameCmd.h"
#include "PlayerMgr.h"
#include "PacketBase.h"
#include "Player.h"
#include "DBOperator.h"

CentServer::CentServer(CGameServer* pServer)
:m_pGameServer(pServer)
{
	m_bIsConnect = false;
	m_CentHandler.SetCentServer(this);
	m_MonitorHandler.SetCentServer(this);

	m_CentHandler.SetHandlerType(emCENTER_HANDLE);
	m_MonitorHandler.SetHandlerType(emMONITER_HANDLE);
}

CentServer::~CentServer()
{

}

bool CentServer::InitConnect(NetAddr &addr)
{
	if(m_pGameServer == NULL)
		return false;
	m_CentConnector.Open(m_pGameServer);
	CentHandler *pHandler = &m_CentHandler;
	m_addr  = addr;
	bool Ret =  m_CentConnector.Connect(pHandler, addr);
	if(!Ret)
	{
		log_error("Can not con to Center");
		m_CentHandler.Reconnection();
	}

	//Ret = ConnectToMonitor();

	//if(!Ret)
	//{
	//	log_error("Can not con to Monitor");
	//	return false;
	//}

	return true;
}

// �ر��Զ�����
bool CentServer::Reconnect(void)  
{
	if(IsConnected())
		return false;

	CentHandler *pClient = &m_CentHandler;
	return m_CentConnector.Connect(pClient, m_addr);
}

//��������״̬
bool CentServer::IsConnected(void)
{
	return m_bIsConnect;
}
void CentServer::SendMsg(NETOutputPacket *pPacket)
{
	if(m_bIsConnect)
		m_CentHandler.Send(pPacket);
}

int CentServer::ProcessPacket(short nMsgType, NETInputPacket *pPacket, CentHandler *pHandler)
{
	switch( nMsgType )
	{      
	case HERO_COMMAND_IPHONE_LOGIN:
		m_pGameServer->Handle_Iphone_Login(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_USER_REGIST:
		m_pGameServer->Handle_Regist_User(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_PROFROM_USER_LOGIN:
		m_pGameServer->Handle_User_Login(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_CHNANGE_USER_PASSWORD:
		m_pGameServer->Handle_User_ChangePassWord(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_FB_USER_LOGIN:
		m_pGameServer->Handle_FB_Login(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_CHNANGE_USER_MAIL:
		m_pGameServer->Handle_User_ChangeMail(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_FIND_USER_PASSWORD:
		m_pGameServer->Handle_User_FindPassWord(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_FB_USER_BIND:
		m_pGameServer->Handle_User_FBBINGDING(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_USER_CHANGE_HEADER:
		m_pGameServer->Handle_User_Change_Header_Type(pHandler, pPacket, pHandler->GetProxy());
		break;
	case SERVER_CMD_SYNC:
		HandleSYN(pPacket,pHandler);						// �������������
		break;
	case SERVER_COMMAND_SEND_SYS_MESSAGE:                   // ϵͳ��Ϣ
		HandleSystemMsg(pPacket,pHandler);
		break; 
	case COMMAND_USER_LOGIN:
		HandleProxyLogin(pPacket, pHandler);
		break;
	case COMMAND_USER_FBLOGIN:
		HandleFBProxyLogin(pPacket, pHandler);
		break;
	case COMMAND_USER_REGISTLOGIN:
		HandleAfterRegistProxyLogin(pPacket, pHandler);
		break;
	case COMMAND_USER_LOGOUT: 
		HandleProxyLogout(pPacket, pHandler);
		break;
	case COMMAND_WANT_ADD_FRIEND:
		m_pGameServer->Handle_User_Add_FriendMessage(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_CHANGE_MESSAGE_STATE:
		m_pGameServer->Handle_User_Change_Message_State(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_ACCEPT_ADD_FRIEND:
		m_pGameServer->Handle_User_Add_FriendAccept(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_DELETE_FRIEND:
		m_pGameServer->Handle_User_Delete_Friend(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_USER_GETFRIEND:
		m_pGameServer->Handle_User_Get_Friend_List(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_GET_USER_MESSAGE:
		m_pGameServer->Handle_User_Get_Message_List(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_GET_SYSTEM_MESSAGE:
		m_pGameServer->Handle_User_System_Message_List(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_GET_MOREGAMELIST_MESSAGE:
		m_pGameServer->Handle_User_Get_More_Game_List(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_SEND_MESSAGE_TO_SERVER:
		m_pGameServer->Handle_User_Send_Message_To_APN(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_GET_ENLARGE_TABLE:
		m_pGameServer->Handle_User_Get_Enlarge_Table(pHandler, pPacket, pHandler->GetProxy());
		break;
	case COMMAND_BUY_ENLARGE_TABLE:
		m_pGameServer->Handle_User_Buy_Enlarge_Table(pHandler, pPacket, pHandler->GetProxy());
		break;


	/////////////////////////////////////////////mechine relate/////////////////////////////////////////////////////////////////
	case COMMAND_GET_GOODS_PCATE:
		m_pGameServer->Handle_User_Get_Production(pHandler, pPacket, pHandler->GetProxy());					// ��ȡ��Ʒ�б�
		break;
	case COMMAND_BUY_GOODS_PCATE:
		m_pGameServer->Handle_User_Buy_Production(pHandler, pPacket, pHandler->GetProxy());					// ������Ʒ�б�
		break;
	case COMMAND_EDIT_GOODS_PID:
		m_pGameServer->Handle_User_Edit_Production(pHandler, pPacket, pHandler->GetProxy());				// �༭��Ʒ
		break;


	case COMMAND_MECHINE_UPLEVEL_REQUIRE:
		m_pGameServer->Handle_User_Require_Mechine_UpLevel(pHandler, pPacket, pHandler->GetProxy());		// ���������������
		break;
	case COMMAND_MECHINE_UPNEXTLEVEL_REQUIRE:
		m_pGameServer->Handle_User_Require_Mechine_UpNextLevel(pHandler, pPacket, pHandler->GetProxy());	// ��������
		break;
	case COMMAND_MECHINE_REQUIRE_MECHINE_STATUS:
		m_pGameServer->Handle_User_Require_Mechine_Status(pHandler, pPacket, pHandler->GetProxy());			// �������״̬
		break;
	case COMMAND_MECHINE_REQUIRE_SPEED:
		m_pGameServer->Handle_User_Require_Mechine_Speed(pHandler, pPacket, pHandler->GetProxy());			// �������
		break;
	case COMMAND_MECHINE_REQUIRE_COMFOR:
		m_pGameServer->Handle_User_Require_Mechine_Comfor(pHandler, pPacket, pHandler->GetProxy());			// �������ȷ��
		break;

	case COMMAND_MECHINE_COLLECT_INFO_REQURITE:
		m_pGameServer->Handle_User_Info_Mechine_CollectCoin(pHandler, pPacket, pHandler->GetProxy());		// �鱨���ռ����
		break;
	case COMMAND_MECHINE_REQUIRE_INFO_COLLECT_SPEED:
		m_pGameServer->Handle_User_Info_Mechine_CollectSpeed(pHandler, pPacket, pHandler->GetProxy());		// �鱨���ռ�����
		break;
	case COMMAND_GET_PROPSSYNTHESIS_CONDITION:															   
		m_pGameServer->Handle_Get_PropsSynthesis_Condition(pHandler, pPacket, pHandler->GetProxy());       //  ��ȡ���ߺϳ�����
		break;
	case COMMAND_GET_RESOURCEPRODUC_CONDITION:
		m_pGameServer->Handle_Get_ResourceProduc_Condition(pHandler, pPacket, pHandler->GetProxy());       //  ��ȡ��Դ����������
		break;
	case COMMAND_MECHINE_RESOURCEPRODUC:
		m_pGameServer->Handle_Mechine_ResourceProduc(pHandler, pPacket, pHandler->GetProxy());             //   ��Դ������
		break;
	case COMMAND_MECHINE_RESOURCE_COLLECT:
		m_pGameServer->Handdle_Mechine_ResourceCollect(pHandler, pPacket, pHandler->GetProxy());           //   ��Դ�ռ�����
		break;
	/////////////////////////////////////////////mechine relate/////////////////////////////////////////////////////////////////

	case CENTER_USER_MSG:                                   // ���ķ�������ָ���û�����Ϣ
		HandleUserMsg(pPacket,pHandler);        
		break;
	default:
		log_error("receive unknow cmd %d\n",nMsgType);
		break;
	}
	return 0;
}
void CentServer::OnConnect(CentHandler *pHandler )
{
	log_debug("OnConnect with hall\n");
	// send the regist info
	ServerConfig_T cfg = g_GetConfigMgr().getServerConfig();
	uint32 nGroupID  = g_GetConfigMgr().getGroupID();
	uint32 nType = g_GetConfigMgr().getServerType();
	uint32 nServerID = g_GetConfigMgr().getServerID();

	if( pHandler->GetHandlerType() == emCENTER_HANDLE )
	{
		m_bIsConnect = true;
		log_debug("CentServer OnConnect \n");
		NETOutputPacket packet;
		packet.Begin( SERVER_TO_CENTSERVER_REGIST );
		packet.WriteShort( cfg.id );
		packet.WriteShort( cfg.type );
		packet.WriteString( cfg.host.c_str() );
		packet.WriteInt( cfg.port );
		packet.End();
		pHandler->Send(&packet);
		log_debug("Send Resgist info %d \n",cfg.id);
	}
	else if ( pHandler->GetHandlerType() == emMONITER_HANDLE )
	{
		log_debug( " CGameMgr::OnMonitorConnect. xMonitor. \n" );
		NETOutputPacket packet;
		packet.Begin(Monitor::REGISTER_SERVER_REQUEST);
		packet.WriteInt(nServerID);
		packet.WriteInt(nType);
		packet.WriteInt(nGroupID);
		packet.End();
		pHandler->Send(&packet);
	}
}

void CentServer::OnDisconnect(CentHandler *pHandler )
{
	// delete all user
	log_debug("delete all player here\n");
	g_GetPlayerMgr().ShutDown();
	ShutDown();

	if( pHandler->GetHandlerType() == emCENTER_HANDLE )
	{
		m_bIsConnect = false;
		log_error("CentServer OnDisConnect , reconnect in 3s\n");
	}
	else if ( pHandler->GetHandlerType() == emMONITER_HANDLE )
	{
		log_error("Monitor Disconnect \n");
	}
}

void CentServer::ForwardPacket2All(NETInputPacket *pPacket,CentHandler *pHandler)
{
	log_debug("CentServer::ForwardPacket2All. double-exp time begin.");

	NETOutputPacket outPacket;
	outPacket.Copy(pPacket->packet_buf(),pPacket->packet_size());
	outPacket.End();

	g_GetPlayerMgr().SendMsgToAll(&outPacket);
}

void CentServer::HandleUserMsg(NETInputPacket *pPacket,CentHandler *pHandler)
{
	log_debug("HandleUserMsg \n");
	uint32 UserID = pPacket->ReadInt();
	CPlayer *pPlayer = g_GetPlayerMgr().GetPlayer(UserID);
	if( pPlayer == NULL )
	{
		log_debug("HandleUserMsg user is null \n");
		return;
	}
	NETOutputPacket outPacket;
	int pos = 9 + sizeof(int);
	int len = pPacket->packet_size() - pos;
	outPacket.Copy(pPacket->packet_buf()+pos,len);
	outPacket.End();

	log_debug("SendMsg is %d \n",outPacket.GetCmdType());
	pPlayer->SendMsg(&outPacket);

}

void CentServer::HandleSystemMsg(NETInputPacket *pPacket,CentHandler *pHandler)
{
	log_debug("recv SERVER_COMMAND_SEND_SYS_MESSAGE \n");
	ForwardPacket2All(pPacket,pHandler);
}

// ���ӵ���ط�����
bool CentServer::ConnectToMonitor()
{
	if(g_GetConfigMgr().getMonitorInfo().enable == true)
	{
		m_MonitorConnector.Open(m_pGameServer);
		CentHandler *pHandler = &m_MonitorHandler;

		bool ret = m_CentConnector.Connect(pHandler, g_GetConfigMgr().getMonitorInfo().host,g_GetConfigMgr().getMonitorInfo().port);
		return ret;
	}
	return true;
}

void CentServer::HandleSYN(NETInputPacket *pPacket,CentHandler *pHandler)
{
	NETOutputPacket OutPkg;
	OutPkg.Begin(CLIENT_CMD_SYNC);
	OutPkg.End();
	pHandler->Send(&OutPkg);
}

int CentServer::OnProxyConnect(int proxyid)
{
	int found = 0;
	if (proxyid != 0)
	{
		if(m_proxyList.find(proxyid) == m_proxyList.end())
		{
			m_proxyList.insert(map<int, void*>::value_type(proxyid, NULL));
			found = 0;
		}
		else
		{
			log_debug("OnProxyConnect login again %d\r\n", proxyid);
			found = 1;
		}
	}

	return found;
}

void CentServer::OnProxyDisConnect(int proxyid)
{
	map<int, void*>::iterator iter = m_proxyList.find(proxyid);
	if(iter != m_proxyList.end())
	{
		m_proxyList.erase(iter);
	}
	else
	{
		log_debug("OnProxyDisConnect Error %d\r\n",proxyid);
	}
}

void CentServer::ShutDown()
{   
	map<int, void*>::iterator It = m_proxyList.begin();
	for(;It != m_proxyList.end();++It)
	{
		It->second = NULL;
	}
	m_proxyList.clear();
}

void CentServer::HandleProxyLogin( NETInputPacket *pPacket,CentHandler *pHandler )
{
	int flag = pPacket->ReadInt();	// 0 ����ɹ�
	if (flag != 0)
	{
		log_error("login packet flag != 0\n");
		m_pGameServer->SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
		return;
	}
	int ret = OnProxyConnect(pHandler->GetProxy());
	if (ret == 1)					// ���proxy�Ѿ���½���ˣ�Ҫ���˳�
	{
		m_pGameServer->ProcessClose(pHandler);
		OnProxyDisConnect(pHandler->GetProxy());
		int ret2 = OnProxyConnect(pHandler->GetProxy());
		if (ret2 == 1)
		{
			log_error("login again error\n");
			m_pGameServer->SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
			return;
		}
	}
	int gameMid = pPacket->ReadInt();
	log_debug("the flag is %d, receive the gamemid is %d\n", flag, gameMid);
	m_pGameServer->Handle_Logon( pHandler, pPacket, gameMid, 0, "123" );
}

void CentServer::HandleFBProxyLogin( NETInputPacket *pPacket,CentHandler *pHandler )
{
	int flag = pPacket->ReadInt();	// 0 ����ɹ�
	if (flag != 0)
	{
		log_error("login packet flag != 0\n");
		m_pGameServer->SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
		return;
	}
	int ret = OnProxyConnect(pHandler->GetProxy());
	if (ret == 1)					// ���proxy�Ѿ���½���ˣ�Ҫ���˳�
	{
		m_pGameServer->ProcessClose(pHandler);
		OnProxyDisConnect(pHandler->GetProxy());
		int ret2 = OnProxyConnect(pHandler->GetProxy());
		if (ret2 == 1)
		{
			log_error("login again error\n");
			m_pGameServer->SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
			return;
		}
	}
	int gameMid = pPacket->ReadInt();
	log_debug("the flag is %d, receive the gamemid is %d\n", flag, gameMid);
	m_pGameServer->Handle_Logon( pHandler, pPacket, gameMid, 0, "123", 1 );
}

void CentServer::HandleAfterRegistProxyLogin( NETInputPacket *pPacket,CentHandler *pHandler )
{
	int flag = pPacket->ReadInt();
	if (flag != 0)
	{
		log_error("login packet flag != 0\n");
		m_pGameServer->SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
		return;
	}
	int ret = OnProxyConnect(pHandler->GetProxy());
	if (ret == 1)					// ���proxy�Ѿ���½���ˣ�Ҫ���˳�
	{
		m_pGameServer->ProcessClose(pHandler);
		OnProxyDisConnect(pHandler->GetProxy());
		int ret2 = OnProxyConnect(pHandler->GetProxy());		// �ڶ������˳����ɹ�
		if (ret2 == 1)
		{
			log_error("login regist error\n");
			m_pGameServer->SendCommonFlagMsgReply( COMMAND_COMLOGIN_REPLY, 1, pHandler);
			return;
		}
	}
	int gameMid = pPacket->ReadInt();
	log_debug("the flag is %d, receive the gamemid is %d\n", flag, gameMid);
	int isNewData = pPacket->ReadShort();
	string iphoneMac;
	if (isNewData != 0)
	{
		iphoneMac = pPacket->ReadString();
	}
	m_pGameServer->Handle_Logon( pHandler, pPacket, gameMid, isNewData, iphoneMac );
}

void CentServer::HandleProxyLogout( NETInputPacket *pPacket,CentHandler *pHandler )
{
	log_debug("ENTER LOGOUT user logout %d\n", pHandler->GetProxy());
	m_pGameServer->ProcessClose(pHandler);		// ��Ϸ�߼��ر�
	OnProxyDisConnect(pHandler->GetProxy());	// socket proxy �ر�
	log_debug("LEAVE LOGOUT user logout %d\n", pHandler->GetProxy());
}