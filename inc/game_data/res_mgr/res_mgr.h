#ifndef _RES_MGR_H_
#define _RES_MGR_H_

#include "define/str_def.h"
#include "define/res_def.h"

#include "file/res_file.h"
#include "time/time_mgr.h"

#include "global_res_mgr.h"

#define SKIP_ROW_COUNT 4

//tolua_begin

struct RES_BASE
{
    uint32_t    nID;
    char        szName[COMMON_NAME_LEN];

    inline BOOL post_readline(void) { return TRUE; };       // trigger after read line
    inline BOOL post_check(void) { return TRUE; };        // trigger after all the res data are loaded
};

//tolua_end

template <class T>
class CResMgr
{
public:
    CResMgr();
    virtual ~CResMgr();

    inline static CResMgr& instance(void);

    virtual BOOL init(const char* pcszConfigName, const char* pcszPath, 
            const char* pcszObjName, BOOL bResume);
    virtual BOOL uninit(void);

    virtual BOOL reload(BOOL bForce);
    virtual BOOL post_check(void);

    const T* get_res(uint32_t nID);
    int32_t get_res_count(void);
    int32_t get_res_size(void);

    T* get_first_res(void);
    T* get_next_res(void);

    inline uint32_t get_crc(void);
    inline uint32_t get_file_size(void);
    inline uint64_t get_last_load_time(void);

private:
    BOOL _init_res_file(CResFile& oResFile);
    BOOL _read_from_file(CResFile& oResFile, BOOL bForce);
    BOOL _read_line(CResFile& oResFile, int32_t nRowIndex, T* data);
    BOOL _set_line_data(CResFile& oResFile, int32_t nRowIndex, T* data);
	
    struct TRAVERSE_RES
	{
		BOOL operator()(uint32_t nID, T* pResData);
	};

private:
    static CResMgr<T>           ms_Instance;

    CShmObjectPool<T, uint32_t>  m_ResPool;
    char                        m_szConfigName[COMMON_NAME_LEN];
    char                        m_szObjName[COMMON_NAME_LEN];
    char                        m_szPath[COMMON_NAME_LEN];
    uint32_t                    m_dwCRC;
    uint32_t                    m_dwFileSize;
    uint64_t                    m_qwLastLoadTick;

    friend class CGlobalResMgr;
};

template <class T>
CResMgr<T>::CResMgr()
{
    m_szConfigName[0] = 0;
    m_szObjName[0] = 0;
    m_szPath[0] = 0;
    m_dwCRC = 0;
    m_dwFileSize = 0;
    m_qwLastLoadTick = 0;

    CGlobalResMgr::instance().add_res_mgr(typeid(T).name(), this);
}

template <class T>
CResMgr<T>::~CResMgr()
{

}

template <class T>
CResMgr<T> CResMgr<T>::ms_Instance;

template <class T>
inline CResMgr<T>& CResMgr<T>::instance(void)
{
    return ms_Instance;
}

template <class T>
BOOL CResMgr<T>::init(const char* pcszConfigName, const char* pcszPath,
                                const char* pcszObjName, BOOL bResume)
{
    int32_t nRetCode = 0;
    CResFile oResFile;

    LOG_PROCESS_ERROR(pcszConfigName);
    LOG_PROCESS_ERROR(pcszPath);
    LOG_PROCESS_ERROR(pcszObjName);

    strxcpy(m_szConfigName, pcszConfigName, sizeof(m_szConfigName));
    strxcpy(m_szPath, pcszPath, sizeof(m_szPath));
    strxcpy(m_szObjName, pcszObjName, sizeof(m_szObjName));

    nRetCode = _init_res_file(oResFile);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_ResPool.init(stdRes, oResFile.get_row_count(), bResume);
    LOG_PROCESS_ERROR(nRetCode);

    if (!bResume)
    {
        nRetCode = _read_from_file(oResFile, TRUE);
        LOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        m_dwCRC = oResFile.get_crc();
        m_dwFileSize = oResFile.get_file_size();
        m_qwLastLoadTick = CTimeMgr::instance().get_server_tick();
    }

    nRetCode = oResFile.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return TRUE;
Exit0:
    nRetCode = oResFile.uninit();
    LOG_CHECK_ERROR(nRetCode);

    return FALSE;
}
    
template <class T>
BOOL CResMgr<T>::_init_res_file(CResFile& oResFile)
{
    int32_t nRetCode = 0;
    char szFileName[COMMON_NAME_LEN];
    int32_t nResMode = CGlobalResMgr::instance().get_res_mode();
    
    switch (nResMode)
    {
    case rlmTab:
        snprintf(szFileName, COMMON_NAME_LEN, "%s/%s.txt", m_szPath, m_szConfigName);
        nRetCode = oResFile.init(szFileName, '\t');
        LOG_PROCESS_ERROR(nRetCode);
        break;
    case rlmCsv:
        snprintf(szFileName, COMMON_NAME_LEN, "%s/%s.csv", m_szPath, m_szConfigName);
        nRetCode = oResFile.init(szFileName, ',');
        LOG_PROCESS_ERROR(nRetCode);
        break;
    default:
        LOG_PROCESS_ERROR(FALSE);
    }

    return TRUE;
Exit0:
    return FALSE;
}
    
template <class T>
BOOL CResMgr<T>::_read_from_file(CResFile& oResFile, BOOL bForce)
{
    int32_t nRetCode = 0;
    int32_t nRowIndex = 0;
    int32_t nLoadCount = 0;

    PROCESS_SUCCESS(!bForce && (m_dwCRC == oResFile.get_crc()));

    for (int32_t nRowIndex = SKIP_ROW_COUNT; nRowIndex < oResFile.get_row_count(); nRowIndex++)
    {
        T NewData;
        memset(&NewData, 0, sizeof(T));

        nRetCode = _read_line(oResFile, nRowIndex, &NewData);
        if (nRetCode)
        {
            nRetCode = NewData.post_readline();
            if (!nRetCode)
                WRN("failed to post read line at file %s row %d id %d name %s", 
                        m_szConfigName, nRowIndex, NewData.nID, NewData.szName);

            nRetCode = _set_line_data(oResFile, nRowIndex, &NewData);
            if (nRetCode)
            {
                nLoadCount++;
            }
            else
            {
                WRN("failed to set line data at file %s row %d id %d name %s",
                        m_szConfigName, nRowIndex, NewData.nID, NewData.szName);
            }
        }
        else
        {
            WRN("failed to read line at file %s row %d id %d name %s",
                m_szConfigName, nRowIndex, NewData.nID, NewData.szName);
        }
    }

    m_dwCRC = oResFile.get_crc();
    m_dwFileSize = oResFile.get_file_size();
    m_qwLastLoadTick = CTimeMgr::instance().get_server_tick();

    INF("loaded %d/%d res data from file %s crc %08X", nLoadCount, oResFile.get_row_count() - SKIP_ROW_COUNT, m_szConfigName, m_dwCRC);

Exit1:
    return TRUE;

Exit0:
    CRI("failed to read data in %s", m_szConfigName);
    return FALSE;
}

template <class T>
BOOL CResMgr<T>::_read_line(CResFile& oResFile, int32_t nRowIndex, T* data)
{
    int32_t nRetCode = 0;
    BOOL bAllSuccess = TRUE;
    CLuaScript& rLuaScript = CGlobalResMgr::instance().get_lua_script();

    LOG_PROCESS_ERROR(data);

    if (oResFile.is_empty_row(nRowIndex))
    {
        WRN("skipped empty row at file %s row %d", m_szConfigName, nRowIndex);
        return TRUE;
    }

    for (int32_t nColIndex = 0; nColIndex < oResFile.get_col_count(); nColIndex++)
    {
        char* pcszResStr = NULL;
        char* pcszEffectType = NULL;
        char* pcszColType = NULL;
        char* pcszColName = NULL;

        int32_t nColNameLen = 0;
        char* pcszArrayIndex = NULL;
        char szArrayIndex[COMMON_NAME_LEN] = { 0 };
        char szArrayName[COMMON_NAME_LEN] = { 0 };

        pcszResStr = oResFile.get_data(nColIndex, nRowIndex);
        if (pcszResStr == NULL)
            continue;

        if (*pcszResStr == '\0')
            continue;

        pcszEffectType = oResFile.get_data(nColIndex, 1);
        LOG_PROCESS_ERROR(pcszEffectType);
        
        if (*(uint32_t*)pcszEffectType != 'htob' && *(uint32_t*)pcszEffectType != 'vres')
            continue;

        pcszColType = oResFile.get_data(nColIndex, 0);
        LOG_PROCESS_ERROR(pcszColType);

        pcszColName = oResFile.get_data(nColIndex, 2);
        LOG_PROCESS_ERROR(pcszColName);

        nColNameLen = strnlen(pcszColName, COMMON_NAME_LEN);
        //array support
        pcszArrayIndex = pcszColName + nColNameLen - 1;
        if (*pcszArrayIndex == ']')
        {
            while (*pcszArrayIndex != '[' && pcszArrayIndex > pcszColName)
                pcszArrayIndex--;

            strxcpy(szArrayIndex, pcszArrayIndex + 1, pcszColName + nColNameLen - pcszArrayIndex - 1);
            strxcpy(szArrayName, pcszColName, pcszArrayIndex - pcszColName + 1);
            pcszColName = szArrayName;
        }

        if (strncmp(pcszColType, "int", sizeof("int")) == 0 ||
            strncmp(pcszColType, "int32", sizeof("int32")) == 0)
        {
            if (szArrayIndex[0] != 0)
            {
                nRetCode = rLuaScript.call_function("read_array_default", "sslo", pcszColName, szArrayIndex,
                    strtoll(pcszResStr, (char**)0, 10), data, m_szObjName);
            }
            else
            {
                nRetCode = rLuaScript.call_function("read_default", "slo", pcszColName,
                    strtoll(pcszResStr, (char**)0, 10), data, m_szObjName);
            }
        }
        else if (strncmp(pcszColType, "string", sizeof("string")) == 0)
        {
            if (szArrayIndex[0] != 0)
            {
                nRetCode = rLuaScript.call_function("read_array_default", "ssso", pcszColName, szArrayIndex,
                    pcszResStr, data, m_szObjName);
            }
            else
            {
                nRetCode = rLuaScript.call_function("read_default", "sso", pcszColName,
                    pcszResStr, data, m_szObjName);
            }
        }
        else if (strncmp(pcszColType, "lua", sizeof("lua")) == 0)
        {
            if (szArrayIndex[0] != 0)
            {
                nRetCode = rLuaScript.call_function("read_lua_array", "ssso", pcszColName, szArrayIndex,
                    pcszResStr, data, m_szObjName);
            }
            else
            {
                nRetCode = rLuaScript.call_function("read_lua", "sso", pcszColName,
                    pcszResStr, data, m_szObjName);
            }
        }
        else
            LOG_PROCESS_ERROR(FALSE);

        if (!nRetCode)
        {
            bAllSuccess = FALSE;
            WRN("faild to read line at file %s row %d col %s", m_szConfigName, nRowIndex + 1, col2str(nColIndex));
        }
    }

    PROCESS_ERROR(bAllSuccess);

    return TRUE;
Exit0:
    return FALSE;
}

template <class T>
BOOL CResMgr<T>::_set_line_data(CResFile& oResFile, int32_t nRowIndex, T* pSetRes)
{
    int32_t nRetCode = 0;
    LOG_PROCESS_ERROR(pSetRes->nID > 0);

    T* pPoolRes = m_ResPool.find_object(pSetRes->nID);

    if (pPoolRes == NULL)
    {
        pPoolRes = m_ResPool.new_object(pSetRes->nID);
        LOG_PROCESS_ERROR(pPoolRes);

        memcpy(pPoolRes, pSetRes, sizeof(T));

        INF("res mgr insert new line %d id %d name %s of file %s", nRowIndex, pSetRes->nID, pSetRes->szName, m_szConfigName);
    }
    else
    {
        if (memcmp(pPoolRes, pSetRes, sizeof(T)) != 0)
        {
            memcpy(pPoolRes, pSetRes, sizeof(T));

            INF("res mgr overwrite line %d id %d name %s of file %s", nRowIndex, pSetRes->nID, pSetRes->szName, m_szConfigName);
        }
    }

    return TRUE;
Exit0:
    return FALSE;
}

template <class T>
BOOL CResMgr<T>::uninit(void)
{
    int32_t nRetCode = 0;

    return TRUE;
Exit0:
    return FALSE;
}
    
template <class T>
BOOL CResMgr<T>::reload(BOOL bForce)
{
    CResFile oResFile;
    int32_t nRetCode = 0;

    nRetCode = _init_res_file(oResFile);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = _read_from_file(oResFile, bForce);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = oResFile.uninit();
    LOG_PROCESS_ERROR(nRetCode);

    return TRUE;
Exit0:
    return FALSE;
}

template<class T>
inline BOOL CResMgr<T>::TRAVERSE_RES::operator()(uint32_t nID, T * pResData)
{
    int32_t nRetCode = 0;

    nRetCode = pResData->post_check();
    if (!nRetCode)
         WRN("failed to post process id %d name %s", pResData->nID, pResData->szName);
    
    return TRUE;
}

template <class T>
BOOL CResMgr<T>::post_check(void)
{
    int32_t nRetCode = 0;
    TRAVERSE_RES traverse_res;

    m_ResPool.traverse(traverse_res);

    return TRUE;
Exit0:
    return FALSE;
}

template <class T>
const T* CResMgr<T>::get_res(uint32_t nID)
{
    return m_ResPool.find_object(nID);
}

template <class T>
int32_t CResMgr<T>::get_res_count(void)
{
    return m_ResPool.get_used_count();
}

template <class T>
int32_t CResMgr<T>::get_res_size(void)
{
    return sizeof(T);
}

template <class T>
T* CResMgr<T>::get_first_res(void)
{
    return m_ResPool.get_first_object();
}

template <class T>
T* CResMgr<T>::get_next_res(void)
{
    return m_ResPool.get_next_object();
}

template <class T>
inline uint32_t CResMgr<T>::get_crc(void)
{
    return m_dwCRC;
}

template <class T>
inline uint32_t CResMgr<T>::get_file_size(void)
{
    return m_dwFileSize;
}

template <class T>
inline uint64_t CResMgr<T>::get_last_load_time(void)
{
    return m_qwLastLoadTick;
}

#endif
