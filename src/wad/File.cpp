// 7wad (c) Nikolas Wipper 2022

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "File.h"

#include <fstream>

File::File(FileMetadata md, std::vector<uint8_t> data)
    : metadata_(md), data_(std::move(data)) {
}

void File::WriteToDisk(const std::string &filename) const {
    std::ofstream out(filename, std::ios::binary);

    if (!out.is_open()) return;

    std::ostream_iterator<uint8_t> output_iterator(out);
    std::copy(data_.begin(), data_.end(), output_iterator);
}