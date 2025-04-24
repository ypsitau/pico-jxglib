//==============================================================================
// jxglib/FS.h
//==============================================================================
#ifndef PICO_JXGLIB_FS_H
#define PICO_JXGLIB_FS_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FS
//------------------------------------------------------------------------------
class FS {
public:
    class File {
    public:
        virtual ~File() {}
        virtual int Read(void* buffer, unsigned int size) = 0;
        virtual int Write(const void* buffer, unsigned int size) = 0;
        virtual void Close() = 0;
        virtual void Seek(unsigned int position) = 0;
        virtual unsigned int Tell() = 0;
        virtual unsigned int Size() = 0;
        virtual bool Remove() = 0;
        virtual bool Rename(const char* newName) = 0;
        virtual bool Flush() = 0;
        virtual bool Truncate(unsigned int size) = 0;
        virtual bool Sync() = 0;
    };
    class Dir {
    public:
        struct FileInfo {
            const char* name;
            unsigned int size;
        };

        virtual ~Dir() {}
        virtual bool First(FileInfo& info) = 0;
        virtual bool Next(FileInfo& info) = 0;
        virtual void Close() = 0;
        virtual bool Remove() = 0;
        virtual bool Exists() = 0;
        virtual bool Rename(const char* newName) = 0;
        virtual bool Create() = 0;
    };
public:
	FS() {}
};

}

#endif
