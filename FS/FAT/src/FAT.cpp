//==============================================================================
// FAT.cpp
//==============================================================================
#include "jxglib/FAT.h"

namespace jxglib::FAT {

//------------------------------------------------------------------------------
// FAT::File
//------------------------------------------------------------------------------
File::File() {}

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
Dir::Dir() {}

Dir::~Dir()
{
	Close();
}

bool Dir::Read(FS::FileInfo** ppFileInfo)
{
	FILINFO& filInfo = fileInfo_.GetEntity();
	*ppFileInfo = &fileInfo_;
	return ::f_readdir(&dir_, &filInfo) == FR_OK && filInfo.fname[0] != '\0';
}

void Dir::Close()
{
	::f_closedir(&dir_);
}

//------------------------------------------------------------------------------
// FAT::Drive
//------------------------------------------------------------------------------
Drive* Drive::pDriveHead_ = nullptr;

Drive::Drive(const char* driveName) : FS::Drive(driveName), pDriveNext_{nullptr}
{
	if (pDriveHead_) {
		Drive* pDrive = pDriveHead_;
		for ( ; pDrive->pDriveNext_; pDrive = pDrive->pDriveNext_) ;
		pDrive->pDriveNext_ = this;
	} else {
		pDriveHead_ = this;
	}
}

void Drive::Mount(MountMode mountMode)
{
	TCHAR path[16];
	::snprintf(path, sizeof(path), "%d:", 0);
	::f_mount(&fatFs_, path, (mountMode == MountMode::Forced)? 1 : 0);
}

FS::File* Drive::OpenFile(const char* fileName, const char* mode)
{
	RefPtr<File> pFile(new File());
	BYTE flags = 0;
	if (mode[0] == 'r') flags |= FA_READ;
	if (mode[0] == 'w') flags |= FA_WRITE | FA_CREATE_ALWAYS;
	if (mode[0] == 'a') flags |= FA_WRITE | FA_OPEN_APPEND;
	return (::f_open(pFile->GetEntity(), fileName, flags) == FR_OK)? pFile.release() : nullptr;
}

FS::Dir* Drive::OpenDir(const char* dirName)
{
	RefPtr<Dir> pDir(new Dir());
	return (::f_opendir(pDir->GetEntity(), dirName) == FR_OK)? pDir.release() : nullptr;
}

bool Drive::RemoveFile(const char* fileName)
{
	return ::f_unlink(fileName) == FR_OK;
}

bool Drive::RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	return ::f_rename(fileNameOld, fileNameNew) == FR_OK;
}

bool Drive::CreateDir(const char* dirName)
{
	return ::f_mkdir(dirName) == FR_OK;
}

bool Drive::RemoveDir(const char* dirName)
{
	return ::f_rmdir(dirName) == FR_OK;
}

bool Drive::RenameDir(const char* fileNameOld, const char* fileNameNew)
{
	return ::f_rename(fileNameOld, fileNameNew) == FR_OK;
}

bool Drive::Format()
{
#if defined(FF_USE_MKFS)
	MKFS_PARM opt {
		fmt: FM_ANY,	// auto
		n_fat: 0,		// default
		align: 0,		// default
		n_root: 0,		// default
		au_size: 0		// default
	};
	char work[FF_MAX_SS];
	return ::f_mkfs("0:", &opt, work, sizeof(work)) == FR_OK;
#else
	return false;
#endif
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

}

//------------------------------------------------------------------------------
// Callback Function
//------------------------------------------------------------------------------
DSTATUS disk_initialize(BYTE pdrv)
{
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	return pDrive->initialize();
}

DSTATUS disk_status(BYTE pdrv)
{
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	return pDrive->status();
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	return pDrive->read(buff, sector, count);
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	using namespace jxglib;
	FAT::Drive* pDrive = FAT::Drive::LookupDrive(pdrv);
	if (!pDrive) return RES_PARERR;
	return pDrive->write(buff, sector, count);
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
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
	DWORD year = 2025;
	DWORD month = 1;
	DWORD dayOfMonth = 1;
	DWORD hour = 0;
	DWORD minute = 0;
	DWORD second = 0;
	return ((year - 1980) << 25) | (month << 21) | (dayOfMonth << 16) |
			(hour << 11) | (minute << 5) | (second >> 1);
}
