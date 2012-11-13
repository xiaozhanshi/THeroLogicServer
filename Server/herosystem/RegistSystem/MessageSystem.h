#ifndef __MESSAGE_SYSTEM__
#define __MESSAGE_SYSTEM__

#include "SingletonClass.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;
typedef struct gamesystem_Info_s
{
	int tid;
	string title;
	string body;
} gamesystem_Info_t;

typedef struct usersystem_Info_s
{
	int receuid;
	int senduid;
	string message;
	int types;
	int states;
	int times;
} usersystem_Info_t;

class CMessageSystem : public AutoDeleteSingleton < CMessageSystem >
{
	friend  class AutoDeleteSingleton < CMessageSystem >; 
public:
	CMessageSystem(void);
	virtual ~CMessageSystem(void);
public:
	void LoadSystemMessage();
	void LoadUserMessageByMid(int mid, vector<usersystem_Info_s> &message);
	void DeleteUserMessageByMidAFid(int mid, int fmid, vector<usersystem_Info_s> &message);
	void AddFriend(int mid, int fmid);
	void DelFriend(int mid, int fmid);
public:
	static const int INVATEMESSAGE;
	static const int USERMESSAGE;
};

extern vector<gamesystem_Info_t> g_systemMessage;

// 获取DB管理对象
#ifndef  g_GetMessageSystem
#define  g_GetMessageSystem()(CMessageSystem::Instance())
#endif 

#endif