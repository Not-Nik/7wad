// 7wad (c) Nikolas Wipper 2022

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Archive.h"
#include "File.h"

#include <zlib.h>
#include <utility>
#include <iostream>

#define DEBUG(...) std::cout << __VA_ARGS__ << std::endl
#define ERROR(...) std::cerr << __VA_ARGS__ << std::endl

Archive::Archive(fs::path ap)
    : archive_file(ap) {
    DEBUG("Opening KIWAD archive at " << ap);

    std::string magic = ReadString(5);
    if (magic != "KIWAD") {
        ERROR("WAD archive has invalid magic. Aborting...");
        exit(0);
    }

    auto version = Read<uint32_t>();
    auto file_count = Read<uint32_t>();

    DEBUG("Archive is at version " << version);
    DEBUG("Archive contains " << file_count << " files");
    if (version >= 2) {
        Read<uint8_t>(); // read null byte
    }

    for (uint32_t i = 0; i < file_count; i++) {
        std::streamoff file_start = archive_file.tellg();
        // skip offset (4B), uncompressed size (4B), compressed size (4B), compressed? (1B), checksum (4B)
        archive_file.seekg(0x11, std::ios_base::cur);
        auto file_name_len = Read<uint32_t>();
        std::string file_name = ReadString(file_name_len);
        file_name.pop_back(); // Null terminator

        files.emplace(file_name, file_start);
    }

    DEBUG("Indexed " << files.size() << " files");
}

FileMetadata Archive::ReadMetadata(std::streamoff descriptor_offset) {
    archive_file.seekg(descriptor_offset, std::ios_base::beg);

    auto offset = Read<uint32_t>();
    auto size_uncompressed = Read<uint32_t>();
    auto size_compressed = Read<uint32_t>();
    bool is_compressed = Read<uint8_t>() != 0;
    auto checksum = Read<uint32_t>();

    std::string file_name = Read<std::string>();
    file_name.pop_back(); // Null terminator

    return FileMetadata{
        .data_offset = offset, .uncompressed_size = size_uncompressed, .compressed_size = size_compressed, .checksum = checksum, .is_compressed = is_compressed, .name = file_name
    };
}

std::vector<uint8_t> Archive::ReadFile(const FileMetadata& metadata) {
    archive_file.seekg(metadata.data_offset, std::ios_base::beg);

    std::vector<uint8_t> data;
    data.resize(metadata.uncompressed_size);

    if (metadata.is_compressed) {
        std::vector<uint8_t> compressed_data;
        compressed_data.resize(metadata.compressed_size);

        archive_file.read((char *) compressed_data.data(), metadata.compressed_size);

        uLongf s = metadata.uncompressed_size;
        int32_t err = uncompress(data.data(), &s, compressed_data.data(), metadata.compressed_size);

        if (err != Z_OK) {
            ERROR("Failed to uncompress " << metadata.name);
            data.clear();
        } else {
            DEBUG("Loaded and uncompressed " << metadata.name);
        }
    } else {
        archive_file.read((char *) data.data(), metadata.uncompressed_size);
        DEBUG("Loaded " << metadata.name);
    }

    return data;
}

std::optional<File> Archive::OpenFile(const std::string &file_name) {
    if (files.count(file_name) == 0) {
        ERROR("Tried to open " << file_name << ", but file doesn't exist in archive");
        return {};
    }

    std::streampos descriptor = files.at(file_name);
    FileMetadata metadata = ReadMetadata(descriptor);
    return {File(metadata, ReadFile(metadata))};
}

std::string Archive::ReadString(size_t n) {
    std::string res;
    res.resize(n);

    archive_file.read(res.data(), n);
    return res;
}
