#ifndef _TBUS_WRAPPER_H_
#define _TBUS_WRAPPER_H_

//for template 8.8.8.8
#define GET_SERVER_AREA(__addr__)		((__addr__) & 0xFF)
#define GET_SERVER_ZONE(__addr__)		((__addr__ >> 8) & 0xFF)
#define GET_SERVER_TYPE(__addr__)		((__addr__ >> 16) & 0xFF)
#define GET_SERVER_INST(__addr__)		((__addr__ >> 24) & 0xFF)

typedef BOOL(*TBUS_MSG_HANDLE)(const char* pBuff, size_t nSize, int32_t nSrcAddr);

BOOL tbus_system_init(int32_t nTbusAddr, int32_t nTbusHandle, TBUS_MSG_HANDLE pMsgHandle, BOOL bResume);
BOOL tbus_system_uninit();

int32_t tbus_get_addr(void);
inline int32_t tbus_get_area(int32_t nTbusAddr);
inline int32_t tbus_get_zone(int32_t nTbusAddr);
inline int32_t tbus_get_type(int32_t nTbusAddr);
inline int32_t tbus_get_inst(int32_t nTbusAddr);

const char* tbus_get_str(int32_t nTbusAddr);
int32_t tbus_make_addr(int32_t nArea, int32_t nZone, int32_t nType, int32_t nInst);

BOOL tbus_send_data(int32_t nDstTbusAddr, const char* pBuff, int32_t nDataLen);
BOOL tbus_sendv_data(int32_t nDstTbusAddr, const struct iovec* pVec, const int32_t nVecCount);
BOOL tbus_recv_data(void);
BOOL tbus_clear_data(void);

inline int32_t tbus_get_area(int32_t nTbusAddr)
{
	return GET_SERVER_AREA(nTbusAddr);
}

inline int32_t tbus_get_zone(int32_t nTbusAddr)
{
	return GET_SERVER_ZONE(nTbusAddr);
}

inline int32_t tbus_get_type(int32_t nTbusAddr)
{
	return GET_SERVER_TYPE(nTbusAddr);
}

inline int32_t tbus_get_inst(int32_t nTbusAddr)
{
	return GET_SERVER_INST(nTbusAddr);
}

#endif