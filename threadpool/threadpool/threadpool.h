#ifndef THREADPOOL_H__
#define THREADPOOL_H__

#include <vector>
#include <deque>

#include "platform.h"
#include "task.h"
class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();
	enum 
	{
		DEFAULT_POOL_SIZE = 10,
		DEFAULT_TASK_QUEUE_SIZE = 50,
		TPS_STARTED = 1,//thread pool state-started
		TPS_STOPPED =2, //thread pool state-stopped
	};

	int		Initialize(const int pool_size = DEFAULT_POOL_SIZE, const int queue_size = DEFAULT_TASK_QUEUE_SIZE);
	int		Destroy();
	bool	AddTask(Task *task);
	void	WaitAll();

	void	ExecThreadFunction();
private:
	bool	CheckHasTask();
	unsigned int TaskQueueSize();
private:
	unsigned int m_poolSize;
	unsigned int m_queue_size;
	std::vector<THREAD_TYPE> m_threads;
	std::deque<Task*> m_queueTasks;
	Mutex		m_mutexTask;   //»¥³âËø
	Event		m_eventTask;	//ÊÂ¼þ
	volatile	int m_statePool;

};
#endif