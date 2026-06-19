#include "TopSongsScreen.hpp"
#include "Terminal.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

TopSongsScreen::TopSongsScreen(Application& app) : app_(app) {}

void TopSongsScreen::onEnter() {
    Terminal::init();
    refreshList();
}

void TopSongsScreen::refreshList() {
    topSongs_.clear();
    for (const auto& ptr : app_.getLibrary().getAllSongs()) {
        if (ptr->getPlayCount() > 0) topSongs_.push_back(ptr.get());
    }
    std::sort(topSongs_.begin(), topSongs_.end(),
              [](Song* a, Song* b) { return a->getPlayCount() > b->getPlayCount(); });
    if (selectedIndex_ >= (int)topSongs_.size()) selectedIndex_ = 0;
}

void TopSongsScreen::render() {
    Terminal::clearScreen();
    Terminal::printAt(0, 0, "======== ♬♬♬ Top Songs (by play count) ♬♬♬ =========", Terminal::CP_DEFAULT, true);

    int row = 2;
    if (topSongs_.empty()) {
        Terminal::printAt(row++, 0, "No songs have been played yet.");
    } else {
        Terminal::printAt(row++, 0, "    #               Title               Artist    Plays", Terminal::CP_CYAN, true);
        Terminal::printAt(row++, 0, std::string(60, '-'), Terminal::CP_CYAN);
        for (size_t i = 0; i < topSongs_.size() && i < 20; ++i) {
            Song* s = topSongs_[i];
            std::ostringstream line;
            line << "  " << std::setw(2) << (i+1) << ". "
                 << std::setw(24) << (s->getTitle().size() > 24 ? s->getTitle().substr(0,24) : s->getTitle())
                 << "  " << std::setw(18) << (s->getArtist().size() > 18 ? s->getArtist().substr(0,18) : s->getArtist())
                 << "  " << s->getPlayCount();
            Terminal::printHighlighted(row + (int)i, 0, line.str(), (int)i == selectedIndex_);
        }
        row += (int)std::min(topSongs_.size(), (size_t)20);
    }

    row += 1;
    Terminal::printAt(row++, 0, "UP/DOWN + ENTER to play, [0] back", Terminal::CP_CYAN);
    Terminal::refresh();
}

void TopSongsScreen::handleInput() {
    int key = Terminal::getKey();
    if (key == '0') {
        app_.goBack();
        return;
    }
    if (topSongs_.empty()) return;

    if (key == Terminal::KEY_UP_) {
        selectedIndex_ = (selectedIndex_ - 1 + (int)topSongs_.size()) % (int)topSongs_.size();
        return;
    }
    if (key == Terminal::KEY_DOWN_) {
        selectedIndex_ = (selectedIndex_ + 1) % (int)topSongs_.size();
        return;
    }
    if (key == Terminal::KEY_ENTER_ || key == '\r') {
        app_.getPlayer().playSong(topSongs_[selectedIndex_]);
        return;
    }
    if (key >= '1' && key <= '9') {
        int num = key - '0';
        if (num >= 1 && num <= (int)topSongs_.size()) {
            selectedIndex_ = num - 1;
            app_.getPlayer().playSong(topSongs_[selectedIndex_]);
        }
        return;
    }
}