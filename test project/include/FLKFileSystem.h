#ifndef FLKFILESYSTEM_HPP
#define FLKFILESYSTEM_HPP

#include <vfspp/IFileSystem.h>
#include <vfspp/Global.h>
#include <vfspp/StringUtils.hpp>

#include "FLKFile.h"

extern "C" {
#include <flakpak-c/flak_definitions.h>
#include <flakpak-c/flak_pswd_definition.h>
}

namespace fs = std::filesystem;

namespace vfspp
{

    using FLKFileSystemPtr = std::shared_ptr<class FLKFileSystem>;
    using FLKFileSystemWeakPtr = std::weak_ptr<class FLKFileSystem>;

    class FLKFileSystem final : public IFileSystem
    {
    public:
        FLKFileSystem(const std::string& flkPath)
            : m_FLKPath(flkPath)
            , m_IsInitialized(false)
            , m_Password(FLAK_get_password())
        {
        }

        ~FLKFileSystem()
        {
            Shutdown();
        }

        virtual void Initialize() override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                InitializeST();
            }
            else {
                InitializeST();
            }
        }

        virtual void Shutdown() override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                ShutdownST();
            }
            else {
                ShutdownST();
            }
        }

        virtual bool IsInitialized() const override
        {
            return m_IsInitialized;
        }

        virtual const std::string& BasePath() const override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return BasePathST();
            }
            else {
                return BasePathST();
            }
        }

        virtual const TFileList& FileList() const override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return FileListST();
            }
            else {
                return FileListST();
            }
        }

        virtual bool IsReadOnly() const override
        {
            return true; // FLK files are read-only
        }

        virtual IFilePtr OpenFile(const FileInfo& filePath, IFile::FileMode mode) override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return OpenFileST(filePath, mode);
            }
            else {
                return OpenFileST(filePath, mode);
            }
        }

        virtual void CloseFile(IFilePtr file) override
        {
            // NO-OP for FLK files
        }

        virtual bool CreateFile(const FileInfo& filePath) override
        {
            return false; // Read-only filesystem
        }

        virtual bool RemoveFile(const FileInfo& filePath) override
        {
            return false; // Read-only filesystem
        }

        virtual bool CopyFile(const FileInfo& src, const FileInfo& dest) override
        {
            return false; // Read-only filesystem
        }

        virtual bool RenameFile(const FileInfo& srcPath, const FileInfo& dstPath) override
        {
            return false; // Read-only filesystem
        }

        virtual bool IsFileExists(const FileInfo& filePath) const override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return IsFileExistsST(filePath);
            }
            else {
                return IsFileExistsST(filePath);
            }
        }

        virtual bool IsFile(const FileInfo& filePath) const override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return IFileSystem::IsFile(filePath, m_FileList);
            }
            else {
                return IFileSystem::IsFile(filePath, m_FileList);
            }
        }

        virtual bool IsDir(const FileInfo& dirPath) const override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return IFileSystem::IsDir(dirPath, m_FileList);
            }
            else {
                return IFileSystem::IsDir(dirPath, m_FileList);
            }
        }

    private:
        inline void InitializeST()
        {
            if (m_IsInitialized) {
                return;
            }

            if (!fs::is_regular_file(m_FLKPath)) {
                return;
            }

            m_FLKFile.open(m_FLKPath, std::ios::binary);
            if (!m_FLKFile) {
                return;
            }

            if (!ReadHeader()) {
                m_FLKFile.close();
                return;
            }

            BuildFileList();
            m_IsInitialized = true;
        }

        inline void ShutdownST()
        {
            for (auto& file : m_FileList) {
                file.second->Close();
            }
            m_FileList.clear();
            m_GlobalSalt.clear();

            if (m_FLKFile.is_open()) {
                m_FLKFile.close();
            }

            m_IsInitialized = false;
        }

        inline const std::string& BasePathST() const
        {
            static std::string rootPath = "/";
            return rootPath;
        }

        inline const TFileList& FileListST() const
        {
            return m_FileList;
        }

        inline IFilePtr OpenFileST(const FileInfo& filePath, IFile::FileMode mode)
        {
            // Check if requesting write access on read-only filesystem
            bool requestWrite = ((mode & IFile::FileMode::Write) == IFile::FileMode::Write);
            requestWrite |= ((mode & IFile::FileMode::Append) == IFile::FileMode::Append);
            requestWrite |= ((mode & IFile::FileMode::Truncate) == IFile::FileMode::Truncate);

            if (IsReadOnly() && requestWrite) {
                return nullptr;
            }

            IFilePtr file = FindFile(filePath, m_FileList);
            if (file) {
                file->Open(mode);
            }

            return file;
        }

        inline bool IsFileExistsST(const FileInfo& filePath) const
        {
            return FindFile(filePath, m_FileList) != nullptr;
        }

        bool ReadHeader()
        {
            // Read the entire header structure
            m_FLKFile.read(reinterpret_cast<char*>(&m_Header), sizeof(m_Header));
            if (!m_FLKFile) {
                return false;
            }

            // Verify magic number
            if (m_Header.magic[0] != 'F' || m_Header.magic[1] != 'L' ||
                m_Header.magic[2] != 'K' || m_Header.magic[3] != '\0') {
                return false;
            }

            // Read global salt if present
            if (m_Header.salt_lenght > 0) {
                m_GlobalSalt.resize(m_Header.salt_lenght);
                m_FLKFile.read(reinterpret_cast<char*>(m_GlobalSalt.data()), m_Header.salt_lenght);
                if (!m_FLKFile) {
                    return false;
                }
            }

            return true;
        }

        void BuildFileList()
        {
            // Iterate through valid entries in the header
            for (uint32_t i = 0; i < m_Header.entry_count && i < FLK_MAX_HEADER_ENTRIES; ++i) {
                const auto& entry = m_Header.entries[i];

                // Skip invalid entries (empty paths)
                if (entry.file_path[0] == '\0') {
                    continue;
                }

                std::string path(entry.file_path);
                FileInfo fileInfo(BasePathST(), path, false);

                IFilePtr file(new FLKFile(
                    fileInfo,
                    entry,
                    m_FLKFile,
                    m_GlobalSalt,
                    m_Password,
                    m_Header.flags
                ));

                m_FileList[fileInfo.AbsolutePath()] = file;
            }
        }

    private:
        std::string m_FLKPath;
        std::ifstream m_FLKFile;
        bool m_IsInitialized;
        FLK_header_t m_Header;
        std::vector<uint8_t> m_GlobalSalt;
        TFileList m_FileList;
        std::string m_Password;

        mutable std::mutex m_Mutex;
    };

} // namespace vfspp

#endif // FLKFILESYSTEM_HPP