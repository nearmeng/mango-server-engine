# 共享内存用法

## 共享内存管理

    ShmHeader* ShmCreate(key_t key, size_t size, bool is_resume); // 创建
    bool ShmDestroy(ShmHeader* header); // 删除


## 内存分配器

    static ShmArenaType* ShmArena::init(void* memory, size_t size); // 初始化
    static ShmArenaType* ShmArena::resume_init(void* memory, size_t size); // resume 初始化
    static auto ShmArena::allocator(ShmArenaType& t); // 获取内存分配器

## 内存恢复机制

1. 执行`ShmArena::resume_init`时触发内存恢复机制
1. 对所有已经分配的对象执行默认构造函数
1. 如果该对象拥有`resume_init`函数, 则调用并检查返回值, 如果执行失败(返回`false`), 则`ShmArena::resume_init`执行失败

### 调试

使用gtest death test, 对要支持共享内存的类进行单元测试.(可参考已有例子)

