#ifndef _LMDB_HELPERS_H_
#define _LMDB_HELPERS_H_

#include "lmdb.h"

BOOL lmdb_env_create(MDB_env **env, unsigned int flags, const char* pcszPath, int32_t nSizeMb);
void lmdb_env_destroy(MDB_env * env);

BOOL lmdb_db_create(MDB_dbi *dbi, MDB_env *env, const char* db_name);
BOOL lmdb_drop_dbs(MDB_env *env, MDB_dbi dbs[], size_t ndbs);

BOOL lmdb_gets(MDB_env *env, MDB_dbi dbi, const char* keystr, MDB_val* val);
BOOL lmdb_puts(MDB_txn *txn, MDB_dbi dbi, const char* keystr, int32_t in);
BOOL lmdb_gets_int(MDB_env *env, MDB_dbi dbi, const char* keystr, int32_t *out);
BOOL lmdb_puts_int(MDB_env *env, MDB_dbi dbi, const char* keystr, int32_t in);
BOOL lmdb_delete_first(MDB_env *env, MDB_dbi dbi, MDB_val *k, MDB_val *v);
BOOL lmdb_delete_last(MDB_env *env, MDB_dbi dbi, MDB_val *k, MDB_val *v);
int32_t lmdb_env_get_mapsize(MDB_env *env);

#endif
