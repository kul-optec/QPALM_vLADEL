#pragma once

#include <string>
#include <typeinfo>

namespace qpalm {

/// Get the pretty name of the given type as a string.
std::string demangled_typename(const std::type_info &t);

} // namespace qpalm
