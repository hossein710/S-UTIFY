#include "M3uLoader.hpp"
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <cstring>

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<Playlist> M3uLoader::loadAll(const std::string& folder, MusicLibrary& library) {
    std::vector<Playlist> playlists;
    DIR* dir = opendir(folder.c_str());
    if (!dir) {
        std::cerr << "Warning: Could not open directory " << folder << std::endl;
        return playlists;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        if (!endsWith(name, ".m3u")) continue;

        std::string filepath = folder + "/" + name;
        Playlist playlist(name.substr(0, name.find_last_of('.')));
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open " << filepath << std::endl;
            continue;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::string path = trim(line);
            if (path.empty()) continue;
            Song* song = library.getSongByPath(path);
            if (song) {
                playlist.addSong(song);
            } else {
                std::cerr << "Warning: Unknown file path in " << name << ": " << path << std::endl;
            }
        }
        playlists.push_back(std::move(playlist));
    }
    closedir(dir);
    return playlists;
}

bool M3uLoader::savePlaylist(const std::string& folder, const Playlist& playlist) {
    std::string filename = folder + "/" + playlist.getName() + ".m3u";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not save playlist to " << filename << std::endl;
        return false;
    }
    for (Song* song : playlist.getSongs()) {
        file << song->getFilePath() << "\n";
    }
    return true;
}