#ifndef __MAILSYSTEM__
#define __MAILSYSTEM__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

class CMailSystem{
public:
	static CMailSystem& instance();

protected:
	static CMailSystem sInstance;

private:
	CMailSystem();
	~CMailSystem();

public:
	int SendEmail(string & mailaddress, string & password);
	void GetIPandSent();	
};

#ifndef  g_mail
#define  g_mail (CMailSystem::instance())
#endif

#endif