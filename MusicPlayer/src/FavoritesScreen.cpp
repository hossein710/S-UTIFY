#include "FavoritesScreen.hpp"
#include "Terminal.hpp"
#include <iomanip>
#include <sstream>

FavoritesScreen::FavoritesScreen(Application& app) : app_(app) {}

void FavoritesScreen::onEnter() {
    Terminal::init();
    refreshList();
}

void FavoritesScreen::refreshList() {
    favoriteSongs_.clear();
    for (const auto& ptr : app_.getLibrary().getAllSongs()) {
        if (ptr->isFavorite()) favoriteSongs_.push_back(ptr.get());
    }
    if (selectedIndex_ >= (int)favoriteSongs_.size()) selectedIndex_ = 0;
}

void FavoritesScreen::render() {
    Terminal::clearScreen();
    Terminal::printAt(0, 0, "============= ♥♥♥ Favorites ♥♥♥ ============", Terminal::CP_RED, true);

    int row = 2;
    if (favoriteSongs_.empty()) {
        Terminal::printAt(row++, 0, "No favorite songs yet.");
    } else {
        Terminal::printAt(row++, 0, "  #  Title", Terminal::CP_CYAN, true);
        Terminal::printAt(row++, 0, std::string(50, '-'), Terminal::CP_CYAN);
        for (size_t i = 0; i < favoriteSongs_.size(); ++i) {
            Song* s = favoriteSongs_[i];
            std::string line = "  " + std::to_string(i+1) + ". " + s->getTitle() + " - " + s->getArtist();
            Terminal::printHighlighted(row + (int)i, 0, line, (int)i == selectedIndex_);
        }
        row += (int)favoriteSongs_.size();
    }

    row += 1;
    Terminal::printAt(row++, 0, "UP/DOWN + ENTER to play, [f] toggle favorite, [0] back", Terminal::CP_CYAN);
    Terminal::refresh();
}

void FavoritesScreen::handleInput() {
    int key = Terminal::getKey();
    if (key == '0') {
        app_.goBack();
        return;
    }
    if (favoriteSongs_.empty()) return;

    if (key == Terminal::KEY_UP_) {
        selectedIndex_ = (selectedIndex_ - 1 + (int)favoriteSongs_.size()) % (int)favoriteSongs_.size();
        return;
    }
    if (key == Terminal::KEY_DOWN_) {
        selectedIndex_ = (selectedIndex_ + 1) % (int)favoriteSongs_.size();
        return;
    }
    if (key == Terminal::KEY_ENTER_ || key == '\r') {
        app_.getPlayer().playSong(favoriteSongs_[selectedIndex_]);
        return;
    }
    if (key == 'f' || key == 'F') {
        Song* s = favoriteSongs_[selectedIndex_];
        app_.toggleFavorite(s);
        refreshList();
        return;
    }
    if (key >= '1' && key <= '9') {
        int num = key - '0';
        if (num >= 1 && num <= (int)favoriteSongs_.size()) {
            selectedIndex_ = num - 1;
            app_.getPlayer().playSong(favoriteSongs_[selectedIndex_]);
        }
        return;
    }
}