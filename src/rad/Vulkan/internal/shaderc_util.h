#pragma once

#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>

namespace shaderc_util
{

using string_piece = std::string_view;

// https://github.com/google/shaderc/blob/main/libshaderc_util/include/libshaderc_util/file_finder.h
// https://github.com/google/shaderc/blob/main/libshaderc_util/src/file_finder.cc
// Finds files within a search path.
class FileFinder
{
public:
    // Searches for a read-openable file based on filename, which must be
    // non-empty.  The search is attempted on filename prefixed by each element of
    // search_path() in turn.  The first hit is returned, or an empty string if
    // there are no hits.  Search attempts treat their argument the way
    // std::fopen() treats its filename argument, ignoring whether the path is
    // absolute or relative.
    //
    // If a search_path() element is non-empty and not ending in a slash, then a
    // slash is inserted between it and filename before its search attempt. An
    // empty string in search_path() means that the filename is tried as-is.
    std::string FindReadableFilepath(const std::string& filename) const;

    // Searches for a read-openable file based on filename, which must be
    // non-empty. The search is first attempted as a path relative to
    // the requesting_file parameter. If no file is found relative to the
    // requesting_file then this acts as FindReadableFilepath does. If
    // requesting_file does not contain a '/' or a '\' character then it is
    // assumed to be a filename and the request will be relative to the
    // current directory.
    std::string FindRelativeReadableFilepath(const std::string& requesting_file,
                                             const std::string& filename) const;

    // Search path for Find().  Users may add/remove elements as desired.
    std::vector<std::string>& search_path() { return search_path_; }

private:
    std::vector<std::string> search_path_;
}; // class FileFinder

bool ReadFile(const std::string& input_file_name, std::vector<char>* input_data);

} // namespace shaderc_util
