#pragma once
#include <string>
#include <vector>
#include "Playlist.hpp"
#include "MusicLibrary.hpp"

class M3uLoader {
public:
    static std::vector<Playlist> loadAll(const std::string& folder, MusicLibrary& library);
    static bool savePlaylist(const std::string& folder, const Playlist& playlist);
};