
#include "PlayerMgr.h"
#include "Player.h"
#include "PacketBase.h"
#include "GameCmd.h"
#include "DBOperator.h"
#include "ConfigMgr.h"

PlayerMgr::PlayerMgr()
{
	m_pServer = NULL;
}
PlayerMgr::~PlayerMgr()
{
}
bool PlayerMgr::Init(CGameServer *pServer)
{
	m_pServer = pServer;
	return true;
}
void PlayerMgr::ShutDown()
{   
	MAP_PLAY::iterator It = m_players.begin();
	for(;It != m_players.end();++It)
	{
		DelPlayer(It->first);
		delete It->second;
	}
	m_players.clear();
}

// 是否存在此用户
bool  PlayerMgr::IsExist(uint32 UserID)
{
	MAP_PLAY::iterator It = m_players.find(UserID);
	if(It == m_players.end())
		return false;
	return true;    
}

// 获取用户
CPlayer* PlayerMgr::GetPlayer(uint32 UserID)
{
	MAP_PLAY::iterator It = m_players.find(UserID);
	if(It != m_players.end())
		return It->second;
	return NULL; 
}

// 添加用户
bool  PlayerMgr::AddPlayer(CPlayer *pPlayer )
{
	if(IsExist(pPlayer->getID()))
	{
		assert(false);
		log_debug("user exit");
		return false;
	}
	m_players.insert(MAP_PLAY::value_type(pPlayer->getID(),pPlayer));
	SetUserServerID(pPlayer->getID(),g_GetConfigMgr().getServerID());
	return true;
}

// 删除用户
bool  PlayerMgr::DelPlayer( uint32 UserID )
{
	log_debug("delete user %d\n", UserID);
	MAP_PLAY::iterator It = m_players.find(UserID);
	if(It != m_players.end())
	{    
		SetUserServerID( UserID,0 );
		SetUserState( UserID,0);
		m_players.erase(It);
		return true;
	}
	return false; 
}
// 发送信息给全部客户端
void  PlayerMgr::SendMsgToAll(NETOutputPacket * outPacket)
{
	log_debug("m_players.size():=%ld.", m_players.size());
	for(MAP_PLAY::iterator It = m_players.begin();It != m_players.end();++It)
	{
		log_debug("PlayerMgr::SendMsgToAll, cmd:=%d, TO:=%d.", outPacket->GetCmdType(), It->first);
		It->second->SendMsg(outPacket);
	}
}

// 发送消息给某个用户
bool  PlayerMgr::SendMsgToUser(NETOutputPacket * outPacket,uint32 UserID , bool rout /*= false*/)
{
	CPlayer * pPlayer = GetPlayer(UserID); 
	if(pPlayer != NULL)
	{
		pPlayer->SendMsg(outPacket);
		return true;
	}

	if (rout == true)
	{
		// 路由
		const uint32 nServerID = GetUserServerID(UserID);
		if( nServerID == 0 )
		{
			return false;
		}

		NETOutputPacket CentPacket;
		CentPacket.Begin(CHATSERVER_ROUTE);
		CentPacket.WriteInt(nServerID);
		CentPacket.WriteInt(CLIENT_COMMAND_CHAT_TO_FRIEND);
		CentPacket.WriteUInt(UserID);
		CentPacket.WriteBinary(outPacket->packet_buf(),outPacket->packet_size());
		CentPacket.End();
		
		m_pServer->SendMsg2Center(&CentPacket);
	}

	return true;
}

// 获得一个玩家的聊天服务器id
uint32  PlayerMgr::GetUserServerID(uint32 UserID)
{
	if(m_players.find(UserID) != m_players.end())
	{
		return g_GetConfigMgr().getServerID();
	}
	return g_GetCDBOperator().GetUserServerID(UserID);
}

void  PlayerMgr::SetUserServerID( uint32 UserID ,uint32 ServerID )
{
	g_GetCDBOperator().SetUserServerID( UserID,ServerID );
}

// 获得一个玩家的状态
uint32  PlayerMgr::GetUserState(uint32 UserID)
{
	if(m_players.find(UserID) != m_players.end()) 
		return m_players[UserID]->getStatus();

	return g_GetCDBOperator().GetUserState(UserID);
}

void   PlayerMgr::SetUserState( uint32 UserID,uint32 nState)
{
	//	TODO : record in redis
}

// 更新负载
void   PlayerMgr::UpdateStress2DB( bool bReset /*= false*/ )
{
	g_GetCDBOperator().SetServerStress(m_players.size());
}





