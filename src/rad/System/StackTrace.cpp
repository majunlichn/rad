#include <rad/System/StackTrace.h>

#include <backward.hpp>

namespace rad
{

std::string PrintStackTrace(int depth)
{
    // On Windows, must declare Printer before load_here, or the first print won't work, is it a bug?
    backward::Printer p;
    backward::StackTrace st;
    p.reverse = false;
    st.skip_n_firsts(2); // skip load_here and the current stack.
    st.load_here(depth);
    std::stringstream stream;
    p.print(st, stream);
    return stream.str();
}

} // namespace rad
