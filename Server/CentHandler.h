#ifndef CENTHANDLER_H__
#define CENTHANDLER_H__

#include "TcpHandler.h"
#include "PacketBase.h"
#include "PacketParser.h"
#include "define.h"
#include <map>
using namespace std;

class CentServer;

enum emCONNECT_TYPE
{
    emCENTER_HANDLE  = 100,
    emMONITER_HANDLE = 200,
};

#define PROXYFENCE 10000
#define PROXYFENCE1 5000
#define PROXYFENCE2 1000

typedef struct _tagProxyList
{
	int proxyid;
	void * m_pUserData;		        //用户数据
}ProxyList;

class CentHandler : public TcpHandler				
{
public:
    CentHandler();
    virtual ~CentHandler(void);
public:
    //返回状态
    int     GetChunkStatus(void){return m_nStatus;}
    // 设置类型
    void    SetHandlerType(int type ){ m_HandlerType = type;}
    int     GetHandlerType(){ return m_HandlerType; }
    //返回点分十进制的IP地址
    string  GetAddr(void){return m_addrremote;}		
    void    SetCentServer(CentServer* pServer){ m_pServer = pServer; }
	void	Reconnection(){OnClose();}
	void	SetProxy(short cmd, NETInputPacket *pPacket);
	int		GetProxy(){return m_proxyId;}

public:
    //packet
    virtual int OnPacketComplete(NETInputPacket *);
	virtual int Send(NETOutputPacket *pPacket);
	virtual void *GetUserData();
	virtual void SetUserData(void *pUserData);

private:
    // 协议解析
    virtual int OnParser(char *buf, int nLen);
    //连接关闭
    virtual int OnClose(void);
    //连接建立
    virtual int OnConnected(void);
    virtual int	ProcessOnTimerOut(int Timerid);

    //获取远端地址
    void GetRemoteAddr(void);
private:
    int    m_nStatus;				//Handler状态
    string m_addrremote;			//远端地址
    int    m_nPort;					//端口	
    int    m_HandlerType;           // handler类型

    IPacketParser * m_pParser;
    CentServer*		m_pServer;
	int		m_proxyId;
};

#endif // CENTHANDLER_H__
