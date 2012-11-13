#pragma once

#include "TcpHandler.h"
#include "PacketBase.h"
#include "PacketParser.h"
#include "define.h"
#include <map>
using namespace std;
/*
    �и�ǿ��չ������ģ���Onconnect,Onclose,OnPacket�Ļص����������óɺ���ָ�룬���Զ�̬��
    ����ASIO 
*/
class SocketHandler:public TcpHandler				
{
public:
	explicit SocketHandler(int nID);
	virtual ~SocketHandler(void);
public:
	//����״̬
	int GetChunkStatus(void){return m_nStatus;}

	int GetHandlerID(void){return m_nHandlerID;}

	//���ص��ʮ���Ƶ�IP��ַ
	string  GetAddr(void){return m_addrremote;}		
	
	virtual void *  GetUserData(){return m_pUserData;}	
	virtual void    SetUserData(void *pUserData){	m_pUserData = pUserData;}
public:
	virtual int Send(NETOutputPacket *pPacket);
    // packet
    virtual int OnPacketComplete(NETInputPacket *);

private:
	// Э�����
	virtual int OnParser(char *buf, int nLen);
	// ���ӹر�
	virtual int OnClose(void);
	// ���ӽ���
	virtual int OnConnected(void);
    virtual int	ProcessOnTimerOut(int Timerid);

    // ��ȡԶ�˵�ַ
	void GetRemoteAddr(void);
private:
	int    m_nStatus;				//Handler״̬
	int    m_nHandlerID;			//socket���ID
	string m_addrremote;			//Զ�˵�ַ
	int    m_nPort;					//�˿�	
	void * m_pUserData;		        //�û�����
    IPacketParser * m_pParser;
};

