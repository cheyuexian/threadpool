#ifndef TASK_H__
#define TASK_H__ 

typedef void(*FUN)(void* arg);

class Task
{
public:
	Task(FUN fun, void* arg);
	~Task(void );
	void	Run();
private:
	FUN		m_pFunc;
	void	*m_arg;
};
#endif
