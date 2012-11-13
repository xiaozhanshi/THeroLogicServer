#include "MessageSystem.h"
#include "DBOperator.h"
#include "log.h"
#include "MD5.h"

vector<gamesystem_Info_t> g_systemMessage;

const int CMessageSystem::INVATEMESSAGE = 1;
const int CMessageSystem::USERMESSAGE = 2;

CMessageSystem::CMessageSystem(void)
{

}

CMessageSystem::~CMessageSystem(void)
{

}

void CMessageSystem::LoadSystemMessage()
{

}

void CMessageSystem::LoadUserMessageByMid(int mid, vector<usersystem_Info_s> &message)
{
	
}

void CMessageSystem::DeleteUserMessageByMidAFid(int mid, int fmid, vector<usersystem_Info_s> &message)
{
	// 同时删除数据库和内存表
}

void CMessageSystem::AddFriend(int mid, int fmid)
{
	//if (mid == fmid)
	//{
	//	return;
	//}
	//bool ret = g_GetCDBOperator().AddFriend(mid, fmid);
	//if (!ret)
	//{
	//	log_error("error in add new friend");
	//}
}

void CMessageSystem::DelFriend(int mid, int fmid)
{
	//bool ret = g_GetCDBOperator().DelFriend(mid, fmid);
	//if (!ret)
	//{
	//	log_error("error in add new friend");
	//}
}