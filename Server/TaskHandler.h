#ifndef _ANT_WARS_SERVER_TASKHANDLER_H_
#define _ANT_WARS_SERVER_TASKHANDLER_H_

#include "define.h"

#include <vector>
#include <string>
#include <map>
using namespace std;

typedef struct _TaskDefine
{
	//uint32 mtype;			//��������:��������(1��������,2�ճ�����)
	uint32 smsid;			//������ID = �û�������ID
	uint32 actioncode;		//��������
	uint32 pcate;			//�����������
	uint32 pframe;			//��������С�� //actioncode. pcate. pframe 
	uint32 times;			//��Ҫ��ɵĴ���.

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

	uint32 id;				//�û�������id
	uint32 taskid;			//������ID
	uint32 count;			//��ɴ���
	uint32 target;			//��Ҫ��ɴ���

	uint32 actioncode_;		//�������
	uint32 pcate_;			//��������
	uint32 pframe_;			//��������
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

	bool   update;			//���������Ƿ񱻸���
	bool   isComplete;		//�����Ƿ����.

	uint32 id;				//�û�����id
	uint32 subid;			//�������id,
	uint32 status_;			//����״̬ 
	string timeTag;			//���ʱ��
	//uint32 mtype;			//��������:��������(1��������,2�ճ�����)

	vector<SubTaskInfo > subTasks;
} ;

typedef vector<TaskInfo > TaskInfos_type;

typedef map<uint32, TaskInfos_type > Tasks_type;

class ITaskHandler
{
public:
	virtual ~ITaskHandler() {}
public:
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
	virtual void OnUseProp(const uint32 uuid, const int32 props) = 0;

	/// �����������.
	/**
	*
	* @param uuid ��ɫID.
	*
	* @param pcate �������ID.
	*
	* @param pframe ����ID.
	*
	* @return ��
	*
	* @node ����ͬʱ���¶�����������ʵʱ���»������ݲ�������������˵Ļ�������ͬ�����ݿ������ ��
	*
	*/
	virtual void OnTaskEvent(const uint32 uuid, const int32 taskcode, const int32 pcate, const int32 pframe) = 0;


	/// ������ս����ص��������.
	/**
	*
	* @param uuid ��ɫID.
	*
	* @param winner �Ƿ�ȡ����ʤ��.
	*
	* @param roomtype ս�����з��������.
	*
	* @node ����ͬʱ���¶�����������ʵʱ���»������ݲ�������������˵Ļ�������ͬ�����ݿ������ ��
	*
	* @return ��
	*
	*/
	//virtual void OnGameOver(const uint32 uuid, const bool winner, const int32 roomtype) = 0;

	/// ж���û�����������.������ͬ���Ļ�������ݿ�
	/**
	*
	* @param uuid ��ɫID.
	*
	* @return ��
	*
	*/
	virtual void unloadTasks(const uint32 uuid) = 0;

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
	virtual void refreshTask(const uint32 uuid) = 0;
};

ITaskHandler * getTaskHandler();

#endif


