#ifndef BOYAA_SOCKET_HANDLER_H_20110312
#define BOYAA_SOCKET_HANDLER_H_20110312

#ifdef WIN32
	#include <WinSock2.h>
#endif

#define RECV_BUFFER 4096

#include "TimerEvent.h"
#include "TimerOutEvent.h"
#include <map>
#include "LoopBuffer.h"

using namespace std;
class TcpServer;
class NETInputPacket;
class NETOutputPacket;

const int MAX_LOOP_BUFFER_LEN = 32*1024;//byte

#define  RECV_BUFFER_SIZE (1024*8)		//8k的缓冲区
#define  SEND_BUFFER_SIZE (1024*3)		//3k的缓冲区


class TcpHandler : public TimerOutEvent
{
public:
	TcpHandler();	
	virtual ~TcpHandler();	

	void	    SetFd(int sock_fd);
	int		    GetFd()const;

    uint32      get_fd_index(){ return m_fd_index;}
    void        set_fd_index(uint32 index){ m_fd_index = index;}
  
    bool        GetNeedDel(){ return m_bNeedDel; }
    void        SetNeedDel(bool bDel){ m_bNeedDel = bDel; }
public:
	int		    handle_OnConnected();
	int		    handle_read();
	int		    handle_output();
	int		    handle_close();
	virtual void *GetUserData(){return NULL;}	
	virtual void SetUserData(void *pUserData){}

protected:
	// 自定义Hook 函数
	virtual int OnClose(void)					{return -1;}	// 连接断开后调用
	virtual int OnConnected(void)				{return 0;}	    // 连接成功建立后调用
	virtual int OnParser(char *, int)			{return -1;}    // 需解析数据包时调用
	virtual int	ProcessOnTimerOut(int Timerid)	{return 0;};	

public:	
	// 发送数据	
	int		    Send(const char *buf, int nLen);
    //packet
    virtual int OnPacketComplete(NETInputPacket *)=0;
	virtual int Send(NETOutputPacket *pPacket){return 0;}

    bool        Writable();

	// 获取及设置server对象指针
	TcpServer * server(void){return m_pServer;}
	virtual void server(TcpServer *p){m_pServer = p;}
protected:
	int		        m_sock_fd;
    uint32          m_fd_index;             
	uint32	        m_SocketType;		    //  
    bool            m_bNeedDel;             // 是否需要delete
    bool            m_bfull;                // 缓冲区满是否已爆
	TimerEvent      m_TcpTimer;
	TcpServer *     m_pServer;
	char	        m_pRecvBuffer[RECV_BUFFER_SIZE];	
	CLoopBuffer*    m_pSendLoopBuffer;
	char	        m_pTmpSendBuffer[SEND_BUFFER_SIZE];
};

#endif  


