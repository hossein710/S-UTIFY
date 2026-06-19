#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

Config ConfigManager::load(const std::string& filename) {
    Config cfg;
    cfg.activePlaylist = "";
    cfg.playbackMode = "NO_REPEAT";
    cfg.lastSong = "";
    cfg.lastPosition = 0.0f;

    std::ifstream file(filename);
    if (!file.is_open()) return cfg;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));
        if (key == "active_playlist") cfg.activePlaylist = value;
        else if (key == "playback_mode") cfg.playbackMode = value;
        else if (key == "last_song") cfg.lastSong = value;
        else if (key == "last_position") cfg.lastPosition = std::stof(value);
    }
    return cfg;
}

void ConfigManager::save(const std::string& filename, const Config& config) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not save config to " << filename << std::endl;
        return;
    }
    file << "active_playlist=" << config.activePlaylist << "\n";
    file << "playback_mode=" << config.playbackMode << "\n";
    file << "last_song=" << config.lastSong << "\n";
    file << "last_position=" << config.lastPosition << "\n";
}