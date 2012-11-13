#include "PacketParser.h"
#include "PacketBase.h"
#include "SocketHandler.h"

class CPacketParser :  public IPacketParser
{
public:
	CPacketParser(TcpHandler * pHandler):IPacketParser(pHandler)
	{
		m_pBuf = m_Packet.packet_buf();
		m_version = NETInputPacket::SERVER_PACKET_DEFAULT_VER;
		m_subVersion = NETInputPacket::SERVER_PACKET_DEFAULT_SUBVER;
		reset();
	}

	virtual ~CPacketParser(void){}

	// 处理PACKET数据
	int ParsePacket(const char *data, const size_t length)
	{
		int ret = -1; //出错返回
		size_t ndx = 0;
		while(ndx < length && m_nStatus != REQ_ERROR)//可能会同时来两个包 
		{
			switch(m_nStatus)
			{
			case REQ_REQUEST:
			case REQ_HEADER:
				if(!read_header(data, length, ndx))
					break;
				ret = parse_header();
				if(ret != 0)
				{
					m_nStatus = REQ_ERROR;
					break;
				}else
					m_nStatus = REQ_BODY;
			case REQ_BODY:
				if(parse_body(data, length, ndx))
					m_nStatus = REQ_DONE;
				break;
			default:
                {
                    log_debug("parse error state return -1 \n");
                    return -1;
                }
			}
			if(m_nStatus == REQ_ERROR)
            {
                log_debug("parse state is REQ_ERROR \n");
                return ret;
            }
			if(m_nStatus == REQ_DONE)
			{
				if(m_pHandler->OnPacketComplete(&m_Packet) == -1)
				{
                    log_debug("OnPacketComplete return -1 \n");
					return -1;
				}

				this->reset();
			}
		}
		return 0; // return 0 to continue
	}

	int ParseProxyPacket(const char *data, const size_t length)
	{
		int ret = -1; //出错返回
		size_t ndx = 0;
		while(ndx < length && m_nStatus != REQ_ERROR)//可能会同时来两个包 
		{
			switch(m_nStatus)
			{
			case REQ_REQUEST:
			case REQ_HEADER:
				if(!read_header(data, length, ndx))
					break;
				ret = parse_Proxy_header();
				if(ret != 0)
				{
					m_nStatus = REQ_ERROR;
					break;
				}else
					m_nStatus = REQ_BODY;
			case REQ_BODY:
				if(parse_body(data, length, ndx))
					m_nStatus = REQ_DONE;
				break;
			default:
				{
					log_debug("parse error state return -1 \n");
					return -1;
				}
			}
			if(m_nStatus == REQ_ERROR)
			{
				log_debug("parse state is REQ_ERROR \n");
				return ret;
			}
			if(m_nStatus == REQ_DONE)
			{
				if(m_pHandler->OnPacketComplete(&m_Packet) == -1)
				{
					log_debug("OnPacketComplete return -1 \n");
					return -1;
				}

				this->reset();
			}
		}
		return 0; // return 0 to continue
	}

protected:
	void reset(void)
	{
		m_nStatus = REQ_REQUEST;
		m_nPacketPos = 0;
		m_nBodyLen = 0;
		m_Packet.Reset();//包完成复位
	}

public:
	short m_version;
	short m_subVersion;

private:
	// 当前处理状态
	int m_nStatus; 
	// PacketPos
	size_t	m_nPacketPos;
	// BODY长度
	size_t m_nBodyLen;
	// PacketBuffer 指针
	char *m_pBuf;
	// PacketBuffer
	NETInputPacket m_Packet;
	// 状态
	enum REQSTATUS{	REQ_REQUEST=0, REQ_HEADER, REQ_BODY, REQ_DONE, REQ_PROCESS, REQ_ERROR };

private:

	// 读取Packet头数据
	bool read_header(const char *data, const size_t length, size_t & ndx)
	{
		while(m_nPacketPos < NETInputPacket::PACKET_HEADER_SIZE && ndx < length)//
		{
			m_pBuf[m_nPacketPos++] = data[ndx++];
		}
		if(m_nPacketPos < NETInputPacket::PACKET_HEADER_SIZE)
			return false;

		return true;
	}
	// 解析Packet头信息
	int parse_header(void) //0:成功 -1:包错误 -2:命令范围错误 -3:版本错误 -4:长度错误
	{
//		if(m_pBuf[0] != 'I' || m_pBuf[1] != 'C')
//			return -1;
//
//		short nCmdType = m_Packet.GetCmdType();
//		if(nCmdType < 0 || nCmdType >= 32000)
//			return -2;
//
//		if(m_Packet.GetVersion() != m_version || m_Packet.GetSubVersion() != m_subVersion)
//			return -3;
//
//		m_nBodyLen = m_Packet.GetBodyLength();
//		if(m_nBodyLen < (NETInputPacket::PACKET_BUFFER_SIZE - NETInputPacket::PACKET_HEADER_SIZE))
//			return 0;
//		return -4;
		
		if(m_pBuf[2] != 'B' || m_pBuf[3] != 'Y')
			return -1;
		
		short nCmdType = m_Packet.GetCmdType();
		if(nCmdType <= 0 || nCmdType >= 32000)
			return -2;
		
		char v1 = m_Packet.GetVersion();
		char v2 = m_Packet.GetSubVersion();
		if(v1 != m_version || v2 != m_subVersion)
			return -3;
		
		m_nBodyLen = m_Packet.GetBodyLength();
		if(m_nBodyLen >= 0 && m_nBodyLen < (NETInputPacket::PACKET_BUFFER_SIZE - 2))
			return 0;
		return -4;
	}

	// 解析ProxyPacket头信息
	int parse_Proxy_header(void) //0:成功 -1:包错误 -2:命令范围错误 -3:版本错误 -4:长度错误
	{
		m_nBodyLen = m_Packet.GetBodyLength();
		//log_debug("the body len is %d\n", m_nBodyLen);
		if(m_nBodyLen >= 0 && m_nBodyLen < (NETInputPacket::PACKET_BUFFER_SIZE - 2))
			return 0;
		return -4;
	}
	// 解析BODY数据
	bool parse_body(const char *data, const size_t length, size_t & ndx)
	{
//		size_t nNeed = (m_nBodyLen + NETInputPacket::PACKET_HEADER_SIZE) - m_nPacketPos;
//		size_t nBuff = length - ndx;
//
//		if(nNeed <= 0)
//			return true;
//		if(nBuff <= 0)
//			return false;
//
//		size_t nCopy = nBuff < nNeed ? nBuff : nNeed;
//		if(!m_Packet.WriteBody(data + ndx,  static_cast<int>(nCopy)))
//			return false;
//
//		m_nPacketPos += nCopy;
//		ndx += nCopy;
//
//		if(m_nPacketPos < (m_nBodyLen + NETInputPacket::PACKET_HEADER_SIZE))
//			return false;
//
//		return true;
		
		int nNeed = (m_nBodyLen+2) - m_nPacketPos;
		int nBuff = length - ndx;
		
		if(nNeed <= 0)
			return true;
		if(nBuff <= 0)
			return false;
		
		int nCopy = nBuff < nNeed ? nBuff : nNeed;
		if(!m_Packet.WriteBody(data + ndx, nCopy))
			return false;
		
		m_nPacketPos += nCopy;
		ndx += nCopy;
		
		if(m_nPacketPos < (m_nBodyLen + 2))
			return false;
		
		return true;
	}
};

IPacketParser * IPacketParser::CreateObject(TcpHandler * pObject)
{
	return new CPacketParser(pObject);
}
