#ifndef MUDUO_BUFFER_H
#define MUDUO_BUFFER_H

#include <cstdint>
#include <vector>
#include <cassert>
#include <cstring>
#include <string>

#define BUFFER_DEFAULT_SIZE 1024

class Buffer {
private:
    uint64_t _read_idx;
    uint64_t _write_idx;
    std::vector<char> _buffer;

public:
    Buffer();
    char* Begin();
    
    // 简单getter在头文件实现
    char* GetWritePtr() { return Begin() + _write_idx; }
    char* GetReadPtr() { return Begin() + _read_idx; }
    uint64_t HeadIdleSize() { return _read_idx; }
    uint64_t TailIdleSize() { return _buffer.size() - _write_idx; }
    size_t ReadableBytes() { return _write_idx - _read_idx; }
    
    void MoveReadoffset(uint64_t len);
    void MoveWriteoffset(uint64_t len);
    void EnsureWritableBytes(uint64_t len);
    void Write(const void* data, uint64_t len);
    void WriteAndpush(const void* data, uint64_t len);
    void Writestring(const std::string& data);
    void WritestringAndpush(const std::string& data);
    void WriteBuffer(Buffer& data);
    void WriteBufferAndpush(Buffer& data);
    void Read(void* buf, uint64_t len);
    std::string ReadAsstring(uint64_t len);
    void ReadAndpop(void* buf, uint64_t len);
    std::string ReadAsstringandpop(uint64_t len);
    char* FindcrLf();
    std::string GetLine();
    std::string GetLineAndPop();
    void clear();
};

#endif