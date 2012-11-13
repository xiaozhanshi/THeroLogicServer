
#include "Thread.h"
#ifndef WIN32
#include <unistd.h>
#endif

void Thread::sleep(int millis)
{
#ifdef WIN32
	Sleep(millis);
#else
	usleep(millis*1000);
#endif
}

ThreadID Thread::getId()
{
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

Thread::~Thread()
{
}
void Thread::start()
{
#ifdef WIN32
	HANDLE h;
	h=CreateThread(NULL,0,runThread,this,0,&p_thread);
	if(h!=0){
		m_hThread=h;
	}
#else
	pthread_create(&p_thread,0,runThread,this);
#endif
	if(p_thread==0){
		//throw Exception(CreationFailure);
	}
}

	//等待线程完成
void Thread::join()
{
#ifdef WIN32
	WaitForSingleObject(m_hThread,INFINITE);
	CloseHandle(m_hThread);	
#else
	pthread_join(p_thread,NULL);
#endif
}

	//立即杀死线程，不建议使用，可能引起内存泄漏
void Thread::kill()
{
#ifdef WIN32
	TerminateThread(m_hThread,0);
	CloseHandle(m_hThread);	
#else
	pthread_cancel(p_thread);
#endif
}

#ifdef WIN32
DWORD WINAPI runThread(void* p)
#else
void* runThread(void* p)
#endif
{
    Thread *thread=(Thread*)p;
    thread->run();
    return 0;
}


