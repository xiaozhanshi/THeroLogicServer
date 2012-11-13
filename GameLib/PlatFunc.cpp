#include "PlatFunc.h"

#ifdef WIN32
DWORD GetTickCount()
{
	return -1;
}
int GetDelayTime()
{
	return -1;
}
DWORD GetUnixTime()
{
	return -1;
}
#else
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

DWORD GetTickCount()
{
	timeval timesnow;
	struct timezone tmpzone;
	gettimeofday(&timesnow,&tmpzone);
	return ((DWORD)(timesnow.tv_sec*1000) + (DWORD)(timesnow.tv_usec/1000));
}

DWORD GetUnixTime()
{
	time_t lt;
	lt =time(NULL);
	
	return lt;
}

int GetDelayTime()
{
	time_t timep;
	struct tm *p;
	int delaytime = 0;

	time(&timep);
	p=localtime(&timep);

	/*tomorrow zero time*/
	delaytime = 86400 - p->tm_hour * 3600 - p->tm_min * 60 - p->tm_sec;

	return delaytime;
}
#endif