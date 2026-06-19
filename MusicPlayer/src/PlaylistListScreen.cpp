#include "PlaylistListScreen.hpp"
#include "Terminal.hpp"
#include <iostream>

PlaylistListScreen::PlaylistListScreen(Application& app) : app_(app) {}

void PlaylistListScreen::onEnter() {
    Terminal::init();
    selectedIndex_ = 0;
}

void PlaylistListScreen::render() {
    Terminal::clearScreen();
    Terminal::printAt(0, 0, "============== ☰ Playlists ☰ ==============", Terminal::CP_DEFAULT, true);

    auto& playlists = app_.getPlaylists();
    int row = 2;
    if (playlists.empty()) {
        Terminal::printAt(row++, 0, "No playlists found.");
    } else {
        for (size_t i = 0; i < playlists.size(); ++i) {
            std::string label = std::to_string(i + 1) + ". " + playlists[i].getName() +
                " (" + std::to_string(playlists[i].size()) + " songs)";
            if (app_.getActivePlaylist() == &playlists[i]) label += "  <-- active";
            Terminal::printHighlighted(row + (int)i, 0, label, (int)i == selectedIndex_);
        }
        row += (int)playlists.size();
    }
    row += 1;
    Terminal::printAt(row++, 0,
        "UP/DOWN + ENTER to switch playlist, or a number key to jump straight to it.",
        Terminal::CP_CYAN);
    Terminal::printAt(row++, 0, "[n] New playlist   [d] Delete selected");
    Terminal::printAt(row, 0, "0. Back");
    Terminal::refresh();
}

void PlaylistListScreen::handleInput() {
    auto& playlists = app_.getPlaylists();
    int count = (int)playlists.size();
    int key = Terminal::getKey();

    if (key == '0') {
        app_.goBack();
        return;
    }

    if (key == 'n' || key == 'N') {
        std::string name = Terminal::readLine(Terminal::height() - 2, 0, "New playlist name: ");
        if (!name.empty()) {
            app_.createPlaylist(name);
        }
        return;
    }

    if ((key == 'd' || key == 'D') && count > 0) {
        // Confirm deletion? We'll do it directly.
        Playlist* toDelete = &playlists[selectedIndex_];
        app_.deletePlaylist(toDelete);
        // After deletion, selectedIndex_ may become out of range; reset
        if (selectedIndex_ >= (int)app_.getPlaylists().size() && !app_.getPlaylists().empty())
            selectedIndex_ = (int)app_.getPlaylists().size() - 1;
        return;
    }

    if (count == 0) return;

    if (key == Terminal::KEY_UP_) {
        selectedIndex_ = (selectedIndex_ - 1 + count) % count;
        return;
    }
    if (key == Terminal::KEY_DOWN_) {
        selectedIndex_ = (selectedIndex_ + 1) % count;
        return;
    }
    if (key == Terminal::KEY_ENTER_ || key == '\r') {
        Playlist* selected = &playlists[selectedIndex_];
        app_.setActivePlaylist(selected);
        app_.goBack();
        return;
    }
    if (key >= '1' && key <= '9') {
        int choice = key - '0';
        if (choice >= 1 && choice <= count) {
            Playlist* selected = &playlists[choice - 1];
            app_.setActivePlaylist(selected);
            app_.goBack();
        }
        return;
    }
}