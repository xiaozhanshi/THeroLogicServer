#ifndef _ENCRYPTDECRYPT_H_
#define _ENCRYPTDECRYPT_H_

#include "define.h"
#include "PacketBase.h"

class CEncryptDecrypt
{
private:
	CEncryptDecrypt(void);
	CEncryptDecrypt(const CEncryptDecrypt &);
	CEncryptDecrypt operator=(const CEncryptDecrypt &);
	~CEncryptDecrypt();

public:
	static int  EncryptBuffer(NETOutputPacket & pPacket);	// ��������
	static int  DecryptBuffer(NETInputPacket & pPacket);	// ��������

private:
	static BYTE MapRecvByte(BYTE const cbData);
	static BYTE MapSendByte(BYTE const cbData); 
private:
	static const BYTE	m_SendByteMap[256];		//�ֽ�ӳ���
	static const BYTE	m_RecvByteMap[256];		//�ֽ�ӳ���
};

#endif

