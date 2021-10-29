#include "gtest/gtest.h"
#include "shm_basic_string.h"
#include "shm_test.h"
using ShmString = ShmBasicString<char>;
namespace std {
template <>
struct hash<ShmString> {
  std::size_t operator()(const ShmString& str) const {
    std::size_t hash = 5381;
    for (auto one_char : str) {
      hash = ((hash << 5) + hash) + std::size_t(one_char); /* hash * 33 + c */
    }
    return hash;
  }
};
}  // namespace std
class ShmStringTest : public ::testing::Test {
 public:
  static void SetUpTestSuite() {
    ShmConfig::SetEnable(true);
    auto shm_stat = ShmCreate(kShmKeyNumber, kShmIdentNumber, false);
    ASSERT_TRUE(shm_stat == ShmStat::kCreate);
  }
  static void TearDownTestSuite() { ShmDestroy(); }
};

TEST_F(ShmStringTest, ctor) {
  ShmString a;
  ShmString b;
  ASSERT_EQ(a.size(), 0);
  ASSERT_EQ(b.size(), 0);
  a = b;
  ASSERT_EQ(a.size(), 0);
  ASSERT_EQ(b.size(), 0);
  a = "123";
  b = "";
  ASSERT_EQ(a, "123");
  ASSERT_EQ(b, "");
  b = a;
  ASSERT_EQ(a, b);

  std::cout << a << std::endl;
  std::cout << b << std::endl;

  a += b;
  ASSERT_EQ(a, "123123");
  b.append(a);
  ASSERT_EQ(b, "123123123");
  b = std::move(a);
  ASSERT_EQ(b, "123123");
  a.clear();
  ASSERT_EQ(a, "");
  a.swap(b);
  ASSERT_EQ(b, "");
  ASSERT_EQ(a, "123123");
  a = a.substr(3);
  a.shrink_to_fit();
  ASSERT_EQ(a, "123");
  ASSERT_EQ(a.back(), '3');
  a.pop_back();
  ASSERT_EQ(a, "12");
  a.append("3");
  ASSERT_EQ(a, "123");
  ASSERT_EQ(a.front(), '1');
  b = a;
  for (auto& one_char : a) {
    one_char += 1;
  }
  ASSERT_EQ(a, "234");
  for (std::size_t i = 0; i < 10; i++) {
    a += b;
  }
  ASSERT_EQ(a.size(), 33);
  a.shrink_to_fit();
  ASSERT_EQ(a.capacity(), a.size());
}

TEST_F(ShmStringTest, stl) {
  ShmString a("111");
  ShmString b("222");
  std::unordered_map<ShmString, int> temp_hash_map;
  temp_hash_map[a] = 1;
  temp_hash_map[b] = 2;
  for (auto one_item : temp_hash_map) {
    std::cout << " key " << one_item.first << " has value " << one_item.second << std::endl;
  }

  std::map<ShmString, int> temp__map;
  temp__map[a] = 1;
  temp__map[b] = 2;
  for (auto one_item : temp__map) {
    std::cout << " key " << one_item.first << " has value " << one_item.second << std::endl;
  }
  ASSERT_TRUE(a < b);

}

