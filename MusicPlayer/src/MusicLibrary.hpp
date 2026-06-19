#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "Song.hpp"

class MusicLibrary {
public:
    void addSong(std::unique_ptr<Song> song);
    Song* getSongByPath(const std::string& path) const;
    const std::vector<std::unique_ptr<Song>>& getAllSongs() const;

private:
    std::vector<std::unique_ptr<Song>> songs_;
    std::unordered_map<std::string, Song*> pathToSong_;  // for fast lookup
};