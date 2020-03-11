#ifndef _ROUTER_HASH_H_
#define _ROUTER_HASH_H_

BOOL router_hash_init(void);

BOOL router_hash_uninit(void);

int32_t router_hash_get_node(int32_t nHashType, int32_t nServiceType, uint64_t qwRouterID, BOOL bCanCache);

int32_t router_hash_get_hash_type(int32_t nServiceType);

#endif