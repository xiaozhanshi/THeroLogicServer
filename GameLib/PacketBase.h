#ifndef _ICHAT_PACKETBASE_H_
#define _ICHAT_PACKETBASE_H_
///////////////////////////////////////////////////////////////////////////////
// PacketBase
////////////////////////////////////////////////////////////////////////////////
#include "define.h"
#include "log.h"
#include <time.h>
#include <assert.h>
#include <memory>
#include <stdarg.h>
#include <string.h>
#include <string>

#include<netinet/in.h>
#include<sys/socket.h>

using namespace std;

const static char PACKET_HEAD_FLAGS[] = "IC";

class PacketBase
{
public:
	enum PACKETVER
	{
		SERVER_PACKET_DEFAULT_VER = 1,
		SERVER_PACKET_DEFAULT_SUBVER = 1
	};

public:
	PacketBase(void){}
	virtual ~PacketBase(void){}

	char *packet_buf(void)	{return m_strBuf;}
	int packet_size(void)	{return m_nPacketSize;}
	enum
	{
		PACKET_HEADER_SIZE = 9,
		PACKET_BUFFER_SIZE = 1024 *4
	};
private:
	char m_strBuf[PACKET_BUFFER_SIZE];	// ±¨ÎÄ°ü»º´æ
	int m_nPacketSize ;	// Êµ¼Ê±¨ÎÄ×Ü³¤¶È
	int m_nBufPos;

protected:
	////////////////////////////////////////////////////////////////////////////////
	bool _copy(const void *pInBuf, int nLen)
	{
		if(nLen > PACKET_BUFFER_SIZE)
			return false;

		_reset();
		memcpy(m_strBuf, pInBuf, nLen);
		m_nPacketSize = nLen;
		assert(m_nPacketSize>=PACKET_HEADER_SIZE);
		return true;
	}
	////////////////////////////////////////////////////////////////////////////////
	void _begin(short nCmdType, char cVersion, char cSubVersion)
	{
//		_reset();
//		_writeHeader("IC", sizeof(char)*2, 0);// √¸¡Ó¬�?
//		_writeHeader((char*)&nCmdType, sizeof(short), 2);// √¸¡Ó¬�?
//		_writeHeader(&cVersion, sizeof(char), 4);		// ÷˜∞Ê±æ∫�?
//		_writeHeader(&cSubVersion, sizeof(char), 5);	// ◊”∞Ê±æ∫≈
		
		_reset();
		short cmd = htons(nCmdType);
		_writeHeader("BY", sizeof(char)*2, 2);			// magic word
		_writeHeader(&cVersion, sizeof(char), 4);		// ÷˜∞Ê±æ∫�?
		_writeHeader(&cSubVersion, sizeof(char), 5);	// ◊”∞Ê±æ∫≈
		_writeHeader((char*)&cmd, sizeof(short), 6);	// √¸¡Ó¬�?	
	}
	void _SetBegin(short nCmdType)
	{
//		_writeHeader((char*)&nCmdType, sizeof(short), 2);// √¸¡Ó¬�?
		short cmd = htons(nCmdType);
		_writeHeader((char*)&cmd, sizeof(short), 6);// √¸¡Ó¬�?
	}

	void _SetProxyId(int nProxyId)
	{
		int pid = htonl(nProxyId);
		_writeHeader((char*)&pid, sizeof(int), 2);
	}

public:
	short GetCmdType(void)
	{
//		short nCmdType;
//		_readHeader((char*)&nCmdType, sizeof(short), 2);// √¸¡Ó¬�?
//		return nCmdType;
		
		short nCmdType;
		_readHeader((char*)&nCmdType, sizeof(short), 6);// √¸¡Ó¬�?
		short cmd = ntohs(nCmdType);
		return cmd;
	}

	int GetProxyId(void)
	{
		int nProxyId;
		_readHeader((char*)&nProxyId, sizeof(int), 2);// √¸¡Ó¬�?
		int ProxyId = ntohl(nProxyId);
		return ProxyId;
	}

	char GetVersion(void)
	{
		char c;
		_readHeader(&c, sizeof(char), 4);	// Ö÷°æ±¾ºÅ
		return c;
	}
	char GetSubVersion(void)
	{
		char c;
		_readHeader(&c, sizeof(char), 5);	// ×Ó°æ±¾ºÅ
		return c;
	}
	
	short GetBodyLength(void)
	{
//		short nLen;
//		_readHeader((char*)&nLen, sizeof(short), 6);// ∞¸’˝Œƒ≥§∂�?
//		return nLen;
		
		short nLen;
		_readHeader((char*)&nLen, sizeof(short), 0);// ∞¸’˝Œƒ≥§∂�?
		short len = ntohs(nLen);
		return len;
	}

	BYTE GetcbCheckCode(void)
	{
		BYTE code;
		_readHeader((char*)&code, sizeof(BYTE), 8);// °üÕýÎÄ³¤¶È
		return code;
	}
	void _reset(void)
	{
		//memset(m_strBuf, 0, PACKET_BUFFER_SIZE);
		m_nBufPos = PACKET_HEADER_SIZE;
		m_nPacketSize = PACKET_HEADER_SIZE;
	}
protected:
	void _end()
	{
//		short nBody = static_cast<short>(m_nPacketSize - PACKET_HEADER_SIZE);
//		_writeHeader((char*)&nBody, sizeof(short), 6);	// ∞¸’˝Œƒ≥§∂�?
//		BYTE code = 0;
//		_writeHeader((char*)&code, sizeof(BYTE), 8); //–ß—È¬�?
		
		short nBody = m_nPacketSize - 2;	// ˝æ›∞¸≥§∂»∞¸¿®√¸¡ÓÕ∑∫Õbody,2∏ˆ◊÷Ω⁄ « ˝æ›∞¸≥§∂»
		short len = htons(nBody);
		_writeHeader((char*)&len, sizeof(short), 0);	// ∞¸’˝Œƒ≥§∂�?
		BYTE code = 0;
		_writeHeader((char*)&code, sizeof(BYTE), 8); //Ð§ÑéÂë
	}
	void _oldend()
	{
//		short nBody = static_cast<short>(m_nPacketSize - PACKET_HEADER_SIZE);
//		_writeHeader((char*)&nBody, sizeof(short), 6);	// ∞¸’˝Œƒ≥§∂�?
		
		short nBody = m_nPacketSize - 2;
		short len = ntohs(nBody);
		_writeHeader((char*)&len, sizeof(short), 0);	// ∞¸’˝Œƒ≥§∂�?
		
	}
	/////////////////////////////////////////////////////////////////////////////////
	
	// È¡³öÒ»¸ö±äÁ¿
	bool _Read(char *pOut, int nLen)
	{
		if((nLen + m_nBufPos) > m_nPacketSize || (nLen + m_nBufPos) > PACKET_BUFFER_SIZE)
			return false ;

		memcpy(pOut, m_strBuf + m_nBufPos, nLen);
		m_nBufPos += nLen;
		return true;
	}
	//È¡³ö±äÁ¿²¢´Ó°üÖÐÒÆ³ý
	bool _ReadDel(char *pOut, int nLen)
	{
		if(!_Read(pOut, nLen))
			return false;
		memcpy(m_strBuf + m_nBufPos - nLen, m_strBuf + m_nBufPos, PACKET_BUFFER_SIZE - m_nBufPos);
		m_nBufPos -= nLen;
		m_nPacketSize -= nLen;
		_end();
		return true;
	}
	//¶Á³·Ïû
	void _readundo(int nLen)
	{
		m_nBufPos -= nLen;
	}
	//¶Á³öµ±Ç°POSÎ»ÖÃµÄBUFFERÖ¸Õë
	char *_readpoint(int nLen) //×¢Òâ·µ»ØµÄÊÇÖ¸Õë ÇëÉ÷ÖØÊ¹ÓÃstring
	{
		if((nLen + m_nBufPos) > m_nPacketSize)
			return NULL; 
		char *p = &m_strBuf[m_nBufPos];
		m_nBufPos += nLen;
		return p;

	}
	// Ð´ÈëÒ»¸ö±äÁ¿
	bool _Write(const char *pIn, int nLen)
	{
		if((m_nPacketSize < 0) || ((nLen + m_nPacketSize) > PACKET_BUFFER_SIZE))
			return false ;
		memcpy(m_strBuf+m_nPacketSize, pIn, nLen);
		m_nPacketSize += nLen;
		return true;
	}
	//²åÈëÒ»¸ö±äÁ¿
	bool _Insert(const char *pIn, int nLen)
	{
		if((nLen + m_nPacketSize) > PACKET_BUFFER_SIZE)
			return false;
		memcpy(m_strBuf+PACKET_HEADER_SIZE+nLen, m_strBuf+PACKET_HEADER_SIZE, m_nPacketSize-PACKET_HEADER_SIZE);
		memcpy(m_strBuf+PACKET_HEADER_SIZE, pIn, nLen);
		m_nPacketSize += nLen;
		_end();
		return true;
	}
	// Ð´ÈëÒ»¸ö±äÁ¿
	bool _writezero(void)
	{
		if((m_nPacketSize + 1) > PACKET_BUFFER_SIZE)
			return false ;
		memset(m_strBuf+m_nPacketSize, '\0', sizeof(char)) ;
		m_nPacketSize ++;
		return true;
	}
	// readHeader
	void _readHeader(char *pOut, int nLen, int nPos)
	{
		if(nPos > 0 || nPos+nLen < PACKET_HEADER_SIZE)
		{
			memcpy(pOut, m_strBuf+nPos, nLen) ;
		}
	}
	// writeHeader
	void _writeHeader(const char *pIn, int nLen, int nPos)
	{
		if(nPos > 0 || nPos+nLen < PACKET_HEADER_SIZE)
		{
			memcpy(m_strBuf+nPos, pIn, nLen) ;
		}
	}
};

//template <int _buffer_size>
class NETInputPacket: public PacketBase//<_buffer_size>
{
public:
	typedef PacketBase/*<_buffer_size>*/ base;

//	int ReadInt(void)		{int nValue = -1; base::_Read((char*)&nValue, sizeof(int)); return nValue;} //’‚¿Ô±ÿ–Ë≥ı ºªØ
//	unsigned int ReadUInt(void)	{unsigned int nValue = 0; base::_Read((char*)&nValue, sizeof(unsigned int)); return nValue; }
//	unsigned long ReadULong(void) {unsigned long nValue = 0; base::_Read((char*)&nValue, sizeof(unsigned long)); return nValue;}
//	int ReadIntDel(void)	{int nValue = -1; base::_ReadDel((char*)&nValue, sizeof(int)); return nValue;} 
//	short ReadShort(void)	{short nValue = -1; base::_Read((char*)&nValue, sizeof(short)); return nValue;}
//	BYTE ReadByte(void)		{BYTE nValue = 0; base::_Read((char*)&nValue, sizeof(BYTE)); return nValue;}

	int ReadInt(void)		{int nValue = -1; base::_Read((char*)&nValue, sizeof(int)); return ntohl(nValue);} //’‚¿Ô±ÿ–Ë≥ı ºªØ
	unsigned int ReadUInt(void)	{unsigned int nValue = 0; base::_Read((char*)&nValue, sizeof(unsigned int)); return ntohl(nValue); }
	unsigned long ReadULong(void) {unsigned long nValue = -1; base::_Read((char*)&nValue, sizeof(unsigned long)); return ntohl(nValue);}
	int ReadIntDel(void)	{int nValue = -1; base::_ReadDel((char*)&nValue, sizeof(int)); return ntohl(nValue);} 
	short ReadShort(void)	{short nValue = -1; base::_Read((char*)&nValue, sizeof(short)); return ntohs(nValue);}
	BYTE ReadByte(void)		{BYTE nValue = -1; base::_Read((char*)&nValue, sizeof(BYTE)); return nValue;}
	
	bool ReadString(char *pOutString, int nMaxLen)
	{
		int nLen = ReadInt();
		if(nLen == -1)  //ÕâÀï±ØÐèÅÐ¶Ï
			return false;
		if(nLen > nMaxLen)
		{
			base::_readundo(sizeof(short));
			return false;
		}
		return base::_Read(pOutString, nLen);
	}

	char *ReadChar(void)
	{
		int nLen = ReadInt();
		if(nLen == -1) 
			return NULL;
		return base::_readpoint(nLen);
	}

	string ReadString(void)
	{
		char *p = ReadChar();
		return (p == NULL ? "" : p);
	}

	int ReadBinary(char *pBuf, int nMaxLen)
	{
		int nLen = ReadInt();
		if(nLen == -1) 
			return -1;
		if(nLen > nMaxLen)
		{
			base::_readundo(sizeof(int));
			return -1;
		}
		if(base::_Read(pBuf, nLen))
			return nLen ;
		return 0;
	}
	void Reset(void)
	{
		base::_reset();
	}
	bool Copy(const void *pInBuf, int nLen)
	{
		return base::_copy(pInBuf, nLen);
	}
	bool WriteBody(const char *pIn, int nLen)
	{
		return base::_Write(pIn, nLen);
	}
	////ÓÃÓÚÎ±Ôì½ÓÊÕ°ü
	void Begin(short nCommand, char cVersion = SERVER_PACKET_DEFAULT_VER, char cSubVersion = SERVER_PACKET_DEFAULT_SUBVER)
	{
		base::_begin(nCommand, cVersion, cSubVersion);
	}

	void End(void)
	{
		base::_end();
	}

	char * toHexString(/*string & str*/)
	{
		char * p = new char[packet_size() * 2 + 1];
		memset(p, 0, packet_size() * 2 + 1);
		for (int i = 0 ; i < this->packet_size(); ++i)
		{
			sprintf(&p[2 * i], "%02x", this->packet_buf()[i]);
		}

		return p;
	}

	void dump()
	{
		char * p = toHexString();
		//log_debug("<PACKET timestamp='%u' type='%d' msg='%s'/>\n", time(NULL), GetCmdType(), p);
		delete [] p;
		p = NULL;
	}
};

//template <int BUFFER_SIZE>
class NETOutputPacket: public PacketBase//<BUFFER_SIZE>
{
	bool m_isCheckCode;
public:
	NETOutputPacket(void){m_isCheckCode = false;}
public:
	typedef PacketBase base;

	//bool WriteInt(int nValue)		{return base::_Write((char*)&nValue, sizeof(int));}
	bool WriteUInt(unsigned int nValue)	{int value = htonl(nValue); return base::_Write((char*)&value, sizeof(unsigned int));}
	//bool WriteULong(unsigned long nValue) {return base::_Write((char*)&nValue, sizeof(unsigned long));}
	bool WriteInt(int nValue)		{int value = htonl(nValue); return base::_Write((char*)&value, sizeof(int));}
	bool WriteULong(unsigned long nValue) {unsigned long value = htonl(nValue);return base::_Write((char*)&value, sizeof(unsigned long));}
	bool WriteByte(BYTE nValue)		{return base::_Write((char*)&nValue, sizeof(BYTE));}
	bool WriteShort(short nValue)	{short value = htons(nValue); return base::_Write((char*)&value, sizeof(short));}
	//‘⁄’˝Œƒ ◊≤Â»Î ˝æ�?
	bool InsertInt(int nValue)		{int value = htonl(nValue); return base::_Insert((char*)&value, sizeof(int));}
	bool InsertByte(BYTE nValue)	{return base::_Insert((char*)&nValue, sizeof(BYTE));}
	bool WriteString(const char *pString)
	{
		int nLen = (int)strlen(pString) ;
		WriteInt(nLen + 1) ;
		return base::_Write(pString, nLen) && base::_writezero();
	}

// 	bool WriteString(const char* pString, int nLen)
// 	{
// 		WriteInt(nLen+1);
// 		return base::_Write(pString, nLen) && base::_writezero();
// 	}

	bool WriteString(const string & strDate)
	{
		int nLen = (int)strDate.size();
		WriteInt(nLen + 1) ;
		return base::_Write(strDate.c_str(), nLen) && base::_writezero();
	}

	bool WriteBinary(const char *pBuf, int nLen)
	{
		WriteInt(nLen) ;
		return base::_Write(pBuf, nLen) ;
	}
	bool Copy(const void *pInBuf, int nLen)
	{
		return base::_copy(pInBuf, nLen);
	}
	bool CopyData(const char* pInBuf, int nBufLen)
	{
		return base::_Write(pInBuf, nBufLen);
	}

	void Begin(short nCommand, char cVersion = SERVER_PACKET_DEFAULT_VER, char cSubVersion = SERVER_PACKET_DEFAULT_SUBVER)
	{
		base::_begin(nCommand, cVersion, cSubVersion);
		m_isCheckCode = false;
	}

	void End(void)
	{
		m_isCheckCode = false;
		base::_end();
	}

	void oldEnd(void)
	{
		m_isCheckCode = false;
		base::_oldend();
	}
	void SetProxyId(int nProxyId)
	{
		base::_SetProxyId(nProxyId);
	}
	//Ôö¼Ó
	void SetBegin(short nCommand)
	{
		base::_SetBegin(nCommand);
	}
	//Ð§ÑéÂë
	void WritecbCheckCode(BYTE nValue)
	{
		base::_writeHeader((char*)&nValue, sizeof(BYTE), 8); //Ð§ÑéÂë
		m_isCheckCode = true;
	}

	bool IsWritecbCheckCode(void)
	{
		return m_isCheckCode;
	}

	void BuildPackage(short nCmdType, const char* pszFmt, ...)
	{
		Begin(nCmdType);

		if (pszFmt == NULL)	//ÄÚÈÝÎª¿ÕµÄ,¾ÍÖ±½Ó¹¹ÔìÒ»¸öÖ»ÓÐÃüÁîÍ·µÄÊý¾Ý°ü
		{
			End();
			return;
		}

		va_list ap; 
		va_start (ap, pszFmt); 
		const char* p = NULL;

		for (p= pszFmt; *p; p++) 
		{ 
			if (*p != '%') 
			{
				continue; 
			}

			switch (*++p) 
			{ 
			case 'd':	//int
				{
					int nVal= va_arg(ap, int);
					WriteInt(nVal);
					break;
				}
			case 'h':	//short
				{
					const int shVal = va_arg(ap, int);
					WriteShort(static_cast<short>(shVal));
					break;
				}
			case 'u':	//unsigned long
				{
					unsigned long dwVal = va_arg(ap, unsigned long);
					WriteULong(dwVal);
					break;
				}
			case 's':	//char*
				{
					char* pVal = va_arg(ap, char*);
					WriteString(pVal);
					break;
				}
			}
		}
		End();
	}
};

#endif //_ICHAT_PACKETBASE_H_

