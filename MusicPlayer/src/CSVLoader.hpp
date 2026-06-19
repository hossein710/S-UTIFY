#pragma once
#include <string>
#include "MusicLibrary.hpp"

class CsvLoader {
public:
    static void loadLibrary(const std::string& filename, MusicLibrary& library);
};