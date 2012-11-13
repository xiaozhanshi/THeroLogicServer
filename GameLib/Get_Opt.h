
#ifndef GET_OPT_H__
#define GET_OPT_H__

#include <string.h>
#include "getopt.h"

namespace
{
	const static unsigned char s_bySize = 20;	// ����ѡ���
}

// �����в���������
class Get_Opt
{
public:	
	Get_Opt(){}
	~Get_Opt(){}

	// ���ò���ѡ��
	void SetOptions(int argc, char* argv[],const char * szOptions){
		m_argc = argc;
		m_argv = argv;
		strcpy(m_szOptions,szOptions);
	}
	// ��õ�ǰѡ��
	int	GetOpt(){
		return getopt(m_argc, m_argv, m_szOptions);
	}
	// ��õ�ǰѡ���ֵ
	char* GetCurValue(){
		return optarg;
	}

protected:
	int		m_argc;
	char**	m_argv;
	char	m_szOptions[s_bySize];
};














#endif

