#include "DBOperator.h"
#include "RegistSystem.h"
#include "log.h"
#include "MD5.h"

CRegistSystem::CRegistSystem(void)
{

}

CRegistSystem::~CRegistSystem(void)
{

}

// ��ȡ��ϷID
bool CRegistSystem::Iphone_Login_Regist(string& IphoneMac, string& NickName, int& gameMid)
{
	// check the mac, �ҵ������Ϸid������һ�㣬���û���ҵ�����ע��һ����ϷID����
	// Mac����֤
	gameMid = 0;
	g_GetCDBOperator().CheckAndAddUser(IphoneMac, NickName, gameMid);
	if (gameMid == 0) //ע��ʧ��
	{
		log_error("user login error, Iphone_Login_Regist");
		return false;
	} 	
	return true;
}

bool CRegistSystem::Check_User_Regist(string& UserName)
{
	int gameMid = 0;
	bool ret2; 
	ret2 = g_GetCDBOperator().CheckAndAddRegistUser(UserName, gameMid);
	if (ret2)
	{
		log_error("user regist error");
		return false;
	}
	
	return true;
}

bool CRegistSystem::Iphone_User_Regist(string& UserName, string& PassWord, string& Mail, int isPub, int& gameMid)
{
	// ����һ�����û����������ݿ����������md5�ǹ����ģ�����һ����ԭʼ���룬�ֱ��ǲ�һ����
	regist_Info_t userInfo;
	string md5PassWord = Get_String_Md5(PassWord);

	userInfo.name = UserName;
	userInfo.password = PassWord;
	userInfo.md5password = md5PassWord;
	userInfo.email = Mail;
	userInfo.isPub = isPub;
	userInfo.gameMid = gameMid;

	bool ret = g_GetCDBOperator().AddRegistUser(userInfo);
	
	return ret;
}

string CRegistSystem::Get_String_Md5(const string& asci)
{
	char md5PassWord[500] = {0};
	char cPassWord[30] = {0};
	memcpy(cPassWord, asci.c_str(), 29);

	CMD5::MD5((unsigned char*)cPassWord, strlen(cPassWord), md5PassWord);
	log_debug("the source password is %s, the md5passWord is %s\n", cPassWord, md5PassWord);

	string ret = md5PassWord;

	return ret;
}
