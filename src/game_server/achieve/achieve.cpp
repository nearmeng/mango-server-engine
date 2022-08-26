#include "stdafx.h"
#include "achieve/achieve.h"

#include "role/role_module.h"

#include "protocol/proto_msgid.pb.h"
#include "protocol/proto_head.pb.h"

#include "great_world_base/gamespace/gamespace_api.h"

class GameSpaceAdapter : public gw::game::IGameSpaceAdapter
{
	virtual void Init(uint64_t& gapp_id, gw::GWString& config_path)
	{
	}
	virtual gw::ITimeMgr* CreateTimerMgr()
	{
		return NULL;
	}
	virtual void DestoryTimerMgr(gw::ITimeMgr* mgr)
	{
	}
	virtual void SendMessageToCell(uint64_t gapp_id, const gw::StringSlice& ss)
	{
	}

	void SendMessageToCellMgr(const gw::StringSlice& ss)
	{
	}

	virtual uint8_t* resume_buffer()
	{
		return NULL;
	}

	virtual void OnCreateSpace(const gw::AddCellMsg& msg)
	{
	}
	virtual void OnDestroySpace(uint64_t space_id, const gw::GWString& tag)
	{
	}

	virtual void OnUpdateLayout(const gw::LayoutMsg& msg, bool add_cell)
	{
	}

	virtual void AllCellCreated(const gw::AllCellCreatedMsg& msg)
	{
	}

	virtual gw::game::ICellEntityAdapter* OnCreateGhost(uint64_t eid, gw::IMemoryIStream& is)
	{
		return NULL;
	}
	virtual gw::game::ICellEntityAdapter* OnTeleportIn(uint64_t eid, gw::IMemoryIStream& is)
	{
		return NULL;
	}

	virtual void OnBeginObserver(const gw::GWVector<uint64_t>& self, uint64_t other)
	{
	}

	virtual void OnEndObserver(uint64_t self, uint64_t other)
	{
	}
	virtual gw::GWVector<gw::game::AoIDiffProp>&& GetDiffProps(uint64_t, uint32_t)
	{
		gw::GWVector<gw::game::AoIDiffProp> a;
		return std::move(a);
	}
	virtual gw::IMemoryOStream* NotifyOS()
	{
		return NULL;
	}

	virtual float GetLoad()
	{
		return 0;
	}
	virtual float GetMemsize()
	{
		return 0;
	}

	virtual gw::ILogger* GetLogger()
	{
		return NULL;
	}
	virtual uint64_t GetTimeMs()
	{
		return 0;
	}
};

BOOL CAchieve::init(void* pOwner)
{
	int32_t nRetCode = 0;
	GameSpaceAdapter imp;

    LOG_PROCESS_ERROR(pOwner);

    m_pOwner = pOwner;
    m_nAchieveID = 0;

    DBG("role sub module achieve is init");

	nRetCode = gw::game::GameSpaceApi::Init(&imp);
	LOG_PROCESS_ERROR(nRetCode);
	
    return TRUE;
Exit0:
    return FALSE;
}

BOOL CAchieve::uninit(void)
{
    INF("role sub module achieve is uninit");

	gw::game::GameSpaceApi::UnInit(false);

    return TRUE;
}
    
void CAchieve::on_event_sync_data()
{
    INF("achieve on event sync data");
}