#ifndef __BASEOPERATE_HEADER__
#define __BASEOPERATE_HEADER__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

class CBaseOperate
{
public:
	static CBaseOperate& instance();

protected:
	static CBaseOperate sInstance;

private:
	CBaseOperate();
	~CBaseOperate();
};

#ifndef  g_baseoperate
#define  g_baseoperate (CBaseOperate::instance())
#endif

#endif