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

// 获取DB管理对象
#ifndef  g_GetRegistSystem
#define  g_GetRegistSystem()   (CRegistSystem::Instance())
#endif 

#endif

// 用户注册，这个账号可以通用所有游戏

// 先检查是否已经注册过，是则返回失败，否则插入注册表和密码表
// 创建一个这个游戏的新用户

// 用户登陆时候 先到用户表验证密码和账号名称
// 到游戏用户表里面获取ID（根据玩家的账号名称）