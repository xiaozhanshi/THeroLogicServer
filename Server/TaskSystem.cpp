#include "TaskSystem.h"
#include "DBOperator.h"
#include "ConfigMgr.h"
#include "log.h"

#include <algorithm>
using namespace std;

class CUpdateTaskCount
{
public:
	CUpdateTaskCount(const uint32 actioncode, const uint32 pcate, const uint32 pframe, const uint32 uuid)
		:actioncode_(actioncode), pcate_(pcate), pframe_(pframe), uuid_(uuid)
	{

	}

	void operator()(TaskInfo & task)
	{
		//迭代任务中的所有子任务. 只负责计数, 一局游戏结束后, 统一写入
		log_debug(" _UpdateTask. task.id:=%d, subid:=%d, status_:=%d", task.id, task.subid, task.status_);
		for (vector<SubTaskInfo >::iterator subIt = task.subTasks.begin(); subIt != task.subTasks.end(); ++subIt)
		{	
			log_debug("CUpdateTaskCount %d, %d, %d.", subIt->pcate_, subIt->pframe_, subIt->actioncode_);
			//匹配任务的触发条件. 
			if (subIt->pcate_ == pcate_ && subIt->pframe_ == pframe_ && subIt->actioncode_ == actioncode_ && subIt->count < subIt->target)
			{
				task.update = true;
				++subIt->count;

				log_debug("_UpdateTask. SUBTASK.id:=%d, subid:=%d, target:=%d, count:=%d", subIt->id, subIt->taskid, subIt->target, subIt->count);
			}
		}

		task.isComplete = (find_if(task.subTasks.begin(), task.subTasks.end(), isNoComplete()) == task.subTasks.end());
		task.status_ = task.isComplete ? 1 : 0;
	}

private:
	/*const*/ uint32 actioncode_;
	/*const*/ uint32 pcate_;
	/*const*/ uint32 pframe_;
	/*const*/ uint32 uuid_;
};

class CSyncTaskRecordHelper
{
private:
	CSyncTaskRecordHelper(const CSyncTaskRecordHelper &);
	CSyncTaskRecordHelper & operator=(const CSyncTaskRecordHelper &);
public:
	//syncall 是否同步所有需要更新的记录
	explicit CSyncTaskRecordHelper(const uint32 uuid, bool syncall = false):uuid_(uuid), syncall_(syncall), m_nCompleteCount(0)
	{

	}

	void operator()(TaskInfo & task)
	{
		if (task.update == true)
		{
			log_debug("Task. CSyncTaskRecordToMem. uuid_:=%d, TASK:=%d, task.isComplete:=%d", 
			 				uuid_, task.id, task.isComplete ? 1 : 0);

			if (task.isComplete == true)
			{
				++m_nCompleteCount;
			}

			if (syncall_ == true || task.isComplete == true)
			{
				 CDBOperator::Instance().updateTask(uuid_, task, false);
				task.update = false;
			}
			else
			{
				CDBOperator::Instance().updateTask(uuid_, task, true);
			}
		}
	}

private:
	const uint32 uuid_;
	const bool syncall_;

public:
	int m_nCompleteCount;
};

CFightTask::CFightTask()
{

}

CFightTask::~CFightTask(void)
{

}

void CFightTask::updateTaskCount(const uint32 uuid, const uint32 actioncode, const uint32 pcate, const uint32 pframe)
{
	log_debug(" updateTaskRecords uuid:=%d, actioncode:=%d, pcate:=%d, pframe:=%d", uuid, actioncode, pcate, pframe);

	Tasks_type::iterator task = m_Tasks.find(uuid);
	assert(task != m_Tasks.end());
	if (task != m_Tasks.end())
	{
		//迭代所有正在进行的任务.
		for_each(task->second.begin(), task->second.end(), CUpdateTaskCount(actioncode, pcate, pframe, uuid));
	}
}

int CFightTask::update(const uint32 uuid, const bool syncall /*= false*/)
{
	const int res = update_(uuid, syncall);
	if (res > 0)
	{
		UnloadTasks(uuid);

		LoadUseTasks(uuid);
	}

	return res;
}

//const bool syncdb = false
int CFightTask::update_(const uint32 uuid, const bool syncall /*= false*/)
{
	//NEW_TIME_TRACER(CTaskSystem::update);
	//ANT_WARS_DEBUG("Task CTaskSystem::update. UUID:=%d ", uuid);

	Tasks_type::iterator task = m_Tasks.find(uuid);
	if (task != m_Tasks.end())
	{
		//迭代所有正在进行的任务.只更新任务完成的.
		CSyncTaskRecordHelper tmp(uuid, syncall);
		for_each<TaskInfos_type::iterator, CSyncTaskRecordHelper &>(task->second.begin(), task->second.end(), tmp);

		//ANT_WARS_DEBUG("Task CTaskSystem::update. UUID:=%d, tmp.m_nCompleteCount:=0 ", uuid, tmp.m_nCompleteCount);

		return tmp.m_nCompleteCount;
	}
	// 	else
	// 	{
	// 		ANT_WARS_DEBUG("Task CTaskSystem::update. UUID:=%d, TASK:=0 ", uuid);
	// 	}

	return 0;
}

void CFightTask::refreshTask(const uint32 uuid)
{
	Tasks_type::iterator it = m_Tasks.find(uuid);
	if (it != m_Tasks.end())
	{
		UnloadTasks(uuid);
	}

	LoadUseTasks(uuid);
}

void CFightTask::LoadUseTasks( const uint32 uuid )
{
	log_debug("CFightTask::LoadUseTasks. mid:=%d, ", uuid);
	pair<Tasks_type::iterator, bool> resutl = m_Tasks.insert(make_pair(uuid, TaskInfos_type()));
	assert(resutl.second == true);
	if (resutl.second == true)
	{
		CDBOperator::Instance().getUseTaskInfo(uuid, resutl.first->second);
	}
}

void CFightTask::UnloadTasks(const uint32 uuid)
{
	if (m_Tasks.find(uuid) != m_Tasks.end())
	{
		log_debug("CFightTask::UnloadTasks mid:=%d.", uuid);
		update_(uuid, true);
		m_Tasks.erase(uuid);
	}
}

void CFightTask::OnUseProp( const uint32 uuid, const int32 props )
{
	updateTaskCount(uuid, 101, 0, props == 4 ? 10 : props);
}

void CFightTask::unloadTasks( const uint32 uuid )
{
	UnloadTasks(uuid);
}

void CFightTask::OnTaskEvent( const uint32 uuid, const int32 taskcode, const int32 pcate, const int32 pframe )
{
	log_debug("CFightTask::OnTaskEvent. mid=%d. taskcode:=%d, pcate:=%d, pframe:=%d.", uuid, taskcode, pcate, pframe);
	updateTaskCount(uuid, taskcode, pcate, pframe);
}

