#include "Playlist.hpp"
#include <algorithm>

Playlist::Playlist(std::string name) : name_(std::move(name)) {}

void Playlist::addSong(Song* song) {
    if (song) songs_.push_back(song);
}

void Playlist::removeSong(Song* song) {
    auto it = std::find(songs_.begin(), songs_.end(), song);
    if (it != songs_.end()) songs_.erase(it);
}

const std::string& Playlist::getName() const { return name_; }
size_t Playlist::size() const { return songs_.size(); }
Song* Playlist::getSong(size_t index) const {
    return (index < songs_.size()) ? songs_[index] : nullptr;
}
const std::vector<Song*>& Playlist::getSongs() const { return songs_; }