#include "gtest.h"
#include "process_error/process_error.h"
#include "coroutine/coro_stackless.h"

class CTestCoro : public CCoroStackless
{
public:
    CTestCoro() {};
    ~CTestCoro() {};

    void set_start_arg(int32_t nTestValue);
    virtual CORO_STATE coro_process();

private:
    int32_t         m_nTestValue;
};

void CTestCoro::set_start_arg(int32_t nTestValue)
{
    m_nTestValue = nTestValue;
}

CORO_STATE CTestCoro::coro_process()
{
    int32_t nRetCode = 0;
    redisReply* pReply = NULL;

	CORO_BEGIN()

	printf("start process\n");

    CORO_YIELD()

    LOG_PROCESS_ERROR(get_coro_ret_code() == crcSuccess);
    LOG_PROCESS_ERROR(get_coro_reply().nReplyType == crtMsg);

    pReply = (redisReply*)(get_coro_reply().pReplyData);
	LOG_PROCESS_ERROR(pReply);

    CORO_END()

Exit0:
    return crsFailed;
}

TEST(CORO_TEST, NORMAL_TEST)
{
    CTestCoro* pCoro = CCoroStacklessMgr<CTestCoro>::instance().new_coro();
	ASSERT_TRUE(pCoro != NULL);

    pCoro->set_start_arg(100);

    int32_t nRetCode = CCoroStacklessMgr<CTestCoro>::instance().start_coro(pCoro);
    ASSERT_TRUE(nRetCode);

	pCoro = CCoroStacklessMgr<CTestCoro>::instance().find_coro(pCoro->get_coro_id());
	ASSERT_TRUE(pCoro != NULL);

	char szData[10] = "123";
	pCoro->set_coro_reply(crtMsg, szData, sizeof(szData));

	nRetCode = CCoroStacklessMgr<CTestCoro>::instance().resume_coro(pCoro);
	ASSERT_TRUE(nRetCode);
}