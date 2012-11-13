#ifndef _ANT_WARS_SERVER_TASKHANDLER_H_
#define _ANT_WARS_SERVER_TASKHANDLER_H_

#include "define.h"

#include <vector>
#include <string>
#include <map>
using namespace std;

typedef struct _TaskDefine
{
	//uint32 mtype;			//任务类型:任务类型(1主线任务,2日常任务)
	uint32 smsid;			//子任务ID = 用户子任务ID
	uint32 actioncode;		//任务类型
	uint32 pcate;			//操作代码大类
	uint32 pframe;			//操作代码小类 //actioncode. pcate. pframe 
	uint32 times;			//需要完成的次数.

	bool operator==(const uint32 smsid_)const
	{
		return smsid == smsid_;
	}
}
TaskDefine;

typedef vector<TaskDefine > TaskDefines_type;

typedef struct _tagSubTaskInfo
{
	_tagSubTaskInfo(uint32 id_, uint32 subid_, uint32 count_, uint32 target_, uint32 actioncode, uint32 pcate, uint32 pframe)
		: id(id_), taskid(subid_), count(count_), /*op(op_), */ target(target_), actioncode_(actioncode), pcate_(pcate), pframe_(pframe)
	{

	}

	uint32 id;				//用户子任务id
	uint32 taskid;			//子任务ID
	uint32 count;			//完成次数
	uint32 target;			//需要完成次数

	uint32 actioncode_;		//任务分类
	uint32 pcate_;			//操作代码
	uint32 pframe_;			//操作代码
} SubTaskInfo;

class isNoComplete
{
public:
	bool operator()(const SubTaskInfo & x)
	{
		return x.target != x.count;
	}
};

struct TaskInfo
{
	TaskInfo():update(false), isComplete(false) {}

	bool   update;			//任务数据是否被更新
	bool   isComplete;		//任务是否完成.

	uint32 id;				//用户任务id
	uint32 subid;			//任务分类id,
	uint32 status_;			//任务状态 
	string timeTag;			//完成时间
	//uint32 mtype;			//任务类型:任务类型(1主线任务,2日常任务)

	vector<SubTaskInfo > subTasks;
} ;

typedef vector<TaskInfo > TaskInfos_type;

typedef map<uint32, TaskInfos_type > Tasks_type;

class ITaskHandler
{
public:
	virtual ~ITaskHandler() {}
public:
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
	virtual void OnUseProp(const uint32 uuid, const int32 props) = 0;

	/// 更新任务计数.
	/**
	*
	* @param uuid 角色ID.
	*
	* @param pcate 任务分类ID.
	*
	* @param pframe 任务ID.
	*
	* @return 。
	*
	* @node 可能同时更新多个任务记数，实时更新缓存数据并且如任务完成了的话，立即同步数据库的数据 。
	*
	*/
	virtual void OnTaskEvent(const uint32 uuid, const int32 taskcode, const int32 pcate, const int32 pframe) = 0;


	/// 更新与战斗相关的任务计数.
	/**
	*
	* @param uuid 角色ID.
	*
	* @param winner 是否取得了胜利.
	*
	* @param roomtype 战斗所有房间的类型.
	*
	* @node 可能同时更新多个任务记数，实时更新缓存数据并且如任务完成了的话，立即同步数据库的数据 。
	*
	* @return 。
	*
	*/
	//virtual void OnGameOver(const uint32 uuid, const bool winner, const int32 roomtype) = 0;

	/// 卸载用户的任务数据.将数据同步的缓存和数据库
	/**
	*
	* @param uuid 角色ID.
	*
	* @return 。
	*
	*/
	virtual void unloadTasks(const uint32 uuid) = 0;

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
	virtual void refreshTask(const uint32 uuid) = 0;
};

ITaskHandler * getTaskHandler();

#endif


