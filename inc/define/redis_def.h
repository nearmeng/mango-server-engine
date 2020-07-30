#ifndef _REDIS_DEF_H_
#define _REDIS_DEF_H_

#define  MAX_CMD_HANDLER_COUNT   (512)
#define  MAX_EVAL_HANDLER_COUNT  (512)
#define  MAX_LOOP_PER_TIMES      (1024)
#define  MAX_REDIS_CALLBACK_BUFFER_LEN  (256)
#define  MAX_MODEL_ARG_COUNT      (64)
#define  MAX_PRIMARY_KEY_LEN      (32)
#define  MAX_REDIS_USER_DATA_LEN  (128)
#define  MAX_REDIS_COMMAND_SIZE         (100 * 1024)
#define  MAX_REDIS_REPLY_BUFFER_SIZE    (1 * 1024 * 1024)
#define  MAX_REDIS_SCIRPT_LEN           (8 * 1024)

struct redisAsyncContext;

typedef void(*REDIS_CMD_CALLBACK)(struct redisReply*, const char*, size_t);

struct REDIS_ARG_DATA
{
    int32_t nArgc;
    char* pArgv[MAX_MODEL_ARG_COUNT];
    size_t dwArgvSize[MAX_MODEL_ARG_COUNT];
};


//tolua_begin

enum REDIS_CLI_STATE
{
    rcsInvalid,

    rcsConnecting,
    rcsConnected,
    rcsAuthing,
    rcsAuthed,
    rcsDisConnecting,
    rcsDisConnected,

    rcsTotal
};

enum REDIS_CMD_TYPE
{
    recInvalid,

    recTest,

    recTotal
};

//tolua_end

#endif
