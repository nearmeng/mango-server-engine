#ifndef _REDIS_MSG_HANDLER_H_
#define _REDIS_MSG_HANDLER_H_

BOOL send_db_proxy_client_redis_rsp(int32_t nTbusAddr, int32_t nCmdID, uint32_t dwUserDataLen, 
                                const char* pUserData, const char* pPackedBuffer, int32_t nPackedSize);

#endif