#pragma once
#include <vector>
#include <string>
#include "Song.hpp"

class Playlist {
public:
    explicit Playlist(std::string name);
    void addSong(Song* song);
    void removeSong(Song* song);   // new
    const std::string& getName() const;
    size_t size() const;
    Song* getSong(size_t index) const;
    const std::vector<Song*>& getSongs() const;

private:
    std::string name_;
    std::vector<Song*> songs_;
};