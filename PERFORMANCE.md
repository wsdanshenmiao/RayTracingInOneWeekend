# CPU 软光追 性能基准与多线程瓶颈分析报告

## 当前项目已落地的优化（2026-10-12）

本节为 `D:\Code\RayTracingInOneWeekend` 的实际源码修改及本机复测；下方旧报告仅保留作历史记录。

### 修改与契约

- 移除 `HittableList` 内部共享互斥锁，包括逐光线 `Hit()` 的读锁。列表增删限定在构建阶段，由调用方串行执行；渲染期间列表和可达资源不可修改或销毁。
- `HitRecord::m_Material` 改为借用的 `const Material*`。Sphere/Quad/ConstantMedium 仍持有材质的 `shared_ptr`，命中时只传递 `.get()`，不更新材质引用计数。
- `Material::Emitted()` 及光源材质实现改为 `const`。
- 保留 `Scene::Render()` 的场景级互斥锁，并为 `SetCamera/SetMaxDepth/SetUseMultithread` 补齐相同的同步。工作线程完成前，场景所有者不被替换。外部对象别名仍须遵守只读契约。
- 没有改动随机数、采样、求交算法、反弹算法、线程数量或任务调度。

### 本机对照结果

AMD Ryzen Threadripper PRO 3945WX，12 核 / 24 逻辑处理器；MSVC 14.51.36231；xmake Release，原有优化参数保持不变。
Cornell Box，768×768，9 spp，最大深度 5。保留修改前可执行文件，分别预热，优化前后单/多线程交错运行，各测 3 次。

| 版本 / 模式 | 三次渲染耗时（ms） | 中位耗时（ms） |
|---|---|---:|
| 原版 MT | 2261.494 / 1997.886 / 1864.659 | 1997.886 |
| 优化 MT | 522.747 / 428.866 / 499.550 | 499.550 |
| 原版 ST | 6164.137 / 6175.125 / 6218.577 | 6175.125 |
| 优化 ST | 6097.751 / 6000.525 / 5669.488 | 6000.525 |

- 多线程渲染约快 **4.00 倍**，耗时降低约 **75%**。
- 各版本自身的 ST/MT 加速比：**3.09 倍 → 12.01 倍**。
- 以上使用程序内场景渲染计时，不含场景构建和 PPM 保存。包含进程启动、场景构建与文件输出的墙钟中位耗时：原版 MT 2166.143 ms，优化 MT 702.086 ms，约快 3.09 倍。
- 此结果只代表上述硬件和参数，不代表 24 线程理论上应达到 24 倍收益。

### 验证与数据

- Debug / Release 行为回归测试：材质持有、嵌套列表/变换/BVH 的多线程查询、介质相位材质、均匀发光场景 ST/MT 的逐像素结果、渲染期间三个配置接口的串行化。
- 原版/优化版 ST/MT 的 Cornell PPM 均为 768×768，包含 1,769,472 个通道值，范围均为 0–255。通道均值为 56.76–56.82。
- Cornell 随机种子未固定，像素统计仅作完整性检查，不能视为逐像素完全一致的证明。
- 本次原始计时、日志和图像保留在 `build/OptimizationBenchmark/`（Git 忽略目录）：`timings.csv`、`output_statistics.csv`、各次 `.log` 和最终四张 `.ppm`。该目录同时保留 `baseline.exe` 与 `optimized.exe`。
- 回归测试命令见 `README.md`。

---

## 历史分析报告（原有内容，非当前环境）

以下为原有报告，硬件与绝对路径不同，代码位置可能已变化。其中的额外微基准及环境排除结论没有在本轮重新验证，不应当作本机测量结果。

- 项目：`E:\Code\RayTracingInOneWeekend`
- 测试日期：2026-10-12
- 场景：Cornell Box，1024 × 1024，MaxDepth 5
- 说明：**本报告涉及的所有源码改动均在副本（`_office_args/rt_probe`）中进行，未修改原项目任何文件。**

---

## 0. 结论摘要

多线程加速比只有 **1.65 ~ 1.78×**（32 逻辑核），**跨环境可复现**（你的终端实测 1.66×，本机 1.76×）—— 因此不是环境问题、不是测量问题、不是 CPU 降频问题。

根因是 `HittableList::Hit` 这条渲染热路径上叠加的**两个原子操作争抢源**：

1. `Src/Geometry/HittableList.cpp` —— `Hit()` 入口的 `std::shared_lock lock{ m_Mutex };`
2. `Src/Geometry/Hittable.h:23` —— `HitRecord::m_Material` 为 `std::shared_ptr<Material>`，每次命中触发原子引用计数

**在副本上同时消除这两点后（同一时段对照）：MT 从 3738 ms 降到 590 ms，多线程加速比从 1.76× 提升到 8.4×。**

---

## 1. 测试配置

| 项目 | 内容 |
|---|---|
| 构建 | xmake `mode.release`，MSVC 14.51.36231 |
| 编译参数 | `/O2 /Ob3 /Oi /Ot /GL /arch:AVX2 /fp:fast` + `/LTCG /OPT:REF /OPT:ICF` |
| 硬件 | Intel i9-13980HX：8 P-core + 16 E-core = 32 逻辑核 |
| 原始可执行 | `bin/release/RayTracing/RayTracing.exe` |

复现命令（**参数顺序**：`Width Height SamplePerPixel MaxDepth UseMultithread [Output]`）：

```
bin\release\RayTracing\RayTracing.exe 1024 1024 9 5 1 mt.ppm
bin\release\RayTracing\RayTracing.exe 1024 1024 9 5 0 st.ppm
```

> 用 **9 spp** 而非 8：见 §6.1，传入 8 会被整型截断成 4。

## 2. 基准结果（原始代码）

### 2.1 你的终端实测（9 spp / depth 5）

| 模式 | 各次 | 中位 |
|---|---|---|
| MT（32 线程） | 3905.5 / 3789.6 / 4298.7 / 3588.0 ms | **3847.6 ms** |
| ST（单线程） | 6485.1 / 6361.1 / 6379.5 ms | **6379.5 ms** |
| 加速比 | | **1.66×** |

### 2.2 本机复测（9 spp / depth 5）

| 模式 | 引擎计时 |
|---|---|
| MT | 3637 ~ 3774 ms（中位 3738） |
| ST | 6576 ms |
| 加速比 | **1.76×** |

### 2.3 深度扫描（1024×1024 / 9 spp）

| MaxDepth | MT | ST | 加速比 |
|---|---|---|---|
| 1 | 1339.6 ms | 2727.8 ms | 2.04× |
| 2 | 2756.4 ms | 4406.6 ms | 1.60× |
| 3 | 3517.2 ms | 5542.8 ms | 1.58× |
| 5 | 3708.6 ms | 6613.4 ms | 1.78× |
| 8 | 4131.3 ms | 7093.0 ms | 1.72× |

**连 depth=1（几乎不递归）也只有 2.04×** —— 说明瓶颈与递归深度、与每弹射分配无关，在更基础的层面。

## 3. 根因定位

### 3.1 每线程计时暴露的问题

在源码副本中给每个工作线程加计时后（MT，9 spp，depth 5）：

```
thr  0  rows    0-  32  start    0.4 ms  end  2e+02 ms   busy  2e+02
thr  7  rows  224- 256  start     80 ms  end  3e+03 ms   busy  3e+03
thr 15  rows  480- 512  start    300 ms  end  4e+03 ms   busy  3e+03
thr 31  rows  992-1024  start    900 ms  end  2e+03 ms   busy  1e+03
total wall = 3692.0 ms
```

- 线程 start 从 0.4 ms 单调递增到 **900 ms**（占总时长 24%）
- 各线程 busy 200 ~ 3000 ms，差 3 倍（静态分带负载不均）

改用**固定线程池 + 动态行调度**后，start 全部收敛到 1~70 ms、结束时间齐平，**但 MT 仍是 4000 ms 量级** —— 说明线程创建与负载不均只是次要因素，主因在别处。

### 3.2 对照实验：哪种负载在 32 线程下会塌

与光追无关的独立微基准（每线程固定完成 1 份工作，"加速比"按单份耗时折算）：

| 负载 | 32 线程加速比 |
|---|---|
| A 纯寄存器计算（零分配、零访存） | **21.66×** |
| B 流式内存访问（每线程私有缓冲） | 11.59× |
| C 高频 `make_shared` 分配 | 11.46× |
| D 共享大图随机指针追逐（模拟 BVH 遍历） | 8.27× |
| E `thread_local` mt19937_64 + distribution（复刻 `Pubh.h`） | 11.53× |
| **G 拷贝指向同一个共享对象的 `shared_ptr`** | **0.31×** ⚠️ |
| H 拷贝指向 8 个不同对象的 `shared_ptr` | **0.41×** ⚠️ |

**G 是唯一复现光追现象的负载**：单线程拷贝一次 6.9 ns，32 线程并发时暴增到 726 ns（慢 105 倍），加速比 0.31× —— **比单线程还慢 3 倍**。

这是 `shared_ptr` 控制块上的**原子引用计数造成的 cache line ping-pong**：32 个核反复争抢同一根 cache line。

> 顺带排除的假设：沙箱 CPU 限流（沙箱内外结果一致）、功耗墙降频（满载实测 `% Processor Performance` = 173%，即 4.19 GHz，正在睿频）、内存带宽、随机访存、`thread_local` RNG。它们在 32 线程下都还有 8~21×。

### 3.3 源码中的实际路径

**（a）`HitRecord::m_Material` 是 shared_ptr**

- `Src/Geometry/Hittable.h:23`：`std::shared_ptr<Material> m_Material;`
- `Src/Geometry/Quad.cpp:41`：`rec.m_Material = m_Mat;`  ← 原子引用计数
- `Src/Geometry/Sphere.cpp:50`：`hitRecord.m_Material = m_Material;`  ← 同上
- `Src/Geometry/ConstantMedium.cpp:48`：`rec.m_Material = m_PhaseFunc;`  ← 同上

每次光线命中都拷贝一次。场景只有少数几个共享材质对象，**所有线程的原子操作都打在同样几个控制块上**。

**（b）渲染热路径上的一把读锁**

- `Src/Geometry/HittableList.cpp` `Hit()` 第一行：`std::shared_lock lock{ m_Mutex };`

`world.Hit()` 每次调用都要获取这把读锁，而它只在 `Add()` / `Clear()` 时才会被写 —— 渲染期场景是只读的。但**每次 lock/unlock 都是 `std::shared_mutex` reader count 上的原子 RMW**，被 32 个线程高频争抢。

## 4. 优化验证

在源码副本上分步改动并实测（9 spp / depth 5）：

| 配置 | MT | ST | 加速比 |
|---|---|---|---|
| 原始 | 3738 ms | 6576 ms | 1.76× |
| 仅去 `shared_ptr` 引用计数 | 3512 ms | 5274 ms | 1.50× |
| 仅去读锁 | 1438 ~ 2543 ms（波动大） | 5215 ms | 2.15× |
| **两者都去** | **426 ~ 624 ms（中位 590）** | **4968 ms** | **8.4×** |

> **两个争抢源必须同时消除**。只解决其中一个时，另一个仍主导争抢，收益有限且不稳定。

**输出正确性已验证**：优化版产出的 PPM 为 1024×1024×3 = 3,145,728 个采样值，像素均值 56.75，与原始版完全一致，**不是提前退出的假结果**。

### 4.1 具体改动

```cpp
// 1) Hittable.h —— HitRecord 的材质指针去引用计数
//    原：std::shared_ptr<Material> m_Material;
Material* m_Material = nullptr;          // 生命周期由场景持有的 shared_ptr 保证

// 2) Quad.cpp / Sphere.cpp / ConstantMedium.cpp —— 赋值处取裸指针
rec.m_Material = m_Mat.get();            // 原子 incr/decr 消失

// 3) HittableList.cpp Hit() —— 移除渲染热路径上的读锁
//    原：std::shared_lock lock{ m_Mutex };
//    改为不加锁（或改用仅在构建期生效的方案）
```

### 4.2 同一时段对照结果

| 指标 | 原始 | 优化后 | 提升 |
|---|---|---|---|
| MT（32 线程） | 3738 ms | **590 ms** | **6.3×** |
| ST（单线程） | 6576 ms | 4968 ms | 1.3× |
| **多线程加速比** | **1.76×** | **8.4×** | **4.8×** |

## 5. 其余优化项（已单独验证，收益较小）

| 项 | 位置 | 实测效果 |
|---|---|---|
| 每弹射 `make_shared<HittablePDF>` 改栈对象 | `Camera.cpp:178` | ST 6593 → 5388 ms（-18%），MT 无改善 |
| `std::async` 逐个建线程改固定线程池 | `Camera.cpp:54-66` | 消除 900 ms 串行启动 |
| 静态分带改动态行调度 | `Camera.cpp:39-76` | 消除 3 倍负载不均（17~51 行/线程） |

## 6. 其他已确认问题

### 6.1 SPP 整型截断（影响参数语义）

- `Src/Camera.h:45`：`int m_SqrtSPP;`
- `Src/Camera.cpp:117`：`m_SqrtSPP = std::sqrt(m_SamplePerPixel);`
- `Src/Camera.cpp:88,97-98`：循环上界 `m_SqrtSPP`

传入 8 时 `sqrt(8)=2.828` 截断为 2，**每像素实际只做 2×2 = 4 次采样**。判据：传入 4 与传入 8 耗时几乎相同（256×256 下 119 ms vs 107 ms）；传入 9 得到 3×3 = 9 次，耗时翻倍。

### 6.2 随机数不可复现

`Src/Pubh.h:28-34`：`thread_local std::mt19937_64 gen{std::random_device{}()}`，每次运行种子不同，**逐像素结果不可复现**，只能做统计比较。

### 6.3 CMake 构建缺优化开关

`CMakeLists.txt` 未设置 `CMAKE_BUILD_TYPE`，**只有 `xmake.lua` 配置了最高优化**。用 CMake 直接构建会得到未优化产物。

## 7. 建议改动优先级

1. **去掉 `HittableList::Hit` 的 `std::shared_lock`** —— 单点收益最大
2. **`HitRecord::m_Material` 改为裸指针/句柄** —— 与上一条必须同时做
3. 修 SPP 整型截断
4. 线程池 + 动态行调度（次优，但结构上更稳）
5. 每弹射分配改栈对象
6. 补 CMake 优化配置

## 8. 数据文件索引

> 基准过程产生的大体积中间图像（`.ppm`，合计约 286 MB）已在报告完成后清理；
> 报告引用的**原始数据记录（CSV / 日志 / 文本输出）保留在 `Output/Benchmark/`**，图像可按 §1 的命令重新生成。

| 路径 | 内容 |
|---|---|
| `Output/Benchmark/benchmark_results.csv` | 原始 MT/ST 各 5 次 engine/wall 计时 |
| `Output/Benchmark/log_mt.txt` / `log_st.txt` / `log_warmup.txt` | 基准运行的完整终端输出 |
| `Output/Benchmark/depth/` | 深度扫描（depth 1/2/3/5/8）原始文本输出 |
| `C:\Users\wsdsm\WorkBuddy\2026-09-21-01-30-43\_office_args\mbtest\` | 负载对照微基准（A~H），可 `xmake` 直接编译 |
| `C:\Users\wsdsm\WorkBuddy\2026-09-21-01-30-43\_office_args\rt_probe\` | 源码副本：已应用全部优化，可编译验证 8.4× |
