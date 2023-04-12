// merlin (c) Nikolas Wipper 2022

/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MERLIN_SRC_UTIL_BINARYREADER_H_
#define MERLIN_SRC_UTIL_BINARYREADER_H_

#include <string>
#include <fstream>
#include <sstream>

#include "../../merlin/src/util/Util.h"
#include "../../merlin/src/util/Types.h"

namespace merlin {

template <class T>
class BinaryReader : public T {
public:
    using T::T;

    BinaryReader(const BinaryReader &) = delete;
    BinaryReader(BinaryReader &&) noexcept;

    template <class U>
    U Read();
    template <>
    std::string Read<std::string>() { return ReadString(Read<u32>()); }

    std::string ReadString(usize n);
    template <class U>
    std::vector<U> ReadVector(usize n);
};

using BinaryFile = BinaryReader<std::ifstream>;
using BinaryStream = BinaryReader<std::stringstream>;

template <class T>
BinaryReader<T>::BinaryReader(BinaryReader &&other) noexcept : T(std::move(other)) {}

template <class T>
template <class U>
U BinaryReader<T>::Read() {
    if constexpr (std::is_arithmetic_v<U>) {
        typename SizedUint<sizeof(U)>::type res = 0;

        for (usize i = 0; i < sizeof(U); i++) {
            u8 byte;
            this->read((char *) &byte, 1);

            res |= ((typename SizedUint<sizeof(U)>::type) byte) << i * 8;
        }

        return *((U*) &res);
    } else if (IsVector<U>::value) {
        return ReadVector<typename U::value_type>(Read<u32>());
    } else {
        static_assert(std::is_arithmetic_v<U> || IsVectorV<U>);
    }
}

template <class T>
std::string BinaryReader<T>::ReadString(usize n) {
    std::string res;
    res.resize(n);

    this->read(res.data(), n);
    return res;
}

template <class T>
template <class U>
std::vector<U> BinaryReader<T>::ReadVector(usize n) {
    std::vector<U> res;
    res.reserve(n);

    for (u32 i = 0; i < n; i++) res.push_back(Read<U>());
    return res;
}
} // merlin

#endif //MERLIN_SRC_UTIL_BINARYREADER_H_
