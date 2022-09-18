#include "ByteBuffer.hh"

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <strings.h>    // this is a GNU header, not a std c header

using namespace base;

ByteBuffer::ByteBuffer() :_buf(kInitialBufferSize), _readIndex(0), _writeIndex(0)
{

}

ByteBuffer::~ByteBuffer()
{

}

ssize_t ByteBuffer::ReadFd(int fd, int* saveErrno)
{
    // 先读取fd到堆栈空间
    char buf[65535] = {0};
    bzero((void*)buf, sizeof(buf));

    ssize_t readRet = read(fd, (void *)buf, sizeof(buf));
    if (readRet < 0) {
        perror("read");
        *saveErrno = errno;
    } else {
        Append(buf, readRet);   // 追加到buffer中
    }

    return readRet;
}

size_t ByteBuffer::ReadableBytes()
{
    return _writeIndex - _readIndex;
}

size_t ByteBuffer::WritableBytes()
{
    return _buf.capacity() - _writeIndex;
}

size_t ByteBuffer::InternalBufferCapacity()
{
    return this->_buf.capacity();
}

void ByteBuffer::MakeRoom(size_t len)
{
    size_t newBufSize = (this->_buf.capacity() + len) * 2;
    this->_buf.resize(newBufSize);
}

void ByteBuffer::Shrink(void)
{
    this->_buf.shrink_to_fit();
}

char* ByteBuffer::Begin()
{
    return &(*_buf.begin());
}

char* ByteBuffer::ReadBegin()
{
    return &(*_buf.begin()) + _readIndex;
}

char* ByteBuffer::WriteBegin()
{
    return &(*_buf.begin()) + _writeIndex;
}

void ByteBuffer::Append(const char *ptr, size_t len)
{
    size_t writable = WritableBytes();
    if(len > writable) {
        MakeRoom(len);
    }

    std::copy(ptr, ptr + len, WriteBegin());
    _writeIndex += len;
}

void ByteBuffer::Retrieve(size_t len)
{
    size_t readableLen = ReadableBytes();
    if (len > readableLen) {
        return;
    }

    this->_readIndex += len;

    // 写入的已经都读完了
    if (_readIndex == _writeIndex) {
        _readIndex = 0;
        _writeIndex = 0;

        // 重置游标时当大于最大buffer大小, 重新置为初始大小
        if (this->_buf.size() > kMaximumBufferBytes) {
            this->_buf.resize(kInitialBufferSize);
        }
    }
}

void ByteBuffer::RetrieveAll(void)
{
    Retrieve(ReadableBytes());
}
