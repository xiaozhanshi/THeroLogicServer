#ifndef _TASKSYSTEM_H_
#define _TASKSYSTEM_H_ 

#include "TaskHandler.h"

class CFightTask : public ITaskHandler
{
protected:
	CFightTask(const CFightTask &);
	CFightTask & operator=(const CFightTask &);

public:
	CFightTask();
	virtual ~CFightTask(void);

	/// 更新与使用道具相关的任务计数.
	/**
	*
	* @param uuid 角色ID.
	*
	* @param props 道具ID.
	*
	* @return 。
	*
	* @node 可能同时更新多个任务记数，实时更新缓存数据并且如任务完成了的话，立即同步数据库的数据 。
	*
	*/
	virtual void OnUseProp(const uint32 uuid, const int32 props);

	/// 卸载用户的任务数据.将数据同步的缓存和数据库
	/**
	*
	* @param uuid 角色ID.
	*
	* @return 。
	*
	*/
	virtual void unloadTasks(const uint32 uuid);

	virtual void OnTaskEvent(const uint32 uuid, const int32 taskcode, const int32 pcate, const int32 pframe);

	/// 重新从缓存读取用户的任务数据
	/**
	*
	* @param uuid 角色ID.
	*
	* @return 。
	*
	* @node 如果有任务记数录数的更新，则先同步到缓存和数据库，再刷新数据。
	*
	*/
	virtual void refreshTask(const uint32 uuid);

private:
	void updateTaskCount(const uint32 uuid, const uint32 actioncode, const uint32 pcate, const uint32 pframe);

	void UnloadTasks(const uint32 uuid);

	//返回已完成的任务记数.如果有任务完成了, 则需要刷新任务列表.
	int update(const uint32 uuid, const bool syncall = false);

	int update_(const uint32 uuid, const bool syncall = false);

	void LoadUseTasks(const uint32 uuid);

	void SyncTaskRecordToDB(const uint32 uuid);

private:
	Tasks_type m_Tasks;
};

#endif //_TASKSYSTEM_H_

