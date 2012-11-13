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

	/// ������ʹ�õ�����ص��������.
	/**
	*
	* @param uuid ��ɫID.
	*
	* @param props ����ID.
	*
	* @return ��
	*
	* @node ����ͬʱ���¶�����������ʵʱ���»������ݲ�������������˵Ļ�������ͬ�����ݿ������ ��
	*
	*/
	virtual void OnUseProp(const uint32 uuid, const int32 props);

	/// ж���û�����������.������ͬ���Ļ�������ݿ�
	/**
	*
	* @param uuid ��ɫID.
	*
	* @return ��
	*
	*/
	virtual void unloadTasks(const uint32 uuid);

	virtual void OnTaskEvent(const uint32 uuid, const int32 taskcode, const int32 pcate, const int32 pframe);

	/// ���´ӻ����ȡ�û�����������
	/**
	*
	* @param uuid ��ɫID.
	*
	* @return ��
	*
	* @node ������������¼���ĸ��£�����ͬ������������ݿ⣬��ˢ�����ݡ�
	*
	*/
	virtual void refreshTask(const uint32 uuid);

private:
	void updateTaskCount(const uint32 uuid, const uint32 actioncode, const uint32 pcate, const uint32 pframe);

	void UnloadTasks(const uint32 uuid);

	//��������ɵ��������.��������������, ����Ҫˢ�������б�.
	int update(const uint32 uuid, const bool syncall = false);

	int update_(const uint32 uuid, const bool syncall = false);

	void LoadUseTasks(const uint32 uuid);

	void SyncTaskRecordToDB(const uint32 uuid);

private:
	Tasks_type m_Tasks;
};

#endif //_TASKSYSTEM_H_

