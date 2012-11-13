#ifndef  __SINGLETON_CLASS_H__
#define  __SINGLETON_CLASS_H__

// SingletonClass.h
#include <assert.h>
#include <memory>
#include "NonCopyableClass.h"
#include <cstdlib>

// �Զ��ͷ�
// ����ʹ�ã�������ʹ�û����ͷ�����˳���ϲ�������
template< typename TYPE, typename REFTYPE = TYPE >
class AutoDeleteSingleton : private NonCopyableClass
{
public:
	static  REFTYPE &
	Instance()
	{
		static  TYPE  s_SingleObj;
		return  s_SingleObj;
	}

protected:
	AutoDeleteSingleton(){};
	virtual ~AutoDeleteSingleton(){};
};


// �ֶ�ִ���ͷ�
template< typename TYPE, typename PTYPE = TYPE >
class ManualDeleteSingleton : private NonCopyableClass
{
public:
	static PTYPE *
	Instance()
	{
		// �������ͷź���ʹ��
		assert( ! s_bDestroy );
		if( s_bDestroy )
			return ( NULL );

		if( s_pSingleObj == NULL )
		{
			// ����ڶ��߳���ʹ����Ҫ����ͬ��
			if( s_pSingleObj == NULL )
				s_pSingleObj = new TYPE();
		}

		return s_pSingleObj;
	}

	static void
	Destroy()
	{
		if( s_pSingleObj != NULL )
		{
			delete (s_pSingleObj);
			s_pSingleObj = NULL;
			s_bDestroy = true;
		}
	}

protected:
	static bool      s_bDestroy;
	static PTYPE    *s_pSingleObj;

protected:
	ManualDeleteSingleton(){};
	virtual ~ManualDeleteSingleton(){};
};

template< typename TYPE, typename PTYPE> PTYPE * ManualDeleteSingleton<TYPE, PTYPE>::s_pSingleObj  = NULL;
template< typename TYPE, typename PTYPE> bool    ManualDeleteSingleton<TYPE, PTYPE>::s_bDestroy    = false;


#endif // __SINGLETON_CLASS_H__
