#ifndef __REGISTSYSTEM_HEADER__
#define __REGISTSYSTEM_HEADER__

#include "SingletonClass.h"

class CRegistSystem : public AutoDeleteSingleton < CRegistSystem >
{
	friend  class AutoDeleteSingleton < CRegistSystem >; 
public:
	CRegistSystem(void);
	virtual ~CRegistSystem(void);

public:
	 bool Iphone_Login_Regist(string& IphoneMac, string& NickName, int& gameMid);
	 bool Check_User_Regist(string& UserName);
	 bool Iphone_User_Regist(string& UserName, string& PassWord, string& Mail, int isPub, int& gameMid);
	 string Get_String_Md5(const string& asci);
};

typedef struct regist_Info_s
{
	string name;
	string password;
	string md5password;
	string email;
	int isPub;
	int gameMid;
} regist_Info_t;

// ��ȡDB�������
#ifndef  g_GetRegistSystem
#define  g_GetRegistSystem()   (CRegistSystem::Instance())
#endif 

#endif

// �û�ע�ᣬ����˺ſ���ͨ��������Ϸ

// �ȼ���Ƿ��Ѿ�ע��������򷵻�ʧ�ܣ��������ע���������
// ����һ�������Ϸ�����û�

// �û���½ʱ�� �ȵ��û�����֤������˺�����
// ����Ϸ�û��������ȡID��������ҵ��˺����ƣ�