#ifndef _TBUS_WRAPPER_H_
#define _TBUS_WRAPPER_H_

typedef BOOL(*TBUS_MSG_HANDLE)(const char* pBuff, size_t nSize, int32_t nSrcAddr);

BOOL tbus_system_init(int32_t nTbusAddr, int32_t nTbusHandle, TBUS_MSG_HANDLE pMsgHandle, BOOL bResume);
BOOL tbus_system_uninit();

int32_t tbus_get_addr(void);
int32_t tbus_get_area(int32_t nTbusAddr);
int32_t tbus_get_zone(int32_t nTbusAddr);
int32_t tbus_get_type(int32_t nTbusAddr);
int32_t tbus_get_inst(int32_t nTbusAddr);

const char* tbus_get_str(int32_t nTbusAddr);
int32_t tbus_make_addr(int32_t nArea, int32_t nZone, int32_t nType, int32_t nInst);

BOOL tbus_send_data(int32_t nDstTbusAddr, const char* pBuff, int32_t nDataLen);
BOOL tbus_sendv_data(int32_t nDstTbusAddr, const struct iovec* pVec, const int32_t nVecCount);
BOOL tbus_recv_data(void);
BOOL tbus_clear_data(void);

#endif