#include "TimerObj.h"
#include "PlayerMgr.h"
#include "CentServer.h"
#include "cpush.h"
#include "MailSystem.h"

namespace
{
    static  const uint32 SECONDS_IN_ONE_DAY	    = 86400;    // 一天的秒数
    static  const uint32 SECONDS_IN_ONE_WEEK    = 7*86400;  // 一周的秒数
    static  const uint32 UPDATE_STRESS_TIME     = 1200;     // 负载更新时间
};

StressTimerObj::StressTimerObj()
{
    m_xTimer.SetTimeEventObj(this);
}

StressTimerObj::~StressTimerObj()
{
    m_xTimer.StopTimer();
}

void StressTimerObj::StartTimer(int nSec)
{
    m_xTimer.StartTimer(nSec);
}

int StressTimerObj::ProcessOnTimerOut(int Timerid)
{
	log_debug("sent mail test");
	//send_Push_Message(1, NULL);
	//g_mail.SendEmail();
	g_mail.GetIPandSent();
    StartTimer(UPDATE_STRESS_TIME);
    return 0;
}