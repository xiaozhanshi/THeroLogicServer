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

    // �Ƿ���ڴ��û�
    bool  IsExist(uint32 UserID); 
    // ��ȡ�û�
    CPlayer* GetPlayer(uint32 UserID);
    // ����û�
    bool   AddPlayer(CPlayer *pPlayer );
    // ɾ���û�
    bool   DelPlayer(uint32 UserID);
    // ������Ϣ��ȫ���ͻ���
    void   SendMsgToAll(NETOutputPacket * outPacket);
    // ������Ϣ��ĳ���û�. rout �Ƿ�����·�ɵ���ķ��������������ᵼ���㲥�籩��
    bool    SendMsgToUser(NETOutputPacket * outPacket,uint32 UserID , bool rout = false);
    // ����������
    uint32  GetPlayerNum(){ return m_players.size(); }

public:
    // ���һ����ҵ����������id
    uint32  GetUserServerID( uint32 UserID );
    void    SetUserServerID( uint32 UserID ,uint32 ServerID );
    // ���һ����ҵ�״̬
    uint32  GetUserState( uint32 UserID );
    void    SetUserState( uint32 UserID,uint32 nState);
    // ���¸���
    void    UpdateStress2DB( bool bReset = false );

protected:
    typedef map<uint32, CPlayer *>  MAP_PLAY;

    MAP_PLAY        m_players;
    CGameServer*    m_pServer;
};

// ��ȡ��ɫ�������
#ifndef  g_GetPlayerMgr
#define  g_GetPlayerMgr()   (PlayerMgr::Instance())
#endif




















#endif  //PLAYERMGR_H__20110602


