#include "MainMenuScreen.hpp"
#include "NowPlayingScreen.hpp"
#include "PlaylistListScreen.hpp"
#include "PlaylistViewScreen.hpp"
#include "SettingsScreen.hpp"
#include "FavoritesScreen.hpp"
#include "TopSongsScreen.hpp"
#include "Terminal.hpp"
#include <memory>
#include <vector>
#include <cstdlib>

namespace {
const std::vector<std::string> kMenuItems = {
    "Now Playing",
    "Playlist List",
    "Playlist View",
    "Favorites",
    "Top Songs",
    "Settings",
    "Exit"
};
}

MainMenuScreen::MainMenuScreen(Application& app) : app_(app) {}

void MainMenuScreen::onEnter() {
    Terminal::init();
}

void MainMenuScreen::render() {
    Terminal::clearScreen();
    Terminal::printAt(0, 0, "=============== ♬ Terminal Music Player ♬ ===============", Terminal::CP_DEFAULT, true);

    int row = 2;
    const Config& cfg = app_.getConfig();
    if (!cfg.lastSong.empty()) {
        Terminal::printAt(row, 0, "Last played: " + cfg.lastSong, Terminal::CP_YELLOW);
        row += 2;
    } else {
        row += 1;
    }

    for (size_t i = 0; i < kMenuItems.size(); ++i) {
        bool isExit = (i == kMenuItems.size() - 1);
        std::string label = (isExit ? "0. " : std::to_string(i + 1) + ". ") + kMenuItems[i];
        Terminal::printHighlighted(row + (int)i, 0, label, (int)i == selectedIndex_);
    }

    Terminal::printAt(row + (int)kMenuItems.size() + 1, 0,
        "UP/DOWN + ENTER to select, or press a number key directly.", Terminal::CP_CYAN);
    Terminal::refresh();
}

void MainMenuScreen::handleInput() {
    int key = Terminal::getKey();
    int count = (int)kMenuItems.size();

    if (key == Terminal::KEY_UP_) {
        selectedIndex_ = (selectedIndex_ - 1 + count) % count;
        return;
    }
    if (key == Terminal::KEY_DOWN_) {
        selectedIndex_ = (selectedIndex_ + 1) % count;
        return;
    }
    if (key == Terminal::KEY_ENTER_ || key == '\r') {
        bool isExit = (selectedIndex_ == count - 1);
        executeChoice(isExit ? 0 : selectedIndex_ + 1);
        return;
    }
    if (key >= '0' && key <= '9') {
        executeChoice(key - '0');
        return;
    }
}

void MainMenuScreen::executeChoice(int choice) {
    switch (choice) {
        case 1:
            app_.switchScreen(std::make_unique<NowPlayingScreen>(app_));
            break;
        case 2:
            app_.switchScreen(std::make_unique<PlaylistListScreen>(app_));
            break;
        case 3:
            app_.switchScreen(std::make_unique<PlaylistViewScreen>(app_));
            break;
        case 4:
            app_.switchScreen(std::make_unique<FavoritesScreen>(app_));
            break;
        case 5:
            app_.switchScreen(std::make_unique<TopSongsScreen>(app_));
            break;
        case 6:
            app_.switchScreen(std::make_unique<SettingsScreen>(app_));
            break;
        case 0:
            app_.saveConfig();
            Terminal::shutdown();
            exit(0);
            break;
        default:
            break;
    }
}