#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/String.h>

#include <vector>

namespace rad
{

std::vector<std::string> get_exec_path();

std::string getlogin();

int getpid();

} // namespace rad
