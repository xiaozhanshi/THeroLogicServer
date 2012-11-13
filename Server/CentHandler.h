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
	void * m_pUserData;		        //�û�����
}ProxyList;

class CentHandler : public TcpHandler				
{
public:
    CentHandler();
    virtual ~CentHandler(void);
public:
    //����״̬
    int     GetChunkStatus(void){return m_nStatus;}
    // ��������
    void    SetHandlerType(int type ){ m_HandlerType = type;}
    int     GetHandlerType(){ return m_HandlerType; }
    //���ص��ʮ���Ƶ�IP��ַ
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
    // Э�����
    virtual int OnParser(char *buf, int nLen);
    //���ӹر�
    virtual int OnClose(void);
    //���ӽ���
    virtual int OnConnected(void);
    virtual int	ProcessOnTimerOut(int Timerid);

    //��ȡԶ�˵�ַ
    void GetRemoteAddr(void);
private:
    int    m_nStatus;				//Handler״̬
    string m_addrremote;			//Զ�˵�ַ
    int    m_nPort;					//�˿�	
    int    m_HandlerType;           // handler����

    IPacketParser * m_pParser;
    CentServer*		m_pServer;
	int		m_proxyId;
};

#endif // CENTHANDLER_H__
