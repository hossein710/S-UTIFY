#include "MusicLibrary.hpp"

void MusicLibrary::addSong(std::unique_ptr<Song> song) {
    if (song) {
        Song* raw = song.get();
        songs_.push_back(std::move(song));
        pathToSong_[raw->getFilePath()] = raw;
    }
}

Song* MusicLibrary::getSongByPath(const std::string& path) const {
    auto it = pathToSong_.find(path);
    return (it != pathToSong_.end()) ? it->second : nullptr;
}

const std::vector<std::unique_ptr<Song>>& MusicLibrary::getAllSongs() const {
    return songs_;
}