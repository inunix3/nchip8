// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace nchip8 {
    struct Breakpoint {
        std::string name;
        std::uint16_t offset;

        inline constexpr bool operator==(const Breakpoint &breakpoint) const {
            return name == breakpoint.name && offset == breakpoint.offset;
        }
    };

    class BreakpointMap {
    private:
        using _Breakpoints    = std::unordered_map<std::uint16_t, Breakpoint>;
        using _StrToOffsetMap = std::unordered_map<std::string, std::uint16_t>;
        using _Iterator       = _Breakpoints::iterator;
        using _ConstIterator  = _Breakpoints::const_iterator;

    public:
        void add(const Breakpoint &breakpoint);
        void remove(const std::string &name);
        void remove(std::uint16_t offset);
        void clear();

        Breakpoint &find(const std::string &name);
        const Breakpoint &find(const std::string &name) const;
        Breakpoint &find(std::uint16_t offset);
        const Breakpoint &find(std::uint16_t offset) const;

        bool has(const std::string &name) const;
        bool has(std::uint16_t offset) const;

        bool empty() const;

        _Iterator begin();
        _ConstIterator begin() const;

        _Iterator end();
        _ConstIterator end() const;

    private:
        _Breakpoints m_map;
        _StrToOffsetMap m_strMap;
    };
}
