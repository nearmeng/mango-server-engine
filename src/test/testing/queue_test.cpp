#include "gtest.h"
#include "queue/async_msg_queue.h"

TEST(QUEUE_TEST, RING_BUFFER)
{
	int32_t nRetCode = 0;
	CRingBuffer buffer;

	nRetCode = buffer.init();
	ASSERT_TRUE(nRetCode);

	char data[10] = "hello";
	nRetCode = buffer.put_data(data, sizeof(data));
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(buffer.get_used_space() == sizeof(data));
	ASSERT_TRUE(buffer.get_left_space() == MAX_RING_BUFFER_SIZE - sizeof(data));

	char szGetData[10];
	nRetCode = buffer.get_data(szGetData, sizeof(data));
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(buffer.get_used_space() == 0);
	ASSERT_TRUE(buffer.get_left_space() == MAX_RING_BUFFER_SIZE);

	printf("get data %s\n", szGetData);
	buffer.uninit();
}

TEST(QUEUE_TEST, ASYNC_MSG_QUEUE)
{
	int32_t nRetCode = 0;
	CShmMsgQueue MsgQueue;

	nRetCode = MsgQueue.init(FALSE);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(MsgQueue.has_msg() == FALSE);

	char szMsg[10] = "hello";
	nRetCode = MsgQueue.push_msg(1234, szMsg, sizeof(szMsg));
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(MsgQueue.has_msg() == TRUE);

	int32_t nSrcAddr = 0;
	size_t dwSize = 0;
	nRetCode = MsgQueue.pop_msg(nSrcAddr, szMsg, dwSize);
	ASSERT_TRUE(nRetCode);
	ASSERT_TRUE(MsgQueue.has_msg() == FALSE);

	nRetCode = MsgQueue.uninit();
	ASSERT_TRUE(nRetCode);

	printf("pop msg is %s\n", szMsg);
}
