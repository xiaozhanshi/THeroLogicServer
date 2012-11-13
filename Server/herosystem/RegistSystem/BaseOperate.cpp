#include "PlatFunc.h"
#include "BaseOperate.h"
#include "log.h"

using namespace std;

CBaseOperate CBaseOperate::sInstance;

CBaseOperate& CBaseOperate::instance()
{
	return sInstance;
}

CBaseOperate::CBaseOperate()
{

}

CBaseOperate::~CBaseOperate()
{

}
