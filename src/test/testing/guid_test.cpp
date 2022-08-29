#include "gtest.h"
#include "guid/guid.h"

TEST(GUID_TEST, COMMON_TEST)
{
	int32_t nRetCode = 0;
	int32_t nType = 1;
	
	uint64_t id = guid_alloc(nType);
	ASSERT_TRUE(id > 0);
}
