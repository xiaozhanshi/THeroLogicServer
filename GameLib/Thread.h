
#ifndef THREAD_H_20110719
#define THREAD_H_20110719

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

//������
#ifdef WIN32
class MyLock
{
    CRITICAL_SECTION m_Lock ;
public :
    MyLock(){ InitializeCriticalSection(&m_Lock); } ;
    ~MyLock(){ DeleteCriticalSection(&m_Lock); } ;
    void	Lock(){ EnterCriticalSection(&m_Lock); } ;
    void	Unlock(){ LeaveCriticalSection(&m_Lock); } ;
};
#else
class MyLock
{
    pthread_mutex_t 	m_Mutex; 
public :
    MyLock(){ pthread_mutex_init( &m_Mutex , NULL );} ;
    ~MyLock(){ pthread_mutex_destroy( &m_Mutex) ; } ;
    void	Lock(){ pthread_mutex_lock(&m_Mutex); } ;
    void	Unlock(){ pthread_mutex_unlock(&m_Mutex); } ;
};
#endif
//�Զ�����������
class AutoLock_T
{
public:
    AutoLock_T(MyLock& rLock)
    {
        m_pLock = &rLock;
        m_pLock->Lock();
    }
    ~AutoLock_T()
    {
        m_pLock->Unlock();
    }
protected:
private:
    AutoLock_T();
    MyLock* m_pLock;
};

#ifdef WIN32
typedef DWORD ThreadID;
#else
typedef pthread_t ThreadID;
#endif

#ifdef WIN32
DWORD WINAPI runThread(void* p);
#else
void* runThread(void* p);
#endif


class Thread
{
public:
    virtual ~Thread();//Ϊ����Thread����������ȷ����
	virtual void run()=0;
	void        start();
	//�ȴ��߳����
	void        join();
	//����ɱ���̣߳�������ʹ�ã����������ڴ�й©
	void        kill();
protected:
    void        sleep(int millis);
    ThreadID    getId();
private:
    ThreadID p_thread;

#ifdef WIN32
    HANDLE m_hThread ;
#endif

};



#endif //THREAD_H_20110719


