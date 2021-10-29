## 介绍
g6 大世界的基础库，其中分为 cell 和 gamespace 2 个部分

本代码库提供的能力有：
* space 的管理
* cell 的管理
* 多 cell 负载均衡
* base/cell 分离
* 分布式 AoI
* realghost 系统（迁移）
* cell间rpc(todo)

## 构建：
debug版本copy到g6
```
sh build_debug.sh
```
release版本copy到g6
```
sh build_release.sh
```

## 发布
```
sh publish.sh
```

## 目录结构
todo

## cellmgr
todo

## gamespace
todo

## 共享内存方案：
* 内部使用的 malloc 和 free 函数是外面传进来的
* 静态类注册到 managers.h，这个结构会以 buffer 的形式提供给应用层，resume 的时候把这个传递回来就好了
* 采用了替换虚表的方案，有虚函数的类必须使用 GWCLASS_DECL 描述

## 多线程方案
* 不通过共享内存进行通信，而是通过通信来共享内存
* debug 包增加各种线程环境检查

tips:
* 内部的stl以及对外提供的数据类型一律使用使用 GWxxx

## 接入方式
### cellmgr
* 继承 ICellMgrAdapter
* 初始化 CellMgrApi::Init，服务器启动完成并且初始 space 加载完成后调用 CellMgrApi::StartUp 标识着 cellmgr 的负载均衡开始执行
* 销毁 CellMgrApi::UnInit  
```
  // cellmgr 启动示例
  SHM_MGR.set_malloc_func(std::malloc); // 可选，默认使用 std::malloc
  SHM_MGR.set_free_func(std::free); // 可选，默认使用 std::free
  gw::cellmgr::CellMgrApi::Init(new gw::CellMgrAdapter(buffer_, "./space/", "./cell_conf.json"));

  // 设置 gapp，gappid不能是 0
  gw::cellmgr::CellMgrApi::RegisterGameApp(301);
  gw::cellmgr::CellMgrApi::RegisterGameApp(302);

  // 创建space
  gw::cellmgr::CellMgrApi::CreateSpace(CreateSpaceMsg& req);
  gw::cellmgr::CellMgrApi::CreateSpace(CreateSpaceMsg& req);

  // 启动完成，开始动态负载均衡
  gw::cellmgr::CellMgrApi::StartUp();

  // 关服的时候执行
  gw::cellmgr::CellMgrApi::UnInit(false); // 使用shm的话参数用true
```


### gamespace
#### space
* 继承 IGameSpaceAdapter/ICellEntityAdapter
* 初始化 GameSpaceApi::Init 
* 销毁 GameSpaceApi::UnInit
```
  gw::game::GameSpaceApi::Init(adapter_);
```

#### cell_entity
* 进入space: Space::Enter，会在大世界中生成一个 cell_entity
* 离线等原因主动离开space Space::Leave
* 大世界中的cell_entity状态主要是根据玩家的位置确定的，调用 Move(const Vector3& pos) 这个更新位置
```
  // 往大世界系统中增加一个玩家
  auto space = gw::game::GameSpaceApi::GetSpace(space_entity_->space_id());
  auto ret = space->Enter(eid_, base_eid_, base_gapp_id_, {x, y, z}, cell_adapter);
  ICellEntity* cell_entity = ret.second;

  // 定期调用更新位置
  cell_entity->Move({x, y, z});

  // 有数据修改的时候，添加同步数据，这块数据会自动同步给ghost，根据 flag 决定是否同步给 aoi
  cell_entity->NotifyData(notify_flag, data_type, raw_data, label);

  // 定期调用，从aoi系统中获取同步数据
  cell_entity->Notify(); 
```
