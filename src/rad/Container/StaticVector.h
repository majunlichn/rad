#pragma once

#include <rad/Common/Platform.h>

#include <boost/container/static_vector.hpp>

namespace rad
{

// StaticVector is a vector-like container with a fixed capacity.
// https://www.boost.org/doc/libs/latest/doc/html/container/non_standard_containers.html#container.non_standard_containers.static_vector
template <class T, std::size_t N>
using StaticVector = boost::container::static_vector<T, N>;

} // namespace rad
