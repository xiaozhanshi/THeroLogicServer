/*
    与大厅的连接处理，有proxy，和非proxy
*/
#ifndef CENTSERVER_H__20110618
#define CENTSERVER_H__20110618

#include <map>
#include <string>
#include <vector>
#include "SingletonClass.h"
#include "define.h"
#include "Connector.h"
#include "SockerAPI.h"
#include "CentHandler.h"
#include "TimerObj.h"
class CGameServer;

class CentServer
{
public:
    CentServer(CGameServer* pServer);
    virtual ~CentServer();

	bool	InitConnect(NetAddr &addr);    //初始化连接
	bool    Reconnect(void);			   //关闭自动重连
	bool    IsConnected(void);             //返回连接状态
    void    SendMsg(NETOutputPacket *pPacket);
    int     ProcessPacket(short nMsgType, NETInputPacket *pPacket, CentHandler *pHandler);
    void    OnConnect(CentHandler *pHandler );
    void    OnDisconnect(CentHandler *pHandler );
	int		OnProxyConnect(int proxyid);
	void	OnProxyDisConnect(int proxyid);
	void	ShutDown();

protected:
    void    ForwardPacket2All( NETInputPacket *pPacket,CentHandler *pHandler );
    void    HandleUserMsg( NETInputPacket *pPacket,CentHandler *pHandler );
    void    HandleSystemMsg( NETInputPacket *pPacket,CentHandler *pHandler );
    void	HandleSYN(NETInputPacket *pPacket,CentHandler *pHandler);
	void    HandleProxyLogin( NETInputPacket *pPacket,CentHandler *pHandler );
	void    HandleFBProxyLogin( NETInputPacket *pPacket,CentHandler *pHandler );
	void    HandleProxyLogout( NETInputPacket *pPacket,CentHandler *pHandler );
	void    HandleAfterRegistProxyLogin( NETInputPacket *pPacket,CentHandler *pHandler );

public:   
	bool    ConnectToMonitor();    // 连接到监控服务器
public:
	map<int, void*>		m_proxyList;

private:
    bool            m_bIsConnect;
    NetAddr         m_addr;
    Connector	    m_CentConnector;
    CentHandler     m_CentHandler;
    Connector       m_MonitorConnector;
    CentHandler     m_MonitorHandler;
    CGameServer*    m_pGameServer;
};

#endif  // CENTSERVER_H__20110618



