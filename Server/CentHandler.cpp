
#include "CentHandler.h"
#include "SocketServer.h"
#include <time.h>
#include "EncryptDecrypt.h"
#include "CentServer.h"
#include "SockerAPI.h"

namespace
{
    static const string s_policystr ="<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>\0" ;// flash文件请求的固定回复
    static const int    s_ReconnectTime = 3;
}

CentHandler::CentHandler()
:TcpHandler()
{	
    m_HandlerType = 0;
    m_nStatus = -1;
    m_addrremote = "";
    m_nPort = 0;
    m_pParser = NULL;
    m_pServer = NULL;
	m_proxyId = -1;
}

CentHandler::~CentHandler(void)
{
    if (m_pParser != NULL)
    {
        delete m_pParser;
        m_pParser = NULL;
    }
}

int CentHandler::Send(NETOutputPacket *pPacket)
{
    CEncryptDecrypt::EncryptBuffer(*pPacket);
	short nMsgType = pPacket->GetCmdType();
	if (nMsgType > PROXYFENCE || (nMsgType <= PROXYFENCE1 && nMsgType >= PROXYFENCE2))
	{
		// wrap proxy packet
		pPacket->SetProxyId(m_proxyId);
	}
    return TcpHandler::Send(pPacket->packet_buf(), pPacket->packet_size());
}

// OnParser 协议解析
int CentHandler::OnParser(char *buf, int nLen)
{
    m_nStatus = REQUEST;
    m_TcpTimer.StopTimer();	//15内有请求取消定时
    if(nLen == 23 && buf[0] == '<' && buf[1] == 'p')
    {
        string policy = "<policy-file-request/>";
        for(int i=0; i<23; ++i)
        {
            if(buf[i] != policy[i])
            {
                log_debug("falsh fail ！\n");
                return -1;
            }
        }
        TcpHandler::Send(s_policystr.c_str(), (int)s_policystr.size());
        return -1;
    }
    if(m_pParser == NULL)
        m_pParser = IPacketParser::CreateObject(this);

    return m_pParser->ParseProxyPacket(buf, nLen);
}

// 解析完成
int CentHandler::OnPacketComplete(NETInputPacket *pPacket)
{
    if(CEncryptDecrypt::DecryptBuffer(*pPacket) == -1)
    {
        log_debug("Decrypt fail \n");
        return -1;
    }
    
	short nMsgType = pPacket->GetCmdType();
	// set proxyid
	SetProxy(nMsgType, pPacket);
    return m_pServer->ProcessPacket(nMsgType, pPacket, this);
}
// OnClose
int CentHandler::OnClose(void)
{
    m_nStatus = CLOSE;	
    m_TcpTimer.StartTimer(s_ReconnectTime);

    m_pServer->OnDisconnect(this);
    return 0;
}
// OnConnected
int CentHandler::OnConnected(void)
{
    m_nStatus = CONNECT;
    GetRemoteAddr();

    m_TcpTimer.StopTimer();
    m_pServer->OnConnect(this);
    return 0;
}
int	CentHandler::ProcessOnTimerOut(int Timerid)
{
    if( m_HandlerType == emCENTER_HANDLE )
    {
        if(!m_pServer->Reconnect())
        {
            m_TcpTimer.StartTimer(s_ReconnectTime);
        }
    }
    else if( m_HandlerType == emMONITER_HANDLE )
    {
        if(!m_pServer->ConnectToMonitor())
        {
            m_TcpTimer.StartTimer(s_ReconnectTime);
        }
    }
    return 0;
}

void CentHandler::GetRemoteAddr(void)
{
    sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    int len = sizeof(remote_addr);
    if(getpeername(GetFd(), reinterpret_cast<sockaddr *> (&remote_addr), (socklen_t*)&len) == 0)
    {
        m_addrremote = inet_ntoa(remote_addr.sin_addr);
        m_nPort = ntohs(remote_addr.sin_port);
    }
}

void CentHandler::SetProxy(short cmd, NETInputPacket *pPacket)
{
	m_proxyId = -1;
	if (cmd > PROXYFENCE || (cmd <= PROXYFENCE1 && cmd >= PROXYFENCE2))
	{
		m_proxyId = pPacket->GetProxyId();
	}
}

void *CentHandler::GetUserData()
{
	log_debug("get the proxy id is %d\n", m_proxyId);
	map<int, void*>::iterator iter = m_pServer->m_proxyList.find(m_proxyId);

	if(iter != m_pServer->m_proxyList.end())
	{
		return iter->second;
	}
	return NULL;
}

void CentHandler::SetUserData(void *pUserData)
{
	log_debug("set the proxy id is %d\n", m_proxyId);
	map<int, void*>::iterator iter = m_pServer->m_proxyList.find(m_proxyId);

	if(iter != m_pServer->m_proxyList.end())
	{
		iter->second = pUserData;
	}

	//if (m_proxyId != 0)
	//{
	//	m_pServer->m_proxyList[m_proxyId] = pUserData;
	//}
}