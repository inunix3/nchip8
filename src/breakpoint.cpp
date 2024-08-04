// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/breakpoint.hpp>

using namespace nchip8;

void BreakpointMap::add(const Breakpoint &breakpoint) {
    if (has(breakpoint.offset)) {
        return;
    }

    m_strMap.insert({ breakpoint.name, breakpoint.offset });
    m_map.insert({ breakpoint.offset, breakpoint });
}

void BreakpointMap::remove(const std::string &name) {
    if (!has(name)) {
        return;
    }

    m_map.erase(m_strMap.at(name));
    m_strMap.erase(name);
}

void BreakpointMap::remove(std::uint16_t offset) {
    if (!has(offset)) {
        return;
    }

    std::string name = m_map.at(offset).name;
    m_strMap.erase(name);
    m_map.erase(offset);
}

void BreakpointMap::clear() {
    m_map.clear();
    m_strMap.clear();
}

Breakpoint &BreakpointMap::find(const std::string &name) {
    return m_map.at(m_strMap.at(name));
}

const Breakpoint &BreakpointMap::find(const std::string &name) const {
    return m_map.at(m_strMap.at(name));
}

Breakpoint &BreakpointMap::find(std::uint16_t offset) {
    return m_map.at(offset);
}

const Breakpoint &BreakpointMap::find(std::uint16_t offset) const {
    return m_map.at(offset);
}

bool BreakpointMap::has(const std::string &name) const {
    return m_strMap.count(name);
}

bool BreakpointMap::has(std::uint16_t offset) const {
    return m_map.count(offset);
}

bool BreakpointMap::empty() const {
    return m_map.empty();
}

BreakpointMap::_Iterator BreakpointMap::begin() {
    return m_map.begin();
}

BreakpointMap::_ConstIterator BreakpointMap::begin() const {
    return m_map.begin();
}

BreakpointMap::_Iterator BreakpointMap::end() {
    return m_map.end();
}

BreakpointMap::_ConstIterator BreakpointMap::end() const {
    return m_map.end();
}
