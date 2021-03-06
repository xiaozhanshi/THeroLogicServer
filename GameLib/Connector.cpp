
#include "Connector.h"
#include "SockerAPI.h"

Connector::Connector()
{
	m_pNetServer = NULL;
}
Connector::~Connector()
{
}

bool  Connector::Open(TcpServer* pServer)
{
	if(pServer == NULL)
		return false;

	m_pNetServer = pServer;
	return true;
}

// 连接
bool  Connector::Connect(TcpHandler *pHandler, const string &strAddr,int port )
{
	int sock_fd = CSocker::SocketInit();
	if( pHandler == NULL || sock_fd < 0)
	{	
		return false;
	}
	//如果连接上
	if(CSocker::ClientConnect(sock_fd, strAddr.c_str(), port) == 0)
	{	
        CSocker::SetSocketMem(sock_fd,16*1024);
        if(CSocker::SocketNoBlock(sock_fd) < 0)
        {
            log_error("SetNonblock faild \n");
            CSocker::SocketClose(sock_fd);
            return false;
        }
        if(CSocker::SetTcpKeepLive(sock_fd) < 0)
        {
            log_error("SetTcpKeepLive faild \n");
            CSocker::SocketClose(sock_fd);
            return false;
        }
        pHandler->SetFd(sock_fd);
		return Register(pHandler);			
	}
	CSocker::SocketClose(sock_fd);	
	return false;
}
bool  Connector::Connect(TcpHandler *pHandler,const NetAddr &addr )
{
	return Connect(pHandler,addr.host,addr.port);
}
bool  Connector::Register(TcpHandler *pHandler)
{
	if(m_pNetServer == NULL)
		return false;
	return m_pNetServer->Register(pHandler);
}

