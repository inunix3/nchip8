// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace nchip8::utils {
    template<typename T>
    bool divisibleBy(T a, T b) {
        return !(a % b);
    }

    // This function is agnostic to the system endianess
    inline std::uint16_t swapEndianess(std::uint16_t value) {
        std::uint8_t highByte = (value & 0xff00) >> 8;
        std::uint8_t  lowByte = (value & 0x00ff);

        return (std::uint16_t) (lowByte << 8) | highByte;
    }

    template<typename T, int NumOfPlaces = sizeof(T) * 2>
    std::string toHex(T number) {
        std::stringstream stream;

        stream << std::setfill('0') << std::setw(NumOfPlaces) << std::hex;

        // cout prints std::uint8_t as character literal (0x41 -> 'A')
        if constexpr (std::is_same_v<std::uint8_t, T>) {
            stream << (int) number;
        } else {
            stream << number;
        }

        return stream.str();
    }

    template<typename T, int NumOfPlaces = sizeof(T) * 2>
    std::string toHexUpper(T number) {
        std::stringstream stream;
        stream << toHex<T, NumOfPlaces>(number);

        std::string str = stream.str();
        std::transform(str.begin(), str.end(), str.begin(), [](auto ch) { return std::toupper(ch); });

        return str;
    }

    template<typename T, int NumOfPlaces = sizeof(T) * 2>
    std::string toHexPrefixed(T number) {
        return "0x" + toHex<T, NumOfPlaces>(number);
    }
}
