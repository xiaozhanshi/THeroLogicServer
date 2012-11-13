
#ifndef GET_OPT_H__
#define GET_OPT_H__

#include <string.h>
#include "getopt.h"

namespace
{
	const static unsigned char s_bySize = 20;	// 参数选项长度
}

// 命令行参数处理类
class Get_Opt
{
public:	
	Get_Opt(){}
	~Get_Opt(){}

	// 设置参数选项
	void SetOptions(int argc, char* argv[],const char * szOptions){
		m_argc = argc;
		m_argv = argv;
		strcpy(m_szOptions,szOptions);
	}
	// 获得当前选项
	int	GetOpt(){
		return getopt(m_argc, m_argv, m_szOptions);
	}
	// 获得当前选项的值
	char* GetCurValue(){
		return optarg;
	}

protected:
	int		m_argc;
	char**	m_argv;
	char	m_szOptions[s_bySize];
};














#endif

