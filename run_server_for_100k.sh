#!/bin/bash
# 以支持 10 万连接的 fd 限制启动服务器
# 用法: ./run_server_for_100k.sh
# 注意: 若 ulimit 调高失败，需修改 /etc/security/limits.conf 后重新登录

ulimit -n 110000 2>/dev/null && echo "ulimit -n 已设为 $(ulimit -n)" || echo "无法调高 ulimit，当前: $(ulimit -n)"
exec ./bin/test
