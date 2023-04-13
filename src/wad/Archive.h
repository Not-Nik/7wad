// 7wad (c) Nikolas Wipper 2022

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef WAD7_SRC_WAD_ARCHIVE_H_
#define WAD7_SRC_WAD_ARCHIVE_H_

#include <map>
#include <fstream>
#include <utility>
#include <optional>

#include "File.h"

namespace fs = std::filesystem;

/// In-memory representation of a WAD Archive
class Archive {
    std::ifstream archive_file;

public:
    uint32_t version = 0;
    /// Offsets for each file in the archive
    std::map<std::string, std::ifstream::pos_type> files{};

    FileMetadata ReadMetadata(std::ifstream::pos_type descriptor_offset);
    std::vector<uint8_t> ReadFile(const FileMetadata& metadata);

    Archive(const fs::path& archive_path);

    static std::optional<Archive> Open(const std::string &archive_path);

    std::optional<File> OpenFile(const std::string &file_name);
private:
    template <class T>
    T Read();
    template <>
    std::string Read<std::string>() { return ReadString(Read<uint32_t>()); }

    std::string ReadString(size_t n);
    template <class T>
    std::vector<T> ReadVector(size_t n);
};

template <size_t N>
struct SizedUint;
template <>
struct SizedUint<1> { typedef uint8_t type; };
template <>
struct SizedUint<2> { typedef uint16_t type; };
template <>
struct SizedUint<4> { typedef uint32_t type; };
template <>
struct SizedUint<8> { typedef uint64_t type; };

template <class T>
T Archive::Read() {
    static_assert(std::is_arithmetic_v<T>);

    typename SizedUint<sizeof(T)>::type res = 0;

    for (size_t i = 0; i < sizeof(T); i++) {
        uint8_t byte;
        archive_file.read((char *) &byte, 1);

        res |= ((typename SizedUint<sizeof(T)>::type) byte) << i * 8;
    }

    return *((T *) &res);
}

template <class U>
std::vector<U> Archive::ReadVector(size_t n) {
    std::vector<U> res;
    res.reserve(n);

    for (uint32_t i = 0; i < n; i++) res.push_back(Read<U>());
    return res;
}

#endif //WAD7_SRC_WAD_ARCHIVE_H_
