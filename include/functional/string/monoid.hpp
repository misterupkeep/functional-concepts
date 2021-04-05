#pragma once

// String has overloaded operator+, making it a semigroup

#include <monoid.hpp>

#include <string>
template<> auto mempty<std::string> = std::string("a");
