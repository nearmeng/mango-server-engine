#ifndef _GS_USER_MODULE_MSG_HANDLER_H_
#define _GS_USER_MODULE_MSG_HANDLER_H_

struct USER;

BOOL do_g2c_sync_role_list(uint64_t qwSessionID, USER* pUser);
BOOL do_g2c_sync_role_data(uint64_t qwSessionID, CRole* pRole);

#endif