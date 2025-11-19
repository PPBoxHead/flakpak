#ifndef FLKFILE_HPP
#define FLKFILE_HPP

#include <vfspp/IFile.h>

extern "C" {
#include <flakpak-c/flak_definitions.h>
#include <flakpak-c/zstd_compressor.h>
#include <flakpak-c/xccp20_encryptor.h>
}

namespace vfspp
{

    using FLKFilePtr = std::shared_ptr<class FLKFile>;
    using FLKFileWeakPtr = std::weak_ptr<class FLKFile>;

    class FLKFile final : public IFile
    {
    public:
        FLKFile(const FileInfo& fileInfo,
            const FLK_entry_t& entry,
            std::ifstream& flkFile,
            const std::vector<uint8_t>& globalSalt,
            const std::string& password,
            FLK_file_flags flags)
            : m_FileInfo(fileInfo)
            , m_Entry(entry)
            , m_FLKFile(flkFile)
            , m_GlobalSalt(globalSalt)
            , m_Password(password)
            , m_Flags(flags)
            , m_IsOpened(false)
            , m_SeekPos(0)
        {
        }

        ~FLKFile()
        {
            Close();
        }

        virtual const FileInfo& GetFileInfo() const override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return GetFileInfoST();
            }
            else {
                return GetFileInfoST();
            }
        }

        virtual uint64_t Size() override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return SizeST();
            }
            else {
                return SizeST();
            }
        }

        virtual bool IsReadOnly() const override
        {
            return true;
        }

        virtual void Open(FileMode mode) override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                OpenST(mode);
            }
            else {
                OpenST(mode);
            }
        }

        virtual void Close() override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                CloseST();
            }
            else {
                CloseST();
            }
        }

        virtual bool IsOpened() const override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return IsOpenedST();
            }
            else {
                return IsOpenedST();
            }
        }

        virtual uint64_t Seek(uint64_t offset, Origin origin) override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return SeekST(offset, origin);
            }
            else {
                return SeekST(offset, origin);
            }
        }

        virtual uint64_t Tell() override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return TellST();
            }
            else {
                return TellST();
            }
        }

        virtual uint64_t Read(uint8_t* buffer, uint64_t size) override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return ReadST(buffer, size);
            }
            else {
                return ReadST(buffer, size);
            }
        }

        virtual uint64_t Write(const uint8_t* buffer, uint64_t size) override
        {
            return 0; // Read-only
        }

        virtual uint64_t Read(std::vector<uint8_t>& buffer, uint64_t size) override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return ReadST(buffer, size);
            }
            else {
                return ReadST(buffer, size);
            }
        }

        virtual uint64_t Write(const std::vector<uint8_t>& buffer) override
        {
            return 0; // Read-only
        }

        virtual uint64_t Read(std::ostream& stream, uint64_t size, uint64_t bufferSize = 1024) override
        {
            if constexpr (VFSPP_MT_SUPPORT_ENABLED) {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return ReadST(stream, size, bufferSize);
            }
            else {
                return ReadST(stream, size, bufferSize);
            }
        }

        virtual uint64_t Write(std::istream& stream, uint64_t size, uint64_t bufferSize = 1024) override
        {
            return 0; // Read-only
        }

    private:
        inline const FileInfo& GetFileInfoST() const
        {
            return m_FileInfo;
        }

        inline uint64_t SizeST()
        {
            return m_Entry.base_size;
        }

        inline void OpenST(FileMode mode)
        {
            // Check if requesting write access on read-only file
            bool requestWrite = ((mode & FileMode::Write) == FileMode::Write);
            requestWrite |= ((mode & FileMode::Append) == FileMode::Append);
            requestWrite |= ((mode & FileMode::Truncate) == FileMode::Truncate);

            if (requestWrite) {
                return; // Can't write to FLK files
            }

            if (IsOpenedST()) {
                SeekST(0, Origin::Begin);
                return;
            }

            try {
                // Seek to file data in FLK
                m_FLKFile.seekg(m_Entry.offset);
                if (!m_FLKFile) {
                    return;
                }

                // Read packed data (encrypted + compressed)
                std::vector<uint8_t> packedData(m_Entry.packed_size);
                m_FLKFile.read(reinterpret_cast<char*>(packedData.data()), m_Entry.packed_size);
                if (!m_FLKFile) {
                    return;
                }

                uint8_t* processedData = packedData.data();
                size_t processedSize = m_Entry.packed_size;

                // Decrypt if encrypted
                FLAK_DECRYPTION_RESULT decryptResult = { 0 };
                if ((m_Flags & FLK_FLAG_ENCRYPTED) && !m_GlobalSalt.empty()) {
                    decryptResult = FLAK_xccp20_decrypt_data(
                        m_FileInfo.Name().c_str(),
                        processedData,
                        processedSize,
                        m_Password.c_str()
                    );

                    if (!decryptResult.data || decryptResult.data_size == 0) {
                        return;
                    }

                    processedData = decryptResult.data;
                    processedSize = decryptResult.data_size;
                }

                // Decompress if compressed
                FLAK_DECOMPRESSION_RESULT decompResult = { 0 };
                if ((m_Flags & FLK_FLAG_COMPRESSED) && m_Entry.packed_size != m_Entry.base_size) {
                    decompResult = FLAK_zstd_decompress_data(
                        m_FileInfo.Name().c_str(),
                        processedData,
                        processedSize
                    );

                    if (!decompResult.data || decompResult.data_size == 0) {
                        if (decryptResult.data) free(decryptResult.data);
                        return;
                    }

                    // Free decrypted data if it was allocated
                    if (decryptResult.data) {
                        free(decryptResult.data);
                    }

                    m_Data.assign(decompResult.data, decompResult.data + decompResult.data_size);
                    free(decompResult.data);
                }
                else {
                    // No decompression needed
                    if (decryptResult.data) {
                        m_Data.assign(decryptResult.data, decryptResult.data + decryptResult.data_size);
                        free(decryptResult.data);
                    }
                    else {
                        m_Data = std::move(packedData);
                    }
                }

                m_SeekPos = 0;
                m_IsOpened = true;
            }
            catch (const std::exception&) {
                m_Data.clear();
                m_IsOpened = false;
            }
        }

        inline void CloseST()
        {
            m_IsOpened = false;
            m_SeekPos = 0;
            m_Data.clear();
        }

        inline bool IsOpenedST() const
        {
            return m_IsOpened;
        }

        inline uint64_t SeekST(uint64_t offset, Origin origin)
        {
            if (!IsOpenedST()) {
                return 0;
            }

            if (origin == Origin::Begin) {
                m_SeekPos = offset;
            }
            else if (origin == Origin::End) {
                m_SeekPos = (offset <= SizeST()) ? SizeST() - offset : 0;
            }
            else if (origin == Origin::Set) {
                m_SeekPos += offset;
            }
            m_SeekPos = std::min(m_SeekPos, SizeST());

            return TellST();
        }

        inline uint64_t TellST()
        {
            return m_SeekPos;
        }

        inline uint64_t ReadST(uint8_t* buffer, uint64_t size)
        {
            if (!IsOpenedST()) {
                return 0;
            }

            uint64_t leftSize = SizeST() - TellST();
            uint64_t maxSize = std::min(size, leftSize);
            if (maxSize > 0) {
                memcpy(buffer, m_Data.data() + m_SeekPos, static_cast<size_t>(maxSize));
                m_SeekPos += maxSize;
                return maxSize;
            }

            return 0;
        }

        inline uint64_t ReadST(std::vector<uint8_t>& buffer, uint64_t size)
        {
            buffer.resize(size);
            return ReadST(buffer.data(), size);
        }

        inline uint64_t ReadST(std::ostream& stream, uint64_t size, uint64_t bufferSize = 1024)
        {
            uint64_t totalSize = size;
            std::vector<uint8_t> buffer(bufferSize);
            while (size > 0) {
                uint64_t bytesRead = ReadST(buffer.data(), std::min(size, static_cast<uint64_t>(buffer.size())));
                if (bytesRead == 0) {
                    break;
                }

                if (size < bytesRead) {
                    bytesRead = size;
                }

                stream.write(reinterpret_cast<char*>(buffer.data()), bytesRead);
                size -= bytesRead;
            }

            return totalSize - size;
        }

    private:
        FileInfo m_FileInfo;
        const FLK_entry_t& m_Entry;
        std::ifstream& m_FLKFile;
        const std::vector<uint8_t>& m_GlobalSalt;
        std::string m_Password;
        FLK_file_flags m_Flags;

        std::vector<uint8_t> m_Data;
        bool m_IsOpened;
        uint64_t m_SeekPos;
        mutable std::mutex m_Mutex;
    };

} // namespace vfspp

#endif // FLKFILE_HPP