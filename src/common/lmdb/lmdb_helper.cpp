#include "stdafx.h"
#include "file/ifile.h"

#include "lmdb/lmdb_helper.h"

BOOL lmdb_env_create(MDB_env **env, unsigned int nFlags, const char* pcszPath, int32_t nSizeMb)
{
    int32_t nRetCode;

	LOG_PROCESS_ERROR(env);
	LOG_PROCESS_ERROR(pcszPath);
	LOG_PROCESS_ERROR(nSizeMb > 0);

	nRetCode = create_dir(pcszPath);
	LOG_PROCESS_ERROR(nRetCode);

    nRetCode = mdb_env_create(env);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_env_set_mapsize(*env, nSizeMb * 1024 * 1024);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_env_set_maxdbs(*env, 1024);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_env_open(*env, pcszPath, nFlags, 0664);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

void lmdb_env_destroy(MDB_env * env)
{
	LOG_PROCESS_ERROR(env);

    mdb_env_close(env);
Exit0:
	return;
}


BOOL lmdb_db_create(MDB_dbi *dbi, MDB_env *env, const char* db_name)
{
	int32_t nRetCode = 0;
    MDB_txn *txn;

    nRetCode = mdb_txn_begin(env, NULL, 0, &txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_dbi_open(txn, db_name, MDB_CREATE, dbi);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_txn_commit(txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

int32_t lmdb_env_get_mapsize(MDB_env *env)
{
	int32_t nRetCode = 0;
    MDB_envinfo info;

    nRetCode = mdb_env_info(env, &info);
	LOG_PROCESS_ERROR(nRetCode == 0);

    return info.me_mapsize;
Exit0:
	return 0;
}

BOOL lmdb_gets(MDB_env *env, MDB_dbi dbi, char* keystr, MDB_val* val)
{
	int32_t nRetCode = 0;
    MDB_txn *txn = NULL;
    MDB_val key = { 0 };

	key.mv_size = strlen(keystr);
	key.mv_data = keystr;

	nRetCode = mdb_txn_begin(env, NULL, 0, &txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_get(txn, dbi, &key, val);
    switch (nRetCode)
    {
    case 0:
        break;
    case MDB_NOTFOUND:
        val->mv_data = NULL;
        val->mv_size = 0;
        break;
    default:
		LOG_PROCESS_ERROR(FALSE);
    }

    nRetCode = mdb_txn_commit(txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL lmdb_gets_int(MDB_env *env, MDB_dbi dbi, char* keystr, int *out)
{
	int32_t nRetCode = 0;
    MDB_val val;

    nRetCode = lmdb_gets(env, dbi, keystr, &val);
	LOG_PROCESS_ERROR(nRetCode);

    if (val.mv_data)
    {
        *out = *(int*)val.mv_data;
        return TRUE;
    }

Exit0:
    return FALSE;
}

BOOL lmdb_puts(MDB_env *env, MDB_dbi dbi, char* pKeyStr, MDB_val* pValue)
{
	int32_t nRetCode = 0;
	MDB_val key;
    MDB_txn *txn;
    
	nRetCode = mdb_txn_begin(env, NULL, 0, &txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

	key.mv_size = strlen(pKeyStr);
	key.mv_data = pKeyStr;

    nRetCode = mdb_put(txn, dbi, &key, pValue, 0);
	LOG_PROCESS_ERROR(nRetCode == 0);
    
	nRetCode = mdb_txn_commit(txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL lmdb_puts_int(MDB_env *env, MDB_dbi dbi, char* pKeyStr, int32_t nValue)
{
	int32_t nRetCode = 0;
    MDB_val val;

	val.mv_size = sizeof(int);
	val.mv_data = &nValue;

	nRetCode = lmdb_puts(env, dbi, pKeyStr, &val);
	LOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
    return FALSE;
}

BOOL lmdb_delete_first(MDB_env *env, MDB_dbi dbi, MDB_val *k, MDB_val *v)
{
	int32_t nRetCode = 0;
    MDB_cursor* curs;
    MDB_txn *txn;

    nRetCode = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_cursor_open(txn, dbi, &curs);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_cursor_get(curs, k, v, MDB_FIRST);
	LOG_PROCESS_ERROR(nRetCode == 0 || nRetCode == MDB_NOTFOUND);

    nRetCode = mdb_del(txn, dbi, k, v);
	LOG_PROCESS_ERROR(nRetCode == 0);

    mdb_cursor_close(curs);

    nRetCode = mdb_txn_commit(txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
	return FALSE;
}

BOOL lmdb_delete_last(MDB_env *env, MDB_dbi dbi, MDB_val *k, MDB_val *v)
{
	int32_t nRetCode = 0;
    MDB_cursor* curs;
    MDB_txn *txn;

    nRetCode = mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_cursor_open(txn, dbi, &curs);
	LOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = mdb_cursor_get(curs, k, v, MDB_LAST);
	LOG_PROCESS_ERROR(nRetCode == 0 || nRetCode == MDB_NOTFOUND);

    nRetCode = mdb_del(txn, dbi, k, v);
	LOG_PROCESS_ERROR(nRetCode == 0);

    mdb_cursor_close(curs);

    nRetCode = mdb_txn_commit(txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

    return TRUE;
Exit0:
	return FALSE;
}

BOOL lmdb_drop_dbs(MDB_env *env, MDB_dbi dbs[], size_t ndbs)
{
	int32_t nRetCode = 0;
    MDB_txn *txn;

    nRetCode = mdb_txn_begin(env, NULL, 0, &txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

    for (int i = 0; i < ndbs; i++)
    {
        nRetCode = mdb_drop(txn, dbs[i], 1);
		LOG_CHECK_ERROR(nRetCode == 0);
    }

    nRetCode = mdb_txn_commit(txn);
	LOG_PROCESS_ERROR(nRetCode == 0);

    for (int i = 0; i < ndbs; i++)
        mdb_dbi_close(env, dbs[i]);

	return TRUE;
Exit0:
	return FALSE;
}