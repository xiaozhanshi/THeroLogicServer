#include "TaskHandler.h"
#include "TaskSystem.h"
#include "ConfigMgr.h"

ITaskHandler * getTaskHandler()
{
	static ITaskHandler * p = NULL;
	if (p == NULL)
	{
		p = new CFightTask();
	}

	return p;
}

