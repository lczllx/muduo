#!/bin/bash
# 在保持 10 万连接的同时测 QPS
# 用法: ./run_100k_qps.sh
# 依赖: 先启动 ./bin/http_server (HTTP 服务，8889 端口)

HOST=127.0.0.1
PORT=8889

# 添加多 IP（与 run_100k.sh 相同）
for i in 2 3 4; do
    ip addr show lo 2>/dev/null | grep -q "127.0.0.$i" || sudo ip addr add 127.0.0.$i/32 dev lo 2>/dev/null || true
done

# 检查服务器
if ! ss -tlnp 2>/dev/null | grep -q ":$PORT "; then
    echo "请先启动 HTTP 服务: ./bin/http_server"
    exit 1
fi

echo "=== 10万连接下 QPS 压测 ==="
echo "1. 后台建立 10 万连接并保持..."
LOG=$(mktemp -d)
for ip in 127.0.0.1 127.0.0.2 127.0.0.3 127.0.0.4; do
    ./bin/concurrent_test $HOST $PORT 25000 90 $ip > "$LOG/${ip}.log" 2>&1 &
done

echo "2. 等待连接建立 (约 90 秒)..."
sleep 90

echo "3. 运行 WebBench 测 QPS (30 秒)..."
cd WebBench-master 2>/dev/null || cd WebBench-master
./webbench -c 2000 -t 30 http://$HOST:$PORT/hello
cd - >/dev/null

echo ""
echo "4. 连接保持任务仍在后台，可等待其自然结束"
wait 2>/dev/null
rm -rf "$LOG"
