
#include <iostream>

#include "threadpool.h"


extern "C"  void THREAD_API MyThreadFunc(void *arg)
{
	ThreadPool *tp = (ThreadPool*)arg;
	tp->ExecThreadFunction();
	END_THREAD();
	THREAD_EXIT;
}

ThreadPool::ThreadPool()
	:m_poolSize(DEFAULT_POOL_SIZE),m_queue_size(DEFAULT_TASK_QUEUE_SIZE)
	,m_mutexTask(),m_eventTask(),m_statePool(TPS_STOPPED)
{
	m_threads.clear();
}

ThreadPool::~ThreadPool()
{
	if (m_statePool != TPS_STOPPED)
	{
		Destroy();
	}
}

int ThreadPool::Initialize(const int pool_size /*= DEFAULT_POOL_SIZE*/, const int queue_size /*= DEFAULT_TASK_QUEUE_SIZE*/)
{
	int ret = -1;
	if (pool_size < 1 || queue_size < 1)
	{
		std::cerr << "pool size < 1 or queue_size < 1\n";
		return -1;
	}

	m_poolSize = pool_size;
	m_queue_size = queue_size;
	m_statePool = TPS_STARTED; // 开始

	int ok_thread_count = 0;

	for (unsigned int i = 0; i < m_poolSize; ++i)
	{
		DEFINE_THREAD(oneThread);
		BEGIN_THREAD(oneThread, MyThreadFunc, this);
		if (FAILED_THREAD(oneThread))
		{
			std::cerr << "create thread failed()! " << oneThread << std::endl;
		}
		else
		{
			ok_thread_count++;
			m_threads.push_back(oneThread);
		}
	}

	return ok_thread_count - m_poolSize;

}

int ThreadPool::Destroy()
{
	if (m_statePool = TPS_STOPPED)
		return 0;

	{
		Lock<Mutex> g(m_mutexTask);  //局部变量 会自动析构,unlock
		m_statePool = TPS_STOPPED;
	}
	

	m_eventTask.Broadcast();

	for (unsigned int i = 0; i < m_poolSize; ++i)
	{
		m_eventTask.Broadcast();
		WAITTHREADEND(m_threads[i]);
	}

	return 0;
}

bool ThreadPool::AddTask(Task *task)
{
	Lock<Mutex> g(m_mutexTask);

	if (m_queueTasks.size() < m_queue_size)
	{
		m_queueTasks.push_back(task);
		m_eventTask.Signal();
		return true;
	}

	return false;
}

void ThreadPool::WaitAll()
{
	while (TaskQueueSize() != 0)
		SLEEP(10);
	Destroy();
}

void ThreadPool::ExecThreadFunction()
{
	Task *task = NULL;
	while (1)
	{
#ifndef WIN32

		m_mutexTask.Lock();
#endif

		while (CheckHasTask())
		{
			m_eventTask.Wait(10, m_mutexTask);
		}

#ifdef WIN32
		m_mutexTask.Lock();
#endif

		if (m_statePool == TPS_STOPPED)
		{
			m_mutexTask.Unlock();
			break;
		}
		if (!m_queueTasks.empty())
		{
			task = m_queueTasks.front();
			m_queueTasks.pop_front();
		}

		m_mutexTask.Unlock();

		if (task)
		{
			task->Run();
			delete task;
			task = NULL;
		}
	}
}

bool ThreadPool::CheckHasTask()
{
#ifdef WIN32

	Lock<Mutex> g(m_mutexTask);
#endif

	return (m_statePool != TPS_STOPPED) && (m_queueTasks.empty());
}

unsigned int ThreadPool::TaskQueueSize()
{
	Lock<Mutex> g(m_mutexTask);
	return m_queueTasks.size();
}

