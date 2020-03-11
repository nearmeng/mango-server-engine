#ifndef _HASH_H_
#define _HASH_H_

extern uint64_t hash_file_name(const char* pcszFileName);
extern uint32_t hash_string(const char* pcszString);
extern int32_t jump_consist_hash(int32_t nNodeCount, uint64_t qwKey);

#endif	// _HASH_H_