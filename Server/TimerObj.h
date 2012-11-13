
#ifndef TIMEROBJ_H__20110611
#define TIMEROBJ_H__20110611

#include "TimerOutEvent.h"
#include "GameData.h"
#include "TimerEvent.h"

class CGameServer;
class CentServer;

// 负载刷新定时器
class StressTimerObj : public TimerOutEvent
{
public:
    StressTimerObj();
    virtual ~StressTimerObj();
    void        StartTimer(int nSec);    
    virtual int ProcessOnTimerOut(int Timerid);
private:
    TimerEvent      m_xTimer;		    // 喇叭定时器
};

#endif // TIMEROBJ_H__20110611