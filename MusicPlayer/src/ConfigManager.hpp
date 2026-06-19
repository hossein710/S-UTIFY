#pragma once
#include <string>

struct Config {
    std::string activePlaylist;
    std::string playbackMode;  // "NO_REPEAT", "REPEAT_ONE", "REPEAT_ALL", "SHUFFLE"
    std::string lastSong;
    float lastPosition = 0.0f;   // resume position in seconds
};

class ConfigManager {
public:
    static Config load(const std::string& filename);
    static void save(const std::string& filename, const Config& config);
};