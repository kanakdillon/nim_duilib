#include "DirectoryTreeImpl.h"

#ifdef DUILIB_BUILD_FOR_MACOS

#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/FilePath.h"
#include "duilib/Utils/FilePathUtil.h"

#include <fstream>
#include <sys/statvfs.h>
#include <sys/mount.h>
#include <stdio.h>

#ifdef DUILIB_BIT_64
    #define __USE_FILE_OFFSET64
#endif
#include <sys/stat.h>

namespace ui
{
struct DirectoryTreeImpl::TImpl
{
    /** 共享的文件夹图标(大图标)
    */
    uint32_t m_nLargeFolderIconID = 0;

    /** 共享的文件夹图标(小图标)
    */
    uint32_t m_nSmallFolderIconID = 0;

    /** 共享的文件图标(大图标)
    */
    uint32_t m_nLargeFileIconID = 0;

    /** 共享的文件图标(小图标)
    */
    uint32_t m_nSmallFileIconID = 0;
};

DirectoryTreeImpl::DirectoryTreeImpl(DirectoryTree* pTree):
    m_pTree(pTree)
{
    m_impl = new TImpl;
}

DirectoryTreeImpl::~DirectoryTreeImpl()
{
    if (m_impl->m_nLargeFolderIconID != 0) {
        GlobalManager::Instance().Icon().RemoveIcon(m_impl->m_nLargeFolderIconID);
        m_impl->m_nLargeFolderIconID = 0;
    }
    if (m_impl->m_nSmallFolderIconID != 0) {
        GlobalManager::Instance().Icon().RemoveIcon(m_impl->m_nSmallFolderIconID);
        m_impl->m_nSmallFolderIconID = 0;
    }
    if (m_impl->m_nLargeFileIconID != 0) {
        GlobalManager::Instance().Icon().RemoveIcon(m_impl->m_nLargeFileIconID);
        m_impl->m_nLargeFileIconID = 0;
    }
    if (m_impl->m_nSmallFileIconID != 0) {
        GlobalManager::Instance().Icon().RemoveIcon(m_impl->m_nSmallFileIconID);
        m_impl->m_nSmallFileIconID = 0;
    }
    delete m_impl;
    m_impl = nullptr;
}

/** 获取图片属性字符串
*/
DString DirectoryTreeImplGetImageString(DirectoryTree* pTree, bool bLargeFile, const DString& imageFileName)
{
    if (imageFileName.empty()) {
        return DString();
    }
    int32_t nSmallIconSize = 20;
    int32_t nLargeIconSize = 32;
    if (pTree != nullptr) {
        nSmallIconSize = pTree->GetSmallIconSize();
        nLargeIconSize = pTree->GetLargeIconSize();
    }
    if (nSmallIconSize < 1) {
        nSmallIconSize = 20;
    }
    if (nLargeIconSize < 1) {
        nLargeIconSize = 32;
    }
    int32_t nIconSize = bLargeFile ? nLargeIconSize : nSmallIconSize;
    DString imageString = StringUtil::Printf(_T("file='%s/filesystem/%s' width='%d' height='%d' valign='center'"), DUILIB_PUBLIC_RES_DIR, imageFileName.c_str(), nIconSize, nIconSize);
    return imageString;
}

bool DirectoryTreeImpl::GetVirtualDirectoryInfo(VirtualDirectoryType type, FilePath& filePath, DString& displayName, uint32_t& nIconID)
{
    filePath.Clear();
    displayName.clear();
    nIconID = 0;

    //用户的HOME目录
    FilePath userHomeDir;
    const char* home = std::getenv("HOME");
    if (home != nullptr) {
        userHomeDir = FilePath(home);
    }
    else {
        userHomeDir = _T("/");
    }

    userHomeDir.NormalizeDirectoryPath();
    switch (type) {
    case VirtualDirectoryType::kUserHome:
        filePath = userHomeDir;
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, _T("folder-home.svg")));
        break;
    case VirtualDirectoryType::kDesktop:
        filePath = userHomeDir;
        filePath += _T("Desktop");
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
            filePath += GlobalManager::GetTextById(_T("STRID_PUBLIC_DIRECTORY_TREE_DESKTOP"));
        }
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
        }
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, _T("folder-desktop.svg")));
        break;
    case VirtualDirectoryType::kDocuments:
        filePath = userHomeDir;
        filePath += _T("Documents");
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
            filePath += GlobalManager::GetTextById(_T("STRID_PUBLIC_DIRECTORY_TREE_DOCUMENTS"));
        }
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
        }
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, _T("folder-documents.svg")));
        break;
    case VirtualDirectoryType::kPictures:
        filePath = userHomeDir;
        filePath += _T("Pictures");
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
            filePath += GlobalManager::GetTextById(_T("STRID_PUBLIC_DIRECTORY_TREE_PICTURES"));
        }
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
        }
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, _T("folder-pictures.svg")));
        break;
    case VirtualDirectoryType::kMusic:
        filePath = userHomeDir;
        filePath += _T("Music");
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
            filePath += GlobalManager::GetTextById(_T("STRID_PUBLIC_DIRECTORY_TREE_MUSIC"));
        }
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
        }
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, _T("folder-music.svg")));
        break;
    case VirtualDirectoryType::kVideos:
        filePath = userHomeDir;
        filePath += _T("Videos");
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
            filePath += GlobalManager::GetTextById(_T("STRID_PUBLIC_DIRECTORY_TREE_VIDEOS"));
        }
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
        }
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, _T("folder-videos.svg")));
        break;
    case VirtualDirectoryType::kDownloads:
        filePath = userHomeDir;
        filePath += _T("Downloads");
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
            filePath += GlobalManager::GetTextById(_T("STRID_PUBLIC_DIRECTORY_TREE_DOWNLOADS"));
        }
        if (!filePath.IsExistsDirectory()) {
            filePath = userHomeDir;
        }
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, _T("folder-download.svg")));
        break;
    default:
        break;
    }
    return true;
}

void DirectoryTreeImpl::GetRootPathInfoList(bool bLargeIcon, std::vector<DirectoryTree::PathInfo>& pathInfoList)
{
    pathInfoList.clear();
    const std::filesystem::path rootPath("/");

    std::weak_ptr<WeakFlag> weakFlag;
    std::vector<DirectoryTree::DiskInfo> diskInfoList;
    GetDiskInfoList(weakFlag, bLargeIcon, diskInfoList);

    //根卷：使用卷名作为显示名称（如"Macintosh HD"），与"计算机"视图的磁盘卡片保持一致
    DirectoryTree::PathInfo pathInfo;
    pathInfo.m_bFolder = true;
    pathInfo.m_filePath = FilePath(rootPath.native());
    pathInfo.m_displayName = pathInfo.m_filePath.ToString();
    pathInfo.m_bIconShared = false;
    pathInfo.m_nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, _T("drive-harddisk.svg")));
    for (const DirectoryTree::DiskInfo& diskInfo : diskInfoList) {
        if (diskInfo.m_filePath == pathInfo.m_filePath) {
            if (!diskInfo.m_displayName.empty()) {
                pathInfo.m_displayName = diskInfo.m_displayName;
                pathInfo.m_bIconShared = diskInfo.m_bIconShared;
                pathInfo.m_nIconID = diskInfo.m_nIconID;
            }
            break;
        }
    }

    //根卷放在列表首位，其后是其他挂载卷
    pathInfoList.push_back(pathInfo);
    for (const DirectoryTree::DiskInfo& diskInfo : diskInfoList) {
        if (diskInfo.m_filePath == pathInfo.m_filePath) {
            continue;
        }

        DirectoryTree::PathInfo subPathInfo;
        subPathInfo.m_bFolder = true;
        subPathInfo.m_filePath = diskInfo.m_filePath;
        subPathInfo.m_displayName = diskInfo.m_volumeName;
        subPathInfo.m_bIconShared = diskInfo.m_bIconShared;
        subPathInfo.m_nIconID = diskInfo.m_nIconID;

        pathInfoList.push_back(subPathInfo);
    }
}

void DirectoryTreeImpl::GetFolderContents(const FilePath& path,
                                          const std::weak_ptr<WeakFlag>& weakFlag,
                                          bool bLargeIcon,
                                          std::vector<DirectoryTree::PathInfo>& folderList,
                                          std::vector<DirectoryTree::PathInfo>* fileList)
{
    folderList.clear();
    if (fileList != nullptr) {
        fileList->clear();
    }
    try {
        std::filesystem::path stdPath(path.NativePathA());
        for (const auto& entry : std::filesystem::directory_iterator(stdPath)) {
            if (weakFlag.expired()) {
                //取消
                break;
            }
            if ((m_pTree != nullptr) && !m_pTree->IsShowHidenFiles()) {
                if (entry.path().has_filename()) {
                    const std::filesystem::path::string_type s = entry.path().filename().native();
                    if (!s.empty() && (s[0] == '.')) {
                        //不显示隐藏文件
                        continue;
                    }
                }
            }

            std::error_code errorCode;
            bool bSymlink = entry.is_symlink(errorCode);
            if (bSymlink) {
                //不支持symlink
                continue;
            }

            bool bDirectory = entry.is_directory(errorCode);
            if (bDirectory) {
                //目录
                DirectoryTree::PathInfo pathInfo;
                pathInfo.m_bFolder = true;
                pathInfo.m_filePath = FilePath(entry.path().native());
                pathInfo.m_displayName = pathInfo.m_filePath.GetFileName();

                if (bLargeIcon) {
                    if (m_impl->m_nLargeFolderIconID == 0) {
                        m_impl->m_nLargeFolderIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, _T("folder.svg")));
                    }
                    pathInfo.m_nIconID = m_impl->m_nLargeFolderIconID;
                }
                else {
                    if (m_impl->m_nSmallFolderIconID == 0) {
                        m_impl->m_nSmallFolderIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, _T("folder.svg"))); 
                    }
                    pathInfo.m_nIconID = m_impl->m_nSmallFolderIconID;
                }                
                pathInfo.m_bIconShared = true;
                struct stat buf{0, };
                int result = ::stat(entry.path().native().c_str(), &buf);
                if (result == 0) {
                    //目录的最后修改时间
                    pathInfo.m_lastWriteTime.FromSecondsSinceEpoch(buf.st_mtime);
                }
                folderList.emplace_back(std::move(pathInfo));
            }
            else if (fileList != nullptr) {
                bool bRegularFile = entry.is_regular_file(errorCode);
                if (bRegularFile) {
                    //普通文件
                    DirectoryTree::PathInfo pathInfo;
                    pathInfo.m_bFolder = false;
                    pathInfo.m_filePath = FilePath(entry.path().native());
                    pathInfo.m_displayName = pathInfo.m_filePath.GetFileName();

                    if (bLargeIcon) {
                        if (m_impl->m_nLargeFileIconID == 0) {
                            m_impl->m_nLargeFileIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, _T("file.svg")));
                        }
                        pathInfo.m_nIconID = m_impl->m_nLargeFileIconID;
                    }
                    else {
                        if (m_impl->m_nSmallFileIconID == 0) {
                            m_impl->m_nSmallFileIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, _T("file.svg")));
                        }
                        pathInfo.m_nIconID = m_impl->m_nSmallFileIconID;
                    }
                    pathInfo.m_bIconShared = true;

                    //文件的最后修改时间和文件大小
                    struct stat buf{0, };
                    int result = ::stat(entry.path().native().c_str(), &buf);
                    if (result == 0) {
                        pathInfo.m_fileSize = buf.st_size;
                        pathInfo.m_lastWriteTime.FromSecondsSinceEpoch(buf.st_mtime);
                    }
                    fileList->emplace_back(std::move(pathInfo));
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& /*e*/) {
        ASSERT(0);
    }
}

bool DirectoryTreeImpl::NeedShowDirPath(const FilePath& path) const
{
    if ((m_pTree == nullptr) || path.IsEmpty()) {
        return false;
    }
    if (!path.IsExistsDirectory()) {
        return false;
    }

    if (!m_pTree->IsShowHidenFiles()) {
        DString s = path.NativePath();
        if (!s.empty() && (s[0] == '.')) {
            //不显示隐藏文件
            return false;
        }
    }
    return true;
}

uint32_t DirectoryTreeImpl::GetMyComputerIconID() const
{
    return GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, _T("computer.svg")));
}
void DirectoryTreeImpl::GetDiskInfoList(const std::weak_ptr<WeakFlag>& /*weakFlag*/,
                                        bool bLargeIcon,
                                        std::vector<DirectoryTree::DiskInfo>& diskInfoList)
{
    diskInfoList.clear();

    //使用macOS的系统API获取挂载的卷（getmntinfo返回的缓冲区由系统管理，无需释放）
    struct statfs* pMountBuf = nullptr;
    int nMountCount = ::getmntinfo(&pMountBuf, MNT_NOWAIT);
    if ((nMountCount <= 0) || (pMountBuf == nullptr)) {
        return;
    }

    for (int i = 0; i < nMountCount; ++i) {
        const struct statfs& fs = pMountBuf[i];
        const std::string mountPoint(fs.f_mntonname);
        const std::string fsType(fs.f_fstypename);

        //跳过虚拟文件系统
        if ((fsType == "devfs") || (fsType == "autofs") || (fsType == "nullfs") ||
            (fsType == "procfs") || (fsType == "tmpfs")) {
            continue;
        }

        //跳过Finder中不显示的卷（nobrowse标志，如Recovery、VM、Preboot等系统卷）
        if (fs.f_flags & MNT_DONTBROWSE) {
            continue;
        }

        //跳过macOS的firmlink系统内部卷（/System/Volumes/Data、Preboot、VM等），不在"计算机"中显示
        if (mountPoint.rfind("/System/Volumes/", 0) == 0) {
            continue;
        }
        if (mountPoint == "/dev") {
            continue;
        }

        //计算磁盘使用情况
        uint64_t total = static_cast<uint64_t>(fs.f_blocks) * fs.f_bsize;
        uint64_t avail = static_cast<uint64_t>(fs.f_bavail) * fs.f_bsize;
        if (total == 0) {
            continue;
        }

        //判断设备类型
        DirectoryTree::DeviceType deviceType = DirectoryTree::DeviceType::HDD;
        if ((fsType == "nfs") || (fsType == "smbfs") || (fsType == "afpfs") || (fsType == "webdavfs")) {
            //网络磁盘
            deviceType = DirectoryTree::DeviceType::SHARE;
        }
        else if ((fsType == "cd9660") || (fsType == "udf")) {
            //光盘
            deviceType = DirectoryTree::DeviceType::CDROM;
        }
        else if ((mountPoint.rfind("/Volumes/", 0) == 0) && (mountPoint != "/Volumes")) {
            //内置卷只挂在"/"，/Volumes下的卷均为外接设备（U盘、移动硬盘、挂载的dmg等）
            deviceType = DirectoryTree::DeviceType::USB;
        }

        //显示名称：根卷显示为"Macintosh HD"，其他卷取挂载点末段名称
        std::string displayName;
        if (mountPoint == "/") {
            displayName = "Macintosh HD";
        }
        else {
            size_t nPos = mountPoint.find_last_of('/');
            displayName = (nPos == std::string::npos) ? mountPoint : mountPoint.substr(nPos + 1);
        }

        DirectoryTree::DiskInfo diskInfo;
        diskInfo.m_displayName = StringConvert::UTF8ToT(displayName);
        diskInfo.m_filePath = FilePath(StringConvert::UTF8ToT(mountPoint));
        diskInfo.m_volumeName = diskInfo.m_displayName;
        diskInfo.m_deviceType = deviceType;
        diskInfo.m_mountOn = StringConvert::UTF8ToT(mountPoint);
        diskInfo.m_fileSystem = StringConvert::UTF8ToT(fsType);
        diskInfo.m_totalBytes = total;
        diskInfo.m_freeBytes = avail;

        diskInfo.m_bIconShared = false;
        diskInfo.m_nIconID = 0;
        if (diskInfo.m_deviceType == DirectoryTree::DeviceType::CDROM) {
            diskInfo.m_nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, _T("drive-cdrom.svg")));
        }
        else if (diskInfo.m_deviceType == DirectoryTree::DeviceType::USB) {
            diskInfo.m_nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, _T("drive-harddisk-usb.svg")));
        }
        else {
            diskInfo.m_nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, _T("drive-harddisk.svg")));
        }

        diskInfoList.emplace_back(std::move(diskInfo));
    }
}

} //namespace ui

#endif //DUILIB_BUILD_FOR_MACOS
