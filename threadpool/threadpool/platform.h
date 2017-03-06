#ifndef PLATFORM_H__
#define PLATFORM_H__

/*
**WIN32/_WIN32 ���������ж��Ƿ� Windows ϵͳ�����ڿ�ƽ̨���򣩣�
�� _WIN64 �����жϱ��뻷���� x86 ���� x64
*/

#ifdef WIN32
#include <windows.h>
#define SLEEP(x)	Sleep(x)
#else
//�Ժ��ƽ̨����ʱ���滻
#endif


//thread
#ifdef _WIN32
#include <process.h>
#define THREAD_EXIT							return
#define THREAD_API 
#define THREAD_TYPE							void*
#define DEFINE_THREAD(_thread)				void* _thread
#define BEGIN_THREAD(_thread,_pfn,_pArg)	_thread = (HANDLE)_beginthread(_pfn,0,_pArg)
#define END_THREAD()							_endthread();
#define FAILED_THREAD(_t)					(_t)<=0
#define SUCCEED_THREAD(_t)					(_t)>0
#define WAITTHREADEND(_t)					WaitForSingleObject(_t,INFINITE)

#else
//�Ժ��ƽ̨����ʱ���滻
#endif

//Event?condition

#ifdef _WIN32
#define DEFINE_EVENT(_event)				HANDLE _event
#define INITIALIZE_EVENT(_event)			_event = CreateEvent(NULL,FALSE,FALSE,NULL);
#define SIGNAL_EVENT(_event)				SetEvent(_event);
#define BROADCAST_EVENTT(_event)			SetEvent(_event);
#define WAIT_EVENT(_event,_timeout,_mutex)	WaitForSingleObject(_event,_timeout);
#define DESTROY_EVENT(_event)				CloseHandle(_event)
#else
//�Ժ��ƽ̨����ʱ���滻

#endif

//Mutex

#ifdef _WIN32

#define MUTEXTYPE		HANDLE
#define MUTEXDEFX(x)	HANDLE x
#define MUTEXINITX(x)	x = CreateMutex(NULL,FALSE,NULL)
#define MUTEXLOCKX(x)	WaitForSingleObject(x,INFINITE)
#define MUTEXUNLOCK(x)	ReleaseMutex(x)
#define MUTEXDESDROYX(x)	CloseHandle(x)
#else
//�Ժ��ƽ̨����ʱ���滻

#endif


class Mutex
{
public:
	Mutex() { MUTEXINITX(x); }
	~Mutex()
	{
		MUTEXDESDROYX(x);
	}
	void Lock()
	{
		MUTEXLOCKX(x);
	}
	void Unlock()
	{
		MUTEXUNLOCK(x);
	}

	MUTEXTYPE* operator &() { return &x; }
private:
	MUTEXDEFX(x);

};
class Event
{
public:
	Event()
	{
		INITIALIZE_EVENT(x);
	}
	~Event()
	{
		DESTROY_EVENT(x);
	}
	void Signal()
	{
		SIGNAL_EVENT(x);
	}
	void Broadcast()
	{
		BROADCAST_EVENTT(x);
	}
	void Wait(int timeout,Mutex& m)
	{
		WAIT_EVENT(x, timeout,&m);
	}
private:
	DEFINE_EVENT(x);
};

template<class T>
class Lock
{
public:
	explicit Lock(T &cs):m_lock(cs)
	{
		m_lock.Lock();
	}
	~Lock() { m_lock.Unlock(); }
private:
	T& m_lock;
};

#endif