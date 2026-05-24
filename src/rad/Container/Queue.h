#pragma once

#include <rad/Common/Platform.h>

#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>

namespace rad
{

// QueueSPSC is a lock-free single-producer/single-consumer queue with runtime capacity.
// https://www.boost.org/doc/libs/latest/doc/html/boost/lockfree/spsc_queue.html
template <typename T>
using QueueSPSC = boost::lockfree::spsc_queue<T>;

// QueueMPMC is a lock-free multi-producer/multi-consumer queue with runtime capacity.
// https://www.boost.org/doc/libs/latest/doc/html/boost/lockfree/queue.html
template <typename T>
using QueueMPMC = boost::lockfree::queue<T>;

} // namespace rad
