// 7wad (c) Nikolas Wipper 2022

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef WAD7_SRC_WAD_FILE_H_
#define WAD7_SRC_WAD_FILE_H_

#include <ios>
#include <string>
#include <vector>

class FileMetadata {
public:
    uint32_t data_offset, uncompressed_size, compressed_size, checksum;
    bool is_compressed;
    std::string name;
};

/// In-memory representation of a parsed file from a WAD Archive
class File {
    friend class Archive;

    std::vector<uint8_t> data_;
    uint32_t data_ptr_ = 0;

    FileMetadata metadata_;

public:
    typedef uint32_t streamoff;

    void read(char *s, std::streamsize n);
    void seekg(streamoff off) { data_ptr_ += off; }
    void WriteToDisk(const std::string &filename) const;

    File(FileMetadata md, std::vector<uint8_t> data);
};

#endif //WAD7_SRC_WAD_FILE_H_
