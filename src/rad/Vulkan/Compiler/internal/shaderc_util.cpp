#include <rad/Common/MemoryDebug.h>

#include "shaderc_util.h"

#include <cassert>

#include <fstream>
#include <mutex>

namespace
{

// Returns "" if path is empty or ends in '/'.  Otherwise, returns "/".
std::string MaybeSlash(const shaderc_util::string_piece& path)
{
    return (path.empty() || path.back() == '/') ? "" : "/";
}

} // anonymous namespace

namespace shaderc_util
{

std::string FileFinder::FindReadableFilepath(const std::string& filename) const
{
    assert(!filename.empty());
    static const auto for_reading = std::ios_base::in;
    std::filebuf opener;
    for (const auto& prefix : search_path_)
    {
        const std::string prefixed_filename = prefix + MaybeSlash(prefix) + filename;
        if (opener.open(prefixed_filename, for_reading))
            return prefixed_filename;
    }
    return "";
}

std::string FileFinder::FindRelativeReadableFilepath(const std::string& requesting_file,
                                                     const std::string& filename) const
{
    assert(!filename.empty());

    string_piece dir_name(requesting_file);

    size_t last_slash = requesting_file.find_last_of("/\\");
    if (last_slash != std::string::npos)
    {
        dir_name = string_piece(requesting_file.c_str(), requesting_file.c_str() + last_slash);
    }

    if (dir_name.size() == requesting_file.size())
    {
        dir_name = {};
    }

    static const auto for_reading = std::ios_base::in;
    std::filebuf opener;
    const std::string relative_filename = dir_name.data() + MaybeSlash(dir_name) + filename;
    if (opener.open(relative_filename, for_reading))
        return relative_filename;

    return FindReadableFilepath(filename);
}

// https://github.com/google/shaderc/blob/main/libshaderc_util/src/io_shaderc.cc
bool ReadFile(const std::string& input_file_name, std::vector<char>* input_data)
{
    std::ifstream input_file;
    input_file.open(input_file_name, std::ios_base::binary);
    std::istream* stream = &input_file;
    if (input_file.fail())
    {
        return false;
    }
    *input_data = std::vector<char>((std::istreambuf_iterator<char>(*stream)),
                                    std::istreambuf_iterator<char>());
    return true;
}

} // namespace shaderc_util
