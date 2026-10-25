#pragma once

#include <stdexcept>

namespace treegen {

struct Error : std::runtime_error { using std::runtime_error::runtime_error; };
struct LoadError : Error { using Error::Error; };
struct RewriteError : Error { using Error::Error; };
struct InterpretError : Error { using Error::Error; };

}
