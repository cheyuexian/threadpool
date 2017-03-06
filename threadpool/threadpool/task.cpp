
#include "task.h"

Task::Task(FUN fun, void* arg):m_pFunc(fun),m_arg(arg)
{
	
}

Task::~Task(void)
{
	if (m_arg != 0)
	{
		delete m_arg;
		m_arg = 0;
	}
}

void Task::Run(void )
{
	(*m_pFunc)(m_arg);
}
