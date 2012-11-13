#include <fstream>
#include "GameServer.h"
#include "ConfigMgr.h"
#include "DBOperator.h"
#include "log.h"
#include "timer.h"
#include "PlayerMgr.h"
#include "Get_Opt.h"
#include "cpush.h"
#include "qredis.h"
#include "GameCmd.h"

using namespace std;

// 1. 初始化 log
// 2. 读取配置文件
// 3. redis缓存服务器
// 4. 初始化网络
// 5. 初始化服务器
// 6. 释放资源

int main(int argc, char ** argv)
{
	srand((unsigned)time(NULL));

	// get port and loglv
	int nPort       = 0;
	int nlog_lv     = 7;
	char * configureFileName = NULL;
	char * logfileName = NULL;

	char szOptions[] = ":p:l:c:n:s:";
	Get_Opt cmdOpt;
	cmdOpt.SetOptions(argc, argv, szOptions);

	int nOption;
	while((nOption = cmdOpt.GetOpt()) != EOF)
	{
		switch ( nOption ) 
		{
		case 'p':		// 监听端口
			{
				nPort = atoi(cmdOpt.GetCurValue());
				if(nPort == 0)
					printf("port is empty \n");
			}
			break;
		case 'l':		// 日记的等级
			{
				nlog_lv = atoi(cmdOpt.GetCurValue());
				printf("start log level %d \n",nlog_lv);
			}
			break;
		case 'c':		// 配置文件
			{
				configureFileName = cmdOpt.GetCurValue();
				printf("configure file name is %s \n",configureFileName);
			}
			break;
		case 'n':		// 日记的名称
			{
				logfileName = cmdOpt.GetCurValue();
				printf("log file name is %s \n",logfileName);
			}
			break;
		default:
			printf("error argv %d \n",nOption);
			break;
		}
	}

	init_timer();

	char logname[100] = {0};
	sprintf(logname, "Log.%s", logfileName);
	init_log(logname, "./Log/");
	set_log_level(nlog_lv);

    CGameServer * pServer = new CGameServer();
    if( pServer== NULL)
    {
        log_error("new CGameServer fail \n");
        return 0;
    }

	g_GetConfigMgr().ReadConfig(configureFileName);

    if(g_GetCDBOperator().init() == false)
    {
        log_error("FAILED CONNECT TO DB!");
        return 0;
    }

	// redis缓存服务器
	g_qredis.InitQredis();

	// 初始化网络
    if( !pServer->InitSocket( nPort ) )
        return -1;

	// 初始化服务器
    if(!pServer->InitServer())
        return -1;

    g_GetPlayerMgr().Init(pServer);

	// 测试定时器 , 定时上报外网ip
	ServerConfig_T cfg = g_GetConfigMgr().getServerConfig();
	StressTimerObj  StressObj;
	log_debug("the server type is %d\n", cfg.type);
	if (cfg.type == SERVER_TYPE_MAIL)
	{
		StressObj.StartTimer(10);
	}
	
	// 连接苹果apn服务器
	APN_connect();

	// 加载数据库的常用表
	pServer->LoadSystemMsg();

	// 服务器运行中
    pServer->Run();
   
	// 服务器停止服务, 释放多种东西
    g_GetPlayerMgr().UpdateStress2DB(true);
    g_GetPlayerMgr().ShutDown();
    g_GetConfigMgr().ShutDown();
    g_GetCDBOperator().ShutDown();
    pServer->ShutDown();
	// 断开与苹果服务器的连接
	APN_closeConnect();

    delete pServer;

    return 0;
}


