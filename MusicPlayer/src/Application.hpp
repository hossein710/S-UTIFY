#pragma once
#include "MusicLibrary.hpp"
#include "Playlist.hpp"
#include "Player.hpp"
#include "ConfigManager.hpp"
#include "Screen.hpp"
#include <vector>
#include <memory>
#include <functional>

class Application {
public:
    Application();
    ~Application();
    void run();

    // Screen switching
    void switchScreen(std::unique_ptr<Screen> screen);
    void goBack();  // returns to main menu

    // Accessors for screens
    MusicLibrary& getLibrary();
    std::vector<Playlist>& getPlaylists();
    Playlist* getActivePlaylist();
    void setActivePlaylist(Playlist* pl);
    Player& getPlayer();
    Config& getConfig();
    void saveConfig();

    // New user data & playlist management
    void toggleFavorite(Song* song);
    void incrementPlayCount(Song* song);
    void createPlaylist(const std::string& name);
    void deletePlaylist(Playlist* pl);
    void addSongToPlaylist(Playlist* pl, Song* song);
    void removeSongFromPlaylist(Playlist* pl, Song* song);

private:
    MusicLibrary library_;
    std::vector<Playlist> playlists_;
    Playlist* activePlaylist_ = nullptr;
    Player player_;
    Config config_;
    std::unique_ptr<Screen> currentScreen_;
    bool running_ = true;

    void loadData();
    void loadUserData();
    void saveUserData();
    std::string getUserDataFilePath() const;
};