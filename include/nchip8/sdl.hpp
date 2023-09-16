// Copyright (c) 2023-present, eaxecx.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#pragma once

#include <SDL.h>

#ifdef __clang__
# pragma clang diagnostic push

# pragma clang diagnostic ignored "-Wimplicit-int-conversion"
# include <SDL2pp/SDL2pp.hh>

# pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
# pragma GCC diagnostic push

# pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
# include <SDL2pp/SDL2pp.hh>

# pragma GCC diagnostic pop
#else
# include <SDL2pp/SDL2pp.hh>
#endif

namespace sdl = SDL2pp;
