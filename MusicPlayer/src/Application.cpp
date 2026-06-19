#include "Application.hpp"
#include "MainMenuScreen.hpp"
#include "NowPlayingScreen.hpp"
#include "CsvLoader.hpp"
#include "M3uLoader.hpp"
#include "Terminal.hpp"

#include <iostream>
#include <memory>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>

Application::Application() : player_(library_) {
    loadData();
    config_ = ConfigManager::load("../Data/settings.cfg");

    // Restore active playlist from config
    if (!config_.activePlaylist.empty()) {
        for (auto& pl : playlists_) {
            if (pl.getName() == config_.activePlaylist) {
                activePlaylist_ = &pl;
                break;
            }
        }
    }
    if (!activePlaylist_ && !playlists_.empty())
        activePlaylist_ = &playlists_[0];

    player_.setPlaylist(activePlaylist_);

    // Restore playback mode
    if (config_.playbackMode == "NO_REPEAT")
        player_.setPlaybackMode(Player::NO_REPEAT);
    else if (config_.playbackMode == "REPEAT_ONE")
        player_.setPlaybackMode(Player::REPEAT_ONE);
    else if (config_.playbackMode == "REPEAT_ALL")
        player_.setPlaybackMode(Player::REPEAT_ALL);
    else if (config_.playbackMode == "SHUFFLE")
        player_.setPlaybackMode(Player::SHUFFLE);

    // Load user data (favorites & play counts)
    loadUserData();

    // Set callback for play count increment
    player_.setOnPlayCountIncrement([this](Song* s) { incrementPlayCount(s); });

    // Resume last song if config has it
    if (!config_.lastSong.empty() && config_.lastPosition > 0.0f) {
        Song* song = library_.getSongByPath(config_.lastSong);
        if (song) {
            // Find playlist containing this song
            Playlist* targetPlaylist = nullptr;
            for (auto& pl : playlists_) {
                for (Song* s : pl.getSongs()) {
                    if (s == song) {
                        targetPlaylist = &pl;
                        break;
                    }
                }
                if (targetPlaylist) break;
            }
            if (targetPlaylist) {
                if (targetPlaylist != activePlaylist_) {
                    activePlaylist_ = targetPlaylist;
                    player_.setPlaylist(activePlaylist_);
                }
                // Load and seek
                player_.setCurrentSong(song, config_.lastPosition);
            }
        }
    }
}

Application::~Application() {
    Terminal::shutdown();
}

void Application::loadData() {
    CsvLoader::loadLibrary("../Data/Library.csv", library_);
    playlists_ = M3uLoader::loadAll("../Data/Playlists", library_);
}

void Application::run() {
    Terminal::init();
    switchScreen(std::make_unique<MainMenuScreen>(*this));

    while (running_) {
        if (currentScreen_) {
            currentScreen_->render();
            currentScreen_->handleInput();
        }
        player_.tick();
        usleep(50000);
    }
    saveConfig();
}

void Application::switchScreen(std::unique_ptr<Screen> screen) {
    if (currentScreen_) currentScreen_->onExit();
    currentScreen_ = std::move(screen);
    if (currentScreen_) currentScreen_->onEnter();
}

void Application::goBack() {
    switchScreen(std::make_unique<MainMenuScreen>(*this));
}

void Application::saveConfig() {
    config_.playbackMode = player_.getPlaybackModeStr();
    if (player_.getCurrentSong())
        config_.lastSong = player_.getCurrentSong()->getFilePath();
    else
        config_.lastSong = "";
    // Save current position if playing or paused
    if (player_.getState() != Player::STOPPED) {
        config_.lastPosition = player_.getCurrentTimeSec();
    } else {
        config_.lastPosition = 0.0f;
    }
    ConfigManager::save("../Data/settings.cfg", config_);
    saveUserData();  // also save user data
}

Config& Application::getConfig() { return config_; }
Player& Application::getPlayer() { return player_; }
std::vector<Playlist>& Application::getPlaylists() { return playlists_; }
Playlist* Application::getActivePlaylist() { return activePlaylist_; }
MusicLibrary& Application::getLibrary() { return library_; }

void Application::setActivePlaylist(Playlist* pl) {
    if (pl && pl != activePlaylist_) {
        activePlaylist_ = pl;
        player_.stop();
        player_.setPlaylist(activePlaylist_);
        config_.activePlaylist = activePlaylist_->getName();
        saveConfig();
    }
}

// ---- User data ----
std::string Application::getUserDataFilePath() const {
    return "../Data/userdata.dat";
}

void Application::loadUserData() {
    std::string path = getUserDataFilePath();
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string key = line.substr(0, colon);
        std::string rest = line.substr(colon + 1);
        if (key == "favorite") {
            Song* song = library_.getSongByPath(rest);
            if (song) song->setFavorite(true);
        } else if (key == "playcount") {
            size_t eq = rest.find('=');
            if (eq != std::string::npos) {
                std::string path = rest.substr(0, eq);
                int count = std::stoi(rest.substr(eq + 1));
                Song* song = library_.getSongByPath(path);
                if (song) song->setPlayCount(count);
            }
        }
    }
}

void Application::saveUserData() {
    std::string path = getUserDataFilePath();
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not save user data to " << path << std::endl;
        return;
    }
    for (const auto& ptr : library_.getAllSongs()) {
        Song* s = ptr.get();
        if (s->isFavorite())
            file << "favorite:" << s->getFilePath() << "\n";
        if (s->getPlayCount() > 0)
            file << "playcount:" << s->getFilePath() << "=" << s->getPlayCount() << "\n";
    }
}

void Application::toggleFavorite(Song* song) {
    if (!song) return;
    song->setFavorite(!song->isFavorite());
    saveUserData();
}

void Application::incrementPlayCount(Song* song) {
    if (!song) return;
    song->incrementPlayCount();
    saveUserData();
}

void Application::createPlaylist(const std::string& name) {
    // Avoid duplicates
    for (const auto& pl : playlists_) {
        if (pl.getName() == name) return;
    }
    Playlist newPl(name);
    playlists_.push_back(std::move(newPl));
    M3uLoader::savePlaylist("../Data/Playlists", playlists_.back());
    // Optionally set as active? Not by default.
}

void Application::deletePlaylist(Playlist* pl) {
    if (!pl) return;
    // If this is the active playlist, change active to first or null
    if (activePlaylist_ == pl) {
        activePlaylist_ = nullptr;
        for (auto& p : playlists_) {
            if (&p != pl) {
                activePlaylist_ = &p;
                break;
            }
        }
        if (activePlaylist_) {
            player_.setPlaylist(activePlaylist_);
            config_.activePlaylist = activePlaylist_->getName();
        } else {
            player_.setPlaylist(nullptr);
            config_.activePlaylist = "";
        }
    }
    // Remove from vector
    auto it = std::find_if(playlists_.begin(), playlists_.end(),
                           [pl](const Playlist& p) { return &p == pl; });
    if (it != playlists_.end()) {
        std::string filename = "../Data/Playlists/" + it->getName() + ".m3u";
        remove(filename.c_str());  // delete file
        playlists_.erase(it);
    }
    saveConfig();
}

void Application::addSongToPlaylist(Playlist* pl, Song* song) {
    if (!pl || !song) return;
    // Check if already in playlist
    for (Song* s : pl->getSongs()) {
        if (s == song) return;
    }
    pl->addSong(song);
    M3uLoader::savePlaylist("../Data/Playlists", *pl);
}

void Application::removeSongFromPlaylist(Playlist* pl, Song* song) {
    if (!pl || !song) return;
    auto& songs = pl->getSongs(); // but getSongs returns const, we need mutable
    // We'll modify the playlist directly. We'll need to access private songs_? 
    // We can add a method to Playlist to remove a song.
    // Alternatively, we can copy songs, remove, and rebuild.
    // Better: add Playlist::removeSong(Song*).
    // We'll add that method.
    // For now, we'll implement it in Playlist.
    pl->removeSong(song);
    M3uLoader::savePlaylist("../Data/Playlists", *pl);
}