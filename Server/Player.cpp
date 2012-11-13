
#include "Player.h"
#include "SocketHandler.h"
#include "GameCmd.h"
#include "log.h"
#include "GameServer.h"
#include <algorithm>
#include "PlayerMgr.h"
#include "PlatFunc.h"
//#include "GameData.h"


CPlayer::CPlayer(uint32 uuid, TcpHandler * pHandler,CGameServer* pServer)
: m_PlatForm(0)
,m_nMid(uuid)
,m_pServer(pServer)
,m_pSocketHandler(pHandler)
{
	m_UserTimer.SetTimeEventObj(this, TIMER_CHECK_SOMETHING);
	m_UserDBTimer.SetTimeEventObj(this, TIMER_WRITEDB_SOMETHING);
	m_istatus = 0;
	m_ibasew = 7;
	m_ibaseh = 7;
	m_PowA = 0;
	m_PowB = 0;
	m_PowC = 0;
	m_PowD = 0;
	m_PowE = 0;
}

CPlayer::~CPlayer(void)
{
	StopTimer();
	log_debug("user %d, free memory\n", m_nMid);
}

void CPlayer::OnUserLogoutServer()
{
	// �˳�ʱ��������
	StopTimer();
	StopDBWTimer();
}

void CPlayer::StartCheckFunc(int second)
{
	log_debug("start timer for player\n");
	m_UserTimer.StartTimer(second);
}

void CPlayer::StartDBWriteTimer(int second)
{
	log_debug("start timer for player\n");
	m_UserDBTimer.StartTimer(second);
}

void CPlayer::StopTimer()
{
	m_UserTimer.StopTimer();
}

void CPlayer::StopDBWTimer()
{
	m_UserDBTimer.StopTimer();
}

// ����û�ID
uint32 CPlayer::getID()const 
{ 
    return m_nMid;
}
TcpHandler* CPlayer::GetSocketHandler()
{
    return m_pSocketHandler;
}

// ��ɫ״̬
uint32  CPlayer::getStatus() 
{ 
    return m_nStatus; 
}

void CPlayer::setStatus( uint32 nStatus ) 
{ 
    m_nStatus = nStatus;
    g_GetPlayerMgr().SetUserState(m_nMid,nStatus);
}

// ������Ϣ
void CPlayer::SendMsg(NETOutputPacket * outPacket )
{
    if(m_pSocketHandler != NULL)
    {
        m_pSocketHandler->Send(outPacket);
    }
}

void CPlayer::CalculateLevel()
{

}

static const int s_nLevel[] = 
{
	480,	1200,	2160,	3360,	4880,	6720,	8880,	11360,	14160,	17280,
	26880,	41280,	60480,	84480,	113280,	148480,	190080,	238080,	292480,	353280,
	506613,	673280,	853280,	1046613,1253280,1473280,1706613,1953280,2213280,2486613,
	3019947,3585280,4182613,4811947,5473280,6166613,6891947,7649280,8438613,2147483647 
};

int CPlayer::CalcLevel( int nPoint )
{
	int nEnd;
	nEnd = sizeof(s_nLevel) / sizeof(int) - 1;

	if(nPoint < s_nLevel[0])
	{
		return 1;
	}
	else if( nPoint >= s_nLevel[nEnd] )
	{
		return nEnd + 2;
	}

	int i = 0, nBegin = 0;
	while( nEnd - nBegin > 1)
	{
		i = (nEnd - nBegin) / 2 + nBegin;
		if( nPoint < s_nLevel[i] )
		{
			nEnd = i;
		}
		else if( nPoint > s_nLevel[i] )
		{
			nBegin = i;
		}
		else
		{
			break;
		}
	}

	if( 1 == nEnd - nBegin )
	{
		i = nBegin;
	}

	return i + 2;
}

int CPlayer::ProcessOnTimerOut(int Timerid)
{
	log_debug("CPlayer on timer\n");
	switch (Timerid)
	{
	case TIMER_CHECK_SOMETHING:
		log_debug("player on check mechine\n");
		ProcessMechineRequire();
		StartCheckFunc(g_GetConfigMgr().getCheckTime());
		break;
	case TIMER_WRITEDB_SOMETHING:
		log_debug("player on write DB mechine\n");
		ProcessMechineWriteDB();
		StartDBWriteTimer(g_GetConfigMgr().getWriteDBTime());
		break;
	default:
		break;
	}
	return 0;
}

void CPlayer::ProcessMechineRequire()
{
	// ���ָ��Ա���Ƿ�������״̬
	if (m_userInstorMechine.status == 3)
	{
		int unixtime = GetUnixTime();
		if (unixtime >= m_userInstorMechine.endtime)
		{
			// �������, ��������
			m_userInstorMechine.endtime = 0;
			m_userInstorMechine.level += 1; 
			m_userInstorMechine.status = 4;

			// �������ݿ�
			bool ret = g_GetCDBOperator().UpdateUserMechineTable(m_userInstorMechine);
			if (!ret)
			{
				log_debug("the timeout update user instormechine error, user id is %d\n", getID());
			}
			NETOutputPacket outPacket;
			outPacket.Begin( COMMAND_MECHINE_UPNEXTLEVEL_COMPLETE );
			outPacket.WriteInt(m_userInstorMechine.Gid);
			outPacket.WriteInt(m_userInstorMechine.level);
			outPacket.End();
			SendMsg( &outPacket );
		}
	}

	// �������������м��
	int len = m_userBuyMechineList.size();
	for (int i = 0; i < len; i++)
	{
		User_Experiment_Mechine_t oneItem = m_userBuyMechineList[i];
		if (oneItem.status == 3)
		{
			int unixtime = GetUnixTime();
			if (unixtime >= oneItem.endtime)
			{
				// �������, ��������
				oneItem.endtime = 0;
				oneItem.level += 1; 
				oneItem.status = 4;

				// �������ݿ�
				bool ret = g_GetCDBOperator().UpdateUserMechineTable(oneItem);
				if (!ret)
				{
					log_debug("the timeout update user instormechine error, user id is %d\n", getID());
				}
				m_pServer->ChangeUserMechine_By_Pid(m_userBuyMechineList, oneItem);

				NETOutputPacket outPacket;
				outPacket.Begin( COMMAND_MECHINE_UPNEXTLEVEL_COMPLETE );
				outPacket.WriteInt(oneItem.Gid);
				outPacket.WriteInt(oneItem.level);
				outPacket.End();
				SendMsg( &outPacket );
			}
		}

		// �Ƿ��ڽ����в����Ƿ����鱨��
		MechineType_PcatePframe_T InfoMechinePcate = g_GetConfigMgr().getMechine_Info_Type();
		if (oneItem.status == 1 && InfoMechinePcate.pcate == oneItem.pcate)
		{
			int unixtime = GetUnixTime();
			if (unixtime >= oneItem.endtime)
			{
				// �������, �����һ���ռ���¼
				UserInfoMechineCollect_T UserInfoCollect;
				UserInfoCollect.Gid = oneItem.Gid;
				UserInfoCollect.Mid = getID();
				UserInfoCollect.LeftTime = 0;
				UserInfoCollect.pcate = oneItem.pcate;
				UserInfoCollect.pframe = oneItem.pframe;
				UserInfoCollect.Status = 1;				// 1-�����ռ�״̬��2-�ռ�������0-�ռ���ͣ
				bool rtime = m_pServer->getCollectTimeByCollItem(UserInfoCollect, oneItem.level);
				bool ret = g_GetCDBOperator().CheckUserInfoCollect(UserInfoCollect);//�����鱨���ռ���
				if (ret && rtime)
				{
					oneItem.endtime = 0;
					oneItem.status = 8;						// �����ռ�״̬, Ҳ������״̬

					// �������ݿ�
					bool ret = g_GetCDBOperator().UpdateUserMechineTable(oneItem);
					if (!ret)
					{
						log_debug("the timeout update user instormechine error, user id is %d\n", getID());
					}
					m_pServer->ChangeUserMechine_By_Pid(m_userBuyMechineList, oneItem);
					m_userInfoCollectList.push_back(UserInfoCollect);

					// ���ؽ������
					NETOutputPacket outPacket;
					outPacket.Begin( COMMAND_MECHINE_COMSTRUCT_REPLY );
					outPacket.WriteInt(oneItem.Gid);
					outPacket.WriteShort(oneItem.status);
					outPacket.End();
					SendMsg( &outPacket );
				}
			}
		}

		//����Ƿ�����Դ���������Ƿ��ڽ�����
		MechineType_PcatePframe_T ResourceMechinePcate = g_GetConfigMgr().getMechine_Resource_Type();
		if (1 == oneItem.status && ResourceMechinePcate.pcate == oneItem.pcate)
		{
			int unixtime = GetUnixTime();
			if (unixtime >= oneItem.endtime)
			{
				// �������, �����һ���ռ���¼
				ResourceProducTable_T UserRecord;
				UserRecord.Gid = oneItem.Gid;
				UserRecord.Mid = getID();
				UserRecord.Pcate = oneItem.pcate;
				UserRecord.Pframe = oneItem.pframe;
				UserRecord.Pmode = 1;
				UserRecord.Level = oneItem.level;
				UserRecord.State = 0;  //��Ϊ����״̬
				oneItem.status = 0;   //�����û���״̬Ҳ��Ϊ����״̬
				UserRecord.Usetime = 0;

				//�����û���Դ��������
				bool ret = g_GetCDBOperator().InsertUserResourceMechineRecord(UserRecord);
				if ( ret )
				{
					//�����û�����������ݿ�
					bool ret = g_GetCDBOperator().UpdateUserMechineTable(oneItem);
					if (!ret)
					{
						log_debug("the timeout update user resourcemechine error, user id is %d\n", getID());
					}
					m_pServer->ChangeUserMechine_By_Pid(m_userBuyMechineList, oneItem); //�ı�ϵͳ�û�������
					m_UserResourceProducTable_list.push_back(UserRecord);  //�����ݴ��vector������ȡ

					// ���ؽ������
					NETOutputPacket outPacket;
					outPacket.Begin( COMMAND_MECHINE_COMSTRUCT_REPLY );
					outPacket.WriteInt(oneItem.Gid);
					outPacket.WriteShort(oneItem.status);
					outPacket.End();
					SendMsg( &outPacket );
				}

			}
		}

		// �����û��ռ���
		//MechineType_PcatePframe_T InfoMechinePcate = g_GetConfigMgr().getMechine_Info_Type();
		if (oneItem.status != 1 && oneItem.pcate == InfoMechinePcate.pcate)
		{
			// �ж��Ƿ��Ѿ�����
			UserInfoMechineCollect_T UserInfoCollect;
			bool ret = m_pServer->getUserInfoMechineCollect_By_Pid(m_userInfoCollectList, UserInfoCollect, oneItem.Gid);

			if (!ret)
			{
				// ���м���
				UserInfoCollect.Gid = oneItem.Gid;
				UserInfoCollect.Mid = getID();
				bool ret = g_GetCDBOperator().CheckUserInfoCollect(UserInfoCollect);
				if (ret)
				{
					m_userInfoCollectList.push_back(UserInfoCollect);
				}
				else
				{
					log_error("the load collect item error\n");
				}
			}
		}

		//�����û���Դ�ռ���
		if (oneItem.status != 1 && oneItem.pcate == ResourceMechinePcate.pcate)
		{
			//�ж��Ƿ�Ӧ������
			ResourceProducTable_T ResourceCollect;
			bool ret = m_pServer->getUserResMechineCollect_By_Pid(m_UserResourceProducTable_list, ResourceCollect, oneItem.Gid);

			if (!ret)
			{
				ResourceCollect.Gid = oneItem.Gid;
				ResourceCollect.Mid = getID();
				bool ret = g_GetCDBOperator().InsertUserResourceMechineRecord(ResourceCollect);
				if (ret)
				{
					m_UserResourceProducTable_list.push_back(ResourceCollect);
				}
				else
				{
					log_error("the load resourcecollect item error\n");
				}
			}

		}
	}


	// ����鱨���ռ�
	int clen = m_userInfoCollectList.size();
	for (int i = 0; i < clen; i++)
	{
		if (m_userInfoCollectList[i].Status == 0 || m_userInfoCollectList[i].Status == 2)
		{
			// 0 - ��ͣ�ռ� 2 - �����ռ��ˣ����Ի�û�ռ�
			continue;
		}
		m_userInfoCollectList[i].LeftTime += g_GetConfigMgr().getCheckTime();
		int needTime = m_userInfoCollectList[i].NeedTime;
		int LeftTime = m_userInfoCollectList[i].LeftTime;
		m_userInfoCollectList[i].Status = 1;
		m_userInfoCollectList[i].isDirty = true;
		//g_GetCDBOperator().UpdateUserInfoMechineCollectTable(m_userInfoCollectList[i]);
		//m_pServer->ChangeUserInfoMechineCollect_By_Pid(m_userInfoCollectList, UserInfoMechineCollect_T & UserInfoCollect);
		User_Experiment_Mechine_t MechineItem;
		bool ret = m_pServer->getUserMechine_By_Pid(m_userBuyMechineList, MechineItem, m_userInfoCollectList[i].Gid);
		if (ret)
		{
			MechineItem.status = 8;
			m_pServer->ChangeUserMechine_By_Pid(m_userBuyMechineList, MechineItem);
		}
		if (LeftTime >= needTime)
		{
			bool ret = m_pServer->getUserMechine_By_Pid(m_userBuyMechineList, MechineItem, m_userInfoCollectList[i].Gid);
			if (ret)
			{
				MechineItem.status = 9;
				m_pServer->ChangeUserMechine_By_Pid(m_userBuyMechineList, MechineItem);
			}
			// �����ռ��ˣ����͸��ͻ���
			m_userInfoCollectList[i].Status = 2;
			//g_GetCDBOperator().UpdateUserInfoMechineCollectTable(m_userInfoCollectList[i]);
			m_userInfoCollectList[i].isDirty = true;
			int Gid = m_userInfoCollectList[i].Gid;
			NETOutputPacket outPacket;
			outPacket.Begin( COMMAND_MECHINE_INFO_CANCOLLECT_REPLY );
			outPacket.WriteInt(Gid);
			outPacket.WriteShort(0);
			outPacket.End();
			SendMsg( &outPacket );
		}
	}

    //�����Դ���ռ�
	int lens = m_UserResourceProducTable_list.size();
	for (int i = 0; i < lens; i++)
	{
		ResourceProducTable_T Item = m_UserResourceProducTable_list[i];
		if (Item.State != 12) // 12-������
		{
			continue;
		}
		//m_UserResourceProducTable_list[i].Usetime += g_GetConfigMgr().getCheckTime(); //�ۼ�ʱ��
		ResourceMechineProductype_T ResProducInfo;
		bool ret = m_pServer->getResourceMechine_By_pacte_pfram_level(Item.Pcate, Item.Pframe, Item.Pmode, ResProducInfo);
		int needtime = ResProducInfo.NeedTime;  // ������Ҫ��ʱ��
		int usetime = m_UserResourceProducTable_list[i].Usetime; // Ŀǰ�ۼƵ�ʱ��
		//m_UserResourceProducTable_list[i].isDirty = true;
		//m_userInfoCollectList[i].Status = 1;

		Item.Usetime = Item.Usetime + g_GetConfigMgr().getCheckTime();
		Item.State = 12;
		//Item.isDirty = true;
		
		
		User_Experiment_Mechine_t MechineItem;
		bool rest = m_pServer->getUserMechine_By_Pid(m_userBuyMechineList, MechineItem, m_UserResourceProducTable_list[i].Gid);
		if (rest)
		{
			MechineItem.status = 12;
			m_pServer->ChangeUserMechine_By_Pid(m_userBuyMechineList, MechineItem);

			//ͬʱ�����û���Դ������Ϣ��״̬��ʹ��ʱ�䣩
			m_pServer->ChangUserResMechineProduc(m_UserResourceProducTable_list, Item);
		}
		if (usetime >= needtime)
		{
			bool ret = m_pServer->getUserMechine_By_Pid(m_userBuyMechineList, MechineItem, m_UserResourceProducTable_list[i].Gid);
			if (ret)
			{
				MechineItem.status = 7;
				Item.State = 7;  // 7-��Դ���������
				m_pServer->ChangeUserMechine_By_Pid(m_userBuyMechineList, MechineItem);

				//ͬʱ�����û���Դ������Ϣ��״̬��ʹ��ʱ�䣩
				m_pServer->ChangUserResMechineProduc(m_UserResourceProducTable_list, Item);
			}

			m_UserResourceProducTable_list[i].State = 12;
			//g_GetCDBOperator().UpdateUserInfoMechineCollectTable(m_userInfoCollectList[i]);
			m_UserResourceProducTable_list[i].isDirty = true;
			int Gid = m_UserResourceProducTable_list[i].Gid;
			NETOutputPacket outPacket;
			outPacket.Begin( COMMAND_MECHINE_INFO_CANCOLLECT_REPLY );
			outPacket.WriteInt(Gid);
			outPacket.WriteShort(0);
			outPacket.End();
			SendMsg( &outPacket );
	    }
	}
}


void CPlayer::ProcessMechineWriteDB()
{
	int clen = m_userInfoCollectList.size();
	for (int i = 0; i < clen; i++)
	{
		if (m_userInfoCollectList[i].isDirty)
		{
			g_GetCDBOperator().UpdateUserInfoMechineCollectTable(m_userInfoCollectList[i]);
		}
	}

	int blen = m_userBuyMechineList.size();
	for (int i = 0; i < blen; i++)
	{
		if (m_userBuyMechineList[i].isDirty)
		{
			g_GetCDBOperator().UpdateUserMechineTable(m_userBuyMechineList[i]);
		}
	}

	if (m_userExperimentMechine.isDirty)
	{
		g_GetCDBOperator().UpdateUserMechineTable(m_userExperimentMechine);
	}

	if (m_userInstorMechine.isDirty)
	{
		g_GetCDBOperator().UpdateUserMechineTable(m_userInstorMechine);
	}

	int reslen = m_UserResourceProducTable_list.size();
	for (int i =0; i < reslen; i++)
	{
		if (m_UserResourceProducTable_list[i].isDirty)
		{
			g_GetCDBOperator().UpdateUserResMechineProducTable(m_UserResourceProducTable_list[i]);
		}
	}


}


void CPlayer::SetPow(int ptype, int pvalue)
{
	switch (ptype) 
	{
	case 1:
		m_PowA = pvalue;
		break;
	case 2:
		m_PowB = pvalue;
		break;
	case 3:
		m_PowC = pvalue;
		break;
	case 4:
		m_PowD = pvalue;
		break;
	case 5:
		m_PowE = pvalue;
		break;
	default:
		break;
	}
}

uint32	CPlayer::GetPow(int ptype)
{
	uint32 pvalue = 0;
	switch (ptype) 
	{
	case 1:
		pvalue = m_PowA;
		break;
	case 2:
		pvalue = m_PowB;
		break;
	case 3:
		pvalue = m_PowC;
		break;
	case 4:
		pvalue = m_PowD;
		break;
	case 5:
		pvalue = m_PowE;
		break;
	default:
		break;
	}

	return pvalue;
}