#include "SettingsScreen.hpp"
#include "Terminal.hpp"
#include <vector>

namespace {
const std::vector<std::string> kModes = {"NO_REPEAT", "REPEAT_ONE", "REPEAT_ALL", "SHUFFLE"};
}

SettingsScreen::SettingsScreen(Application& app) : app_(app) {}

void SettingsScreen::onEnter() {
    Terminal::init();
    Player& player = app_.getPlayer();
    std::string cur = player.getPlaybackModeStr();
    for (size_t i = 0; i < kModes.size(); ++i) {
        if (kModes[i] == cur) { selectedIndex_ = (int)i; break; }
    }
}

void SettingsScreen::render() {
    Terminal::clearScreen();
    Terminal::printAt(0, 0, "========= ⚙ Settings ⚙ =========", Terminal::CP_YELLOW, true);

    Player& player = app_.getPlayer();
    std::string currentMode = player.getPlaybackModeStr();
    Terminal::printAt(2, 0, "Playback Mode (current: " + currentMode + ")", Terminal::CP_CYAN);

    int row = 4;
    for (size_t i = 0; i < kModes.size(); ++i) {
        std::string label = std::to_string(i + 1) + ". " + kModes[i];
        Terminal::printHighlighted(row + (int)i, 0, label, (int)i == selectedIndex_);
    }
    row += (int)kModes.size() + 1;
    Terminal::printAt(row++, 0, "0. Back (changes saved automatically)");
    row += 1;
    Terminal::printAt(row, 0, "UP/DOWN + ENTER, or a number key, to set the mode directly.", Terminal::CP_CYAN);
    Terminal::refresh();
}

void SettingsScreen::handleInput() {
    Player& player = app_.getPlayer();
    int count = (int)kModes.size();
    int key = Terminal::getKey();

    if (key == '0') {
        app_.goBack();
        return;
    }
    if (key == Terminal::KEY_UP_) {
        selectedIndex_ = (selectedIndex_ - 1 + count) % count;
        return;
    }
    if (key == Terminal::KEY_DOWN_) {
        selectedIndex_ = (selectedIndex_ + 1) % count;
        return;
    }

    int choice = -1;
    if (key == Terminal::KEY_ENTER_ || key == '\r') {
        choice = selectedIndex_ + 1;
    } else if (key >= '1' && key <= '4') {
        choice = key - '0';
    } else {
        return;
    }

    switch (choice) {
        case 1: player.setPlaybackMode(Player::NO_REPEAT);  selectedIndex_ = 0; break;
        case 2: player.setPlaybackMode(Player::REPEAT_ONE); selectedIndex_ = 1; break;
        case 3: player.setPlaybackMode(Player::REPEAT_ALL); selectedIndex_ = 2; break;
        case 4: player.setPlaybackMode(Player::SHUFFLE);    selectedIndex_ = 3; break;
        default: return;
    }
    app_.saveConfig();
}
