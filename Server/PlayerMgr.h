#ifndef     PLAYERMGR_H__20110602
#define     PLAYERMGR_H__20110602

#include <map>
#include <string>
#include <vector>
#include "SingletonClass.h"
#include "define.h"
#include "GameServer.h"

class CPlayer;
class NETOutputPacket;
class CGameServer;

using namespace std;

class PlayerMgr : public AutoDeleteSingleton < PlayerMgr >
{
    friend  class AutoDeleteSingleton < PlayerMgr >; 
public:
    PlayerMgr(void);
    virtual ~PlayerMgr(void);
    bool    Init(CGameServer *pServer);
    void    ShutDown();

    // 是否存在此用户
    bool  IsExist(uint32 UserID); 
    // 获取用户
    CPlayer* GetPlayer(uint32 UserID);
    // 添加用户
    bool   AddPlayer(CPlayer *pPlayer );
    // 删除用户
    bool   DelPlayer(uint32 UserID);
    // 发送信息给全部客户端
    void   SendMsgToAll(NETOutputPacket * outPacket);
    // 发送消息给某个用户. rout 是否允许路由到别的服务器（处理不当会导至广播风暴）
    bool    SendMsgToUser(NETOutputPacket * outPacket,uint32 UserID , bool rout = false);
    // 获得玩家数量
    uint32  GetPlayerNum(){ return m_players.size(); }

public:
    // 获得一个玩家的聊天服务器id
    uint32  GetUserServerID( uint32 UserID );
    void    SetUserServerID( uint32 UserID ,uint32 ServerID );
    // 获得一个玩家的状态
    uint32  GetUserState( uint32 UserID );
    void    SetUserState( uint32 UserID,uint32 nState);
    // 更新负载
    void    UpdateStress2DB( bool bReset = false );

protected:
    typedef map<uint32, CPlayer *>  MAP_PLAY;

    MAP_PLAY        m_players;
    CGameServer*    m_pServer;
};

// 获取角色管理对象
#ifndef  g_GetPlayerMgr
#define  g_GetPlayerMgr()   (PlayerMgr::Instance())
#endif




















#endif  //PLAYERMGR_H__20110602


