#!/bin/bash

# LCZMuduo 网络库安装脚本
# 使用方法: ./install.sh [安装路径]
# 默认安装路径: /usr/local

set -e  # 遇到错误立即退出

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 获取安装路径
INSTALL_PREFIX="${1:-/usr/local}"

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  LCZMuduo 网络库安装脚本${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# 检查是否为root用户（安装到系统路径需要）
if [ "$INSTALL_PREFIX" = "/usr/local" ] && [ "$EUID" -ne 0 ]; then
    echo -e "${YELLOW}警告: 安装到系统路径需要root权限${NC}"
    echo -e "${YELLOW}请使用: sudo ./install.sh${NC}"
    echo -e "${YELLOW}或者安装到用户目录: ./install.sh ~/local${NC}"
    exit 1
fi

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}错误: 未找到cmake，请先安装cmake${NC}"
    echo "Ubuntu/Debian: sudo apt-get install cmake"
    echo "CentOS/RHEL: sudo yum install cmake"
    exit 1
fi

# 检查编译器
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}错误: 未找到g++，请先安装g++${NC}"
    echo "Ubuntu/Debian: sudo apt-get install g++"
    echo "CentOS/RHEL: sudo yum install gcc-c++"
    exit 1
fi

echo -e "${GREEN}[1/4] 清理旧的构建文件...${NC}"
rm -rf build
mkdir -p build

echo -e "${GREEN}[2/4] 配置CMake...${NC}"
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" ..

echo -e "${GREEN}[3/4] 编译项目...${NC}"
make -j$(nproc)

echo -e "${GREEN}[4/4] 安装到系统...${NC}"
make install

cd ..

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  安装完成！${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "安装路径: ${YELLOW}$INSTALL_PREFIX${NC}"
echo -e "库文件: ${YELLOW}$INSTALL_PREFIX/lib/liblczmuduo.a${NC}"
echo -e "头文件: ${YELLOW}$INSTALL_PREFIX/include/lczmuduo/${NC}"
echo ""
echo -e "${GREEN}使用方法:${NC}"
echo ""
echo "1. 编译时链接库:"
echo "   g++ your_code.cpp -llczmuduo -lpthread -o your_program"
echo ""
echo "2. 如果库不在默认路径，指定路径:"
echo "   g++ your_code.cpp -L$INSTALL_PREFIX/lib -llczmuduo -lpthread -I$INSTALL_PREFIX/include/lczmuduo -o your_program"
echo ""
echo "3. 使用CMake:"
echo "   find_library(LCZMUDUO_LIB lczmuduo PATHS $INSTALL_PREFIX/lib)"
echo "   include_directories($INSTALL_PREFIX/include/lczmuduo)"
echo "   target_link_libraries(your_target \${LCZMUDUO_LIB} pthread)"
echo ""
echo -e "${GREEN}示例代码:${NC}"
echo ""
echo "#include <lczmuduo/TcpServer.hpp>"
echo "#include <lczmuduo/Logger.hpp>"
echo ""
echo "int main() {"
echo "    TcpServer server(8889);"
echo "    server.Start();"
echo "    return 0;"
echo "}"
echo ""

