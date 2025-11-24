# LCZMuduo - 高性能 C++ 网络库

## 📖 项目简介

LCZMuduo 是一个基于 C++11 开发的高性能网络库，灵感来自陈硕的 muduo 网络库。本项目采用 Reactor 模式和多线程架构，提供了完整的 TCP 网络编程解决方案。

## ✨ 核心特性

- 🚀 **高性能事件驱动架构** - 基于 epoll 的 Reactor 模式
- 🔄 **多线程支持** - 主从 Reactor 线程池设计
- ⏰ **内置定时器** - 时间轮算法实现高效定时任务
- 🛡️ **连接管理** - 自动非活跃连接清理
- 📦 **缓冲区管理** - 高效的内存读写缓冲
- 🧵 **线程安全** - 完善的线程间任务调度

## 🚀 快速开始

### 环境要求
- Linux 系统 (推荐 Ubuntu 18.04+ 或 CentOS 7+)
- GCC 4.8+ (支持 C++11)
- CMake 3.0+

### 编译安装
```bash
# 克隆项目
git clone https://github.com/lczllx/muduo.git
cd muduo

# 编译项目
mkdir build && cd build
cmake ..
make -j$(nproc)

# 运行示例（从build目录）
../bin/test

# 或者从项目根目录运行
./bin/test
```

### 使用示例
```cpp
#include "TcpServer.hpp"
#include "Logger.hpp"

int main() {
    TcpServer server(8889);
    server.SetThreadCnt(2);
    
    server.SetMessageCallBack([](const PtrConnection& conn, Buffer* buf) {
        std::string msg = buf->GetLineAndPop();
        L_INFO("Received: %s", msg.c_str());
        conn->Send("Echo: " + msg);
    });
    
    L_INFO("Echo server starting on port 8889...");
    server.Start();
    return 0;
}
```

## 📁 项目结构

```
muduo/
├── include/                   # 头文件目录
├── src/                      # 源文件目录
├── example/                  # 使用示例
├── CMakeLists.txt           # 构建配置
├── LICENSE                  # 许可证文件
├── .gitignore              # Git忽略配置
└── README.md               # 项目说明文档
```

**编译后的目录结构**:
```
muduo/
├── build/                   # 编译目录（构建时生成）
├── lib/                     # 静态库输出目录
│   └── liblczmuduo.a       # 编译生成的静态库
└── bin/                     # 可执行文件输出目录
    └── test                 # 示例程序
```

## 📞 联系作者

- **作者**: lczllx
- **邮箱**: 2181719471@qq.com
- **GitHub**: [lczllx](https://github.com/lczllx)

## 🙏 致谢

感谢陈硕的 muduo 网络库提供的设计灵感。

## ⭐ 支持项目

如果这个项目对你有帮助，请给它一个 Star！

🐛 发现问题？欢迎提交 Issue

💡 有改进建议？欢迎提交 Pull Request