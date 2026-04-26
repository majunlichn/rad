#pragma once

#include <boost/outcome.hpp>

namespace rad
{

template <typename T, typename E>
using Result = boost::outcome_v2::result<T, E>;

} // namespace rad
