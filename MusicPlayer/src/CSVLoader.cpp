#include "CsvLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

static std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(s);
    while (std::getline(stream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

void CsvLoader::loadLibrary(const std::string& filename, MusicLibrary& library) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filename << std::endl;
        return;
    }

    std::string line;
    // bool header = true;
    while (std::getline(file, line)) {
        // if (header) {
        //     header = false;
        //     continue;
        // }
        if (line.empty()) continue;

        auto tokens = split(line, ',');
        if (tokens.size() < 7) {
            std::cerr << "Warning: Skipping malformed line: " << line << std::endl;
            continue;
        }

        try {
            std::string title = tokens[0];
            std::string artist = tokens[1];
            std::string album = tokens[2];
            std::string genre = tokens[3];
            int year = std::stoi(tokens[4]);
            int duration = std::stoi(tokens[5]);
            std::string filePath = tokens[6];

            auto song = std::make_unique<Song>(title, artist, album, genre, year, duration, filePath);
            library.addSong(std::move(song));
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid data in line, skipping: " << line << std::endl;
        }
    }
}