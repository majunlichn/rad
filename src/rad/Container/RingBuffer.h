#pragma once

#include <rad/Common/Platform.h>

#include <boost/circular_buffer.hpp>

namespace rad
{

// RingBuffer is a sequence container with runtime capacity and efficient insertion at either end.
// https://www.boost.org/doc/libs/latest/doc/html/circular_buffer.html
template <typename T>
using RingBuffer = boost::circular_buffer<T>;

} // namespace rad
