//==============================================================================
// FAT.cpp
//==============================================================================
#include <memory>
#include "jxglib/FAT.h"

namespace jxglib::FAT {

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
FS::FileInfo* MakeFileInfo(const FILINFO& filInfo)
{
	DateTime dateTime(
		((filInfo.fdate >> 9) & 0x7f) + 1980,	// year
		(filInfo.fdate >> 5) & 0x0f,			// month
		filInfo.fdate & 0x1f,					// day
		filInfo.ftime >> 11,					// hour
		(filInfo.ftime >> 5) & 0x3F,			// minute
		(filInfo.ftime & 0x1f) * 2,				// second
		0);										// msec
	return new FS::FileInfo(filInfo.fname,
		((filInfo.fattrib & AM_DIR)? FS::FileInfo::Attr::Directory : 0) |
		((filInfo.fattrib & AM_ARC)? FS::FileInfo::Attr::Archive : 0) |
		((filInfo.fattrib & AM_RDO)? FS::FileInfo::Attr::ReadOnly : 0) |
		((filInfo.fattrib & AM_HID)? FS::FileInfo::Attr::Hidden : 0) |
		((filInfo.fattrib & AM_SYS)? FS::FileInfo::Attr::System : 0),
		filInfo.fsize, dateTime);
}

FS::FileInfo* MakeFileInfoForRootDir()
{
	// Create a dummy FileInfo for the root directory
	return new FS::FileInfo("", FS::FileInfo::Attr::Directory, 0, DateTime::Empty);
}

//------------------------------------------------------------------------------
// FAT::Drive
//------------------------------------------------------------------------------
Drive* Drive::pDriveHead_ = nullptr;

Drive::Drive(const char* driveName) : FS::Drive("FAT", driveName), pdrv_{0}, pDriveNext_{nullptr}
{
	if (pDriveHead_) {
		pdrv_ = 1;
		Drive* pDrive = pDriveHead_;
		for ( ; pDrive->pDriveNext_; pDrive = pDrive->pDriveNext_, pdrv_++) ;
		pDrive->pDriveNext_ = this;
	} else {
		pDriveHead_ = this;
	}
	if (pdrv_ >= FF_VOLUMES) {
		::panic("set FF_VOLUMES of jxglib_configure_FAT() in CMakeLists.txt to %d or more", pdrv_ + 1);
	}
}

const char* Drive::GetFileSystemName()
{
	if (!Mount()) return "unmounted";
	return
		(fatFs_.fs_type == FS_FAT12)? "FAT12" :
		(fatFs_.fs_type == FS_FAT16)? "FAT16" :
		(fatFs_.fs_type == FS_FAT32)? "FAT32" :
		(fatFs_.fs_type == FS_EXFAT)? "exFAT" : "unknown";
}

const char* Drive::NativePathName(char* pathNameBuff, int lenBuff, const char* pathName)
{
	char* p = pathNameBuff;
	int len = ::snprintf(p, lenBuff, "%d:", pdrv_);
	if (len >= lenBuff) ::panic("Drive::RegulatePathName");
	p += len, lenBuff -= len;
	FS::Drive::RegulatePathName(p, lenBuff, pathName);
	return pathNameBuff;
}

FS::File* Drive::OpenFile(const char* fileName, const char* mode)
{
	if (!Mount()) return nullptr;
	std::unique_ptr<File> pFile(new File(*this));
	BYTE flags = 0;
	if (mode[0] == 'r') flags |= FA_READ;
	if (mode[0] == 'w') flags |= FA_WRITE | FA_CREATE_ALWAYS;
	if (mode[0] == 'a') flags |= FA_WRITE | FA_OPEN_APPEND;
	return (::f_open(pFile->GetEntity(), fileName, flags) == FR_OK)? pFile.release() : nullptr;
}

FS::Dir* Drive::OpenDir(const char* dirName, uint8_t attrExclude)
{
	if (!Mount()) return nullptr;
	std::unique_ptr<Dir> pDir(new Dir(*this, attrExclude));
	return (::f_opendir(pDir->GetEntity(), dirName) == FR_OK)? pDir.release() : nullptr;
}

bool Drive::RemoveFile(const char* fileName)
{
	return Mount() && ::f_unlink(fileName) == FR_OK;
}

bool Drive::RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	return Mount() && ::f_rename(fileNameOld, fileNameNew) == FR_OK;
}

bool Drive::CreateDir(const char* dirName)
{
	return Mount() && ::f_mkdir(dirName) == FR_OK;
}

bool Drive::RemoveDir(const char* dirName)
{
	return Mount() && ::f_rmdir(dirName) == FR_OK;
}

bool Drive::RenameDir(const char* fileNameOld, const char* fileNameNew)
{
	return Mount() && ::f_rename(fileNameOld, fileNameNew) == FR_OK;
}

bool Drive::Format()
{
#if defined(FF_USE_MKFS)
	MKFS_PARM opt {
		fmt:		FM_ANY,	// auto
		n_fat:		0,		// default
		align:		0,		// default
		n_root:		0,		// default
		au_size:	0		// default
	};
	char path[16];
	::snprintf(path, sizeof(path), "%d:", pdrv_);
	char work[FF_MAX_SS];
	return ::f_mkfs(path, &opt, work, sizeof(work)) == FR_OK;
#else
	return false;
#endif
}

bool Drive::Mount()
{
	if (mountedFlag_) return true;
	TCHAR path[16];
	::snprintf(path, sizeof(path), "%d:", pdrv_);
	if (::f_mount(&fatFs_, path, 1) == FR_OK) {	// mounts immediately
		mountedFlag_ = true;
		return true;
	}
	return false;
}

bool Drive::Unmount()
{
	if (!mountedFlag_) return true;
	char path[16];
	::snprintf(path, sizeof(path), "%d:", pdrv_);
	if (::f_mount(nullptr, path, 0) == FR_OK) {
		mountedFlag_ = false;
		return true;
	}
	return false;
}

FS::FileInfo* Drive::GetFileInfo(const char* pathName)
{
	if (!Mount()) return nullptr;
	FILINFO filInfo;
	// f_stat() fails when given with a root directory. See https://elm-chan.org/fsw/ff/doc/stat.html.
	return (::strcmp(pathName, "/") == 0 || pathName[0] == '\0')? MakeFileInfoForRootDir() :
		(::f_stat(pathName, &filInfo) == FR_OK)? MakeFileInfo(filInfo) : nullptr;
}

uint64_t Drive::GetBytesTotal()
{
	if (!Mount()) return 0;
#if 0
	// see Examle in https://elm-chan.org/fsw/ff/doc/getfree.html
	FATFS* fs;
	char path[16];
	::snprintf(path, sizeof(path), "%d:", pdrv_);
	DWORD fre_clust;
	if (::f_getfree(path, &fre_clust, &fs) != FR_OK) return 0;
	DWORD tot_sect = (fs->n_fatent - 2) * fs->csize;
	return tot_sect * SectorSize;
#endif
	LBA_t sectorCount;
	ioctl_GET_SECTOR_COUNT(&sectorCount);
	return static_cast<uint64_t>(sectorCount) * SectorSize;
}

uint64_t Drive::GetBytesUsed()
{
	if (!Mount()) return 0;
	// see Examle in https://elm-chan.org/fsw/ff/doc/getfree.html
	FATFS* fs;
	char path[16];
	::snprintf(path, sizeof(path), "%d:", pdrv_);
	DWORD fre_clust;
	if (::f_getfree(path, &fre_clust, &fs) != FR_OK) return 0;
	uint64_t tot_sect = (static_cast<uint64_t>(fs->n_fatent) - 2) * fs->csize;
	uint64_t fre_sect = static_cast<uint64_t>(fre_clust) * fs->csize;
	return (tot_sect - fre_sect) * SectorSize;
}

Drive* Drive::LookupDrive(BYTE pdrv)
{
	Drive* pDrive = pDriveHead_;
	for (BYTE pdrvIter = 0; pDrive; pDrive = pDrive->pDriveNext_, pdrvIter++) {
		if (pdrv == pdrvIter) return pDrive;
	}
	return nullptr;
}

const char* Drive::FRESULTToStr(FRESULT result)
{
	static const char* strTbl[] = {
		"OK",					/* (0) Function succeeded */
		"DISK_ERR",				/* (1) A hard error occurred in the low level disk I/O layer */
		"INT_ERR",				/* (2) Assertion failed */
		"NOT_READY",			/* (3) The physical drive does not work */
		"NO_FILE",				/* (4) Could not find the file */
		"NO_PATH",				/* (5) Could not find the path */
		"INVALID_NAME",			/* (6) The path name format is invalid */
		"DENIED",				/* (7) Access denied due to a prohibited access or directory full */
		"EXIST",				/* (8) Access denied due to a prohibited access */
		"INVALID_OBJECT",		/* (9) The file/directory object is invalid */
		"WRITE_PROTECTED",		/* (10) The physical drive is write protected */
		"INVALID_DRIVE",		/* (11) The logical drive number is invalid */
		"NOT_ENABLED",			/* (12) The volume has no work area */
		"NO_FILESYSTEM",		/* (13) Could not find a valid FAT volume */
		"MKFS_ABORTED",			/* (14) The f_mkfs function aborted due to some problem */
		"TIMEOUT",				/* (15) Could not take control of the volume within defined period */
		"LOCKED",				/* (16) The operation is rejected according to the file sharing policy */
		"NOT_ENOUGH_CORE",		/* (17) LFN working buffer could not be allocated or given buffer is insufficient in size */
		"TOO_MANY_OPEN_FILES",	/* (18) Number of open files > FF_FS_LOCK */
		"INVALID_PARAMETER",	/* (19) Given parameter is invalid */
	};
	return strTbl[static_cast<int>(result)];
}

//------------------------------------------------------------------------------
// FAT::File
//------------------------------------------------------------------------------
File::File(FS::Drive& drive) : FS::File(drive) {}

File::~File()
{
	Close();
}

int File::Read(void* buff, int bytesBuff)
{
	UINT bytesRead;
	return (::f_read(&fil_, buff, bytesBuff, &bytesRead) == FR_OK)? static_cast<int>(bytesRead) : -1;
}

int File::Write(const void* buff, int bytesBuff)
{
	UINT bytesWritten;
	return (::f_write(&fil_, buff, bytesBuff, &bytesWritten) == FR_OK)? static_cast<int>(bytesWritten) : -1;
}

void File::Close()
{
	::f_close(&fil_);
}

bool File::Seek(int position)
{
	return ::f_lseek(&fil_, position) == FR_OK;
}

int File::Tell()
{
	return f_tell(&fil_);
}

int File::Size()
{
	return f_size(&fil_);
}

bool File::Flush()
{
	return f_sync(&fil_) == FR_OK;
}

bool File::Truncate(int bytes)
{
	return f_truncate(&fil_) == FR_OK;
}

bool File::Sync()
{
	return f_sync(&fil_) == FR_OK;
}

//------------------------------------------------------------------------------
// FAT::Dir
//------------------------------------------------------------------------------
Dir::Dir(FS::Drive& drive, uint8_t attrExclude) : FS::Dir(drive),
	fattribSkip_{static_cast<BYTE>(
		((attrExclude & FS::FileInfo::Attr::Hidden)? 0 : AM_HID) |
		((attrExclude & FS::FileInfo::Attr::System)? 0 : AM_SYS))},
	nItems_{0}
{
}

Dir::~Dir()
{
	Close();
}

FS::FileInfo* Dir::Read()
{
	FILINFO	filInfo;
	for (;;) {
		if (::f_readdir(&dir_, &filInfo) != FR_OK) return nullptr;
		if (filInfo.fname[0] == '\0') {	// end of directory
			if (rewindFlag_ && nItems_ > 0) {
				::f_rewinddir(&dir_);
				nItems_ = 0;
			} else {
				return nullptr;	// no more items
			}
		} else if (!(filInfo.fattrib & fattribSkip_)) {
			nItems_++;
			break;	// find a valid file or directory entry
		}
	}
	return MakeFileInfo(filInfo);
}

void Dir::Close()
{
	::f_closedir(&dir_);
}

}

//------------------------------------------------------------------------------
// Callback Function
//------------------------------------------------------------------------------
DSTATUS disk_initialize(BYTE pdrv)
{
	//::printf("disk_initialize(%d)\n", pdrv);
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	return pDrive->initialize();
}

DSTATUS disk_status(BYTE pdrv)
{
	//::printf("disk_status(%d)\n", pdrv);
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	return pDrive->status();
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("disk_read(%d, %d, %d)\n", pdrv, sector, count);
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	return pDrive->read(buff, sector, count);
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("disk_write(%d, %d, %d)\n", pdrv, sector, count);
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	return pDrive->write(buff, sector, count);
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
	//::printf("disk_ioctl(%d, %d)\n", pdrv, cmd);
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	switch (cmd) {
	case CTRL_SYNC: {
		return pDrive->ioctl_CTRL_SYNC();
	}
	case GET_SECTOR_COUNT: {
		LBA_t* pSectorCount = reinterpret_cast<LBA_t*>(buff);
		return pDrive->ioctl_GET_SECTOR_COUNT(pSectorCount);
	}
	case GET_SECTOR_SIZE: {
		WORD* pSectorSize = reinterpret_cast<WORD*>(buff);
		return pDrive->ioctl_GET_SECTOR_SIZE(pSectorSize);
	}
	case GET_BLOCK_SIZE: {
		DWORD* pBlockSize = reinterpret_cast<DWORD*>(buff);
		return pDrive->ioctl_GET_BLOCK_SIZE(pBlockSize);
	}
	case CTRL_TRIM: {
		LBA_t* args = reinterpret_cast<LBA_t*>(buff);
		return pDrive->ioctl_CTRL_TRIM(args[0], args[1]);
	}
	default:
		break;
	}
	return RES_PARERR;
}

DWORD get_fattime()
{
	DWORD year = 2020;
	DWORD month = 1;
	DWORD dayOfMonth = 1;
	DWORD hour = 0;
	DWORD minute = 0;
	DWORD second = 0;
	return ((year - 1980) << 25) | (month << 21) | (dayOfMonth << 16) |
			(hour << 11) | (minute << 5) | (second >> 1);
}
