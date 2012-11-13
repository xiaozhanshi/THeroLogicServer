#ifndef PLATFUNC_H__20110531_
#define PLATFUNC_H__20110531_

#include "define.h"
#include <time.h>

#ifdef WIN32
DWORD GetTickCount();
int GetDelayTime();
DWORD GetUnixTime();
#else
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

DWORD GetTickCount();
DWORD GetUnixTime();
int GetDelayTime();
#endif

#endif // PLATFUNC_H__20110531

