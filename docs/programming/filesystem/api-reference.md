## File System API

## File Writing

An example of writing a file to a LittleFS file system:

`CMakeLists.txt`'s end should include:

```cmake
target_link_libraries(fs-flash jxglib_LFS_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp title="fs-flash.cpp" linenums="1"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash drive("Drive:", 0x0004'0000);  // 256kB
    if (!FS::Mount("Drive:") && !FS::Format(Stdio::Instance, "Drive:")) return 1;
    FS::File* pFile = FS::OpenFile("Drive:/test.txt", "w");
    if (pFile) {
        for (int i = 0; i < 10; ++i) {
            pFile->Printf("Line %d\n", i + 1);
        }
        pFile->Close();
        delete pFile;
    }
}
```

- `LFS::Flash drive("Drive", 0x0004'0000);  // 256kB`
  Creates a LittleFS file system drive named `Drive` with 256kB size
- `if (!FS::Mount("Drive:") && !FS::Format(Stdio::Instance, "Drive:")) return 1;`
  Checks if the drive is mounted. If not, formats the drive
- `File* pFile = FS::OpenFile("Drive:/test.txt", "w");`
  Opens a file for writing
- `pFile->Printf("Line %d\n", i + 1);`
  Writes a line to the file
- `pFile->Close();`
  Closes the file
- `delete pFile;`
  Deletes the file pointer

The same operations are possible with FAT file systems.

### File Reading

An example of reading a file from a LittleFS file system:

`CMakeLists.txt`'s end should include:

```cmake
target_link_libraries(fs-flash jxglib_LFS_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp title="fs-flash.cpp" linenums="1"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash drive("Drive:", 0x0004'0000);  // 256kB
    FS::File* pFile = FS::OpenFile("Drive:/test.txt", "r");
    if (pFile) {
        char line[256];
        while (pFile->ReadLine(line, sizeof(line)) > 0) {
            ::printf("%s\n", line);      
        }
        pFile->Close();
        delete pFile;
    }
}
```

- `File* pFile = FS::OpenFile("Drive:/test.txt", "r");`
  Opens a file for reading
- `pFile->ReadLine(line, sizeof(line))`
  Reads a line from the file

### Directory Information

An example of getting directory information from a LittleFS file system:

`CMakeLists.txt`'s end should include:

```cmake
target_link_libraries(fs-flash jxglib_LFS_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp title="fs-flash.cpp" linenums="1"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash drive("Drive:", 0x0004'0000);  // 256kB
    FS::Dir* pDir = FS::OpenDir("Drive:/");
    if (pDir) {
        FS::FileInfo* pFileInfo;
        while (pFileInfo = pDir->Read()) {
            ::printf("%-16s%d\n", pFileInfo->GetName(), pFileInfo->GetSize());
            delete pFileInfo;
        }
        pDir->Close();
        delete pDir;
    }
}
```

- `FS::Dir* pDir = FS::OpenDir("Drive:/");`
  Opens a directory
- `pFileInfo = pDir->Read()`
  Reads file information into an FS::FileInfo instance
- `pDir->Close();`
  Closes the directory

## API Reference

### FS::CopyFile

```cpp
bool FS::CopyFile(const char* fileNameSrc, const char* fileNameDst);
```

Copies a file

### FS::Move

```cpp
bool FS::Move(const char* fileNameOld, const char* fileNameNew);
```

Renames or moves a file or directory (directory moving is not supported)

### FS::RemoveFile

```cpp
bool FS::RemoveFile(const char* fileName);
```

Deletes a file

### FS::RemoveDir

```cpp
bool FS::RemoveDir(const char* dirName);
```

Deletes a directory

### FS::CreateDir

```cpp
bool FS::CreateDir(const char* dirName);
```

Creates a directory

### FS::ChangeCurDir

```cpp
bool FS::ChangeCurDir(const char* dirName);
```

Changes the current directory

### FS::Format

```cpp
bool FS::Format(const char* driveName);
```

Formats the file system

### FS::Mount

```cpp
bool FS::Mount(const char* driveName);
```

Mounts the file system

### FS::Unmount

```cpp
bool FS::Unmount(const char* driveName);
```

Unmounts the file system
