#include "PlaylistViewScreen.hpp"
#include "Terminal.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <unistd.h>

PlaylistViewScreen::PlaylistViewScreen(Application& app) : app_(app) {}

void PlaylistViewScreen::onEnter() {
    Terminal::init();
    selectedIndex_ = 0;
    rebuildDisplay();
}

void PlaylistViewScreen::rebuildDisplay() {
    Playlist* pl = app_.getActivePlaylist();
    if (!pl) {
        displayedSongs_.clear();
        return;
    }
    displayedSongs_ = pl->getSongs();
    applySearch();
    applyFilter();
    applySort();

    if (displayedSongs_.empty()) {
        selectedIndex_ = 0;
    } else if (selectedIndex_ >= (int)displayedSongs_.size()) {
        selectedIndex_ = (int)displayedSongs_.size() - 1;
    }
}

void PlaylistViewScreen::applySearch() {
    if (searchQuery_.empty()) return;
    std::string query = searchQuery_;
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);
    displayedSongs_.erase(
        std::remove_if(displayedSongs_.begin(), displayedSongs_.end(),
            [&](Song* s) {
                std::string title = s->getTitle();
                std::string artist = s->getArtist();
                std::transform(title.begin(), title.end(), title.begin(), ::tolower);
                std::transform(artist.begin(), artist.end(), artist.begin(), ::tolower);
                return title.find(query) == std::string::npos &&
                       artist.find(query) == std::string::npos;
            }),
        displayedSongs_.end()
    );
}

void PlaylistViewScreen::applyFilter() {
    if (filterValue_.empty()) return;
    displayedSongs_.erase(
        std::remove_if(displayedSongs_.begin(), displayedSongs_.end(),
            [&](Song* s) {
                if (filterType_ == "artist")
                    return s->getArtist() != filterValue_;
                else if (filterType_ == "album")
                    return s->getAlbum() != filterValue_;
                return false;
            }),
        displayedSongs_.end()
    );
}

void PlaylistViewScreen::applySort() {
    if (sortKey_.empty()) return;
    std::sort(displayedSongs_.begin(), displayedSongs_.end(),
        [&](Song* a, Song* b) {
            bool less;
            if (sortKey_ == "title") less = a->getTitle() < b->getTitle();
            else if (sortKey_ == "artist") less = a->getArtist() < b->getArtist();
            else if (sortKey_ == "album") less = a->getAlbum() < b->getAlbum();
            else if (sortKey_ == "year") less = a->getYear() < b->getYear();
            else if (sortKey_ == "duration") less = a->getDurationSec() < b->getDurationSec();
            else return false;
            return sortAscending_ ? less : !less;
        }
    );
}

namespace {
std::string durationStr(int totalSec) {
    int mins = totalSec / 60, secs = totalSec % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << mins << ":"
        << std::setw(2) << std::setfill('0') << secs;
    return oss.str();
}
std::string truncate(const std::string& s, size_t n) {
    return s.size() > n ? s.substr(0, n) : s;
}
std::string padRight(const std::string& s, size_t n) {
    std::string r = s;
    if (r.size() < n) r += std::string(n - r.size(), ' ');
    return r;
}
}

void PlaylistViewScreen::render() {
    Terminal::clearScreen();
    std::string playlistName = app_.getActivePlaylist() ? app_.getActivePlaylist()->getName() : "";
    Terminal::printAt(0, 0, "===== Playlist View: " + playlistName + " =====", Terminal::CP_MAGENTA, true);

    int row = 2;
    if (displayedSongs_.empty()) {
        Terminal::printAt(row++, 0, "No songs to display.");
    } else {
        std::string header = "  " + padRight("#", 4) + padRight("Title", 30) +
                              padRight("Artist", 20) + padRight("Album", 20) + "Duration";
        Terminal::printAt(row++, 0, header, Terminal::CP_DEFAULT, true);
        Terminal::printAt(row++, 0, std::string(86, '-'), Terminal::CP_CYAN);

        for (size_t i = 0; i < displayedSongs_.size(); ++i) {
            Song* s = displayedSongs_[i];
            std::string line = "  " + padRight(std::to_string(i + 1), 4) +
                padRight(truncate(s->getTitle(), 29), 30) +
                padRight(truncate(s->getArtist(), 19), 20) +
                padRight(truncate(s->getAlbum(), 19), 20) +
                durationStr(s->getDurationSec());
            // add a star if favorite
            if (s->isFavorite()) line += " ★";
            Terminal::printHighlighted(row + (int)i, 0, line, (int)i == selectedIndex_);
        }
        row += (int)displayedSongs_.size();
        Terminal::printAt(row++, 0, std::string(86, '-'), Terminal::CP_CYAN);
    }

    row += 1;
    Terminal::printAt(row++, 0, "UP/DOWN + ENTER, or [1-9], to play a song.", Terminal::CP_CYAN);
    Terminal::printAt(row++, 0, "[s] sort   [f] filter   [/] search   [r] reset");
    Terminal::printAt(row++, 0, "[a] add this to playlist   [A] add song to this playlist   [x] remove from playlist");
    Terminal::printAt(row,   0, "[c] clear search           [0] Back");
    row += 2;
    if (!searchQuery_.empty())
        Terminal::printAt(row++, 0, "Search: " + searchQuery_, Terminal::CP_YELLOW);
    if (!filterValue_.empty())
        Terminal::printAt(row++, 0, "Filter: " + filterType_ + " = " + filterValue_, Terminal::CP_YELLOW);
    if (!sortKey_.empty())
        Terminal::printAt(row++, 0, "Sort: " + sortKey_ + (sortAscending_ ? " (asc)" : " (desc)"), Terminal::CP_YELLOW);

    Terminal::refresh();
}

int PlaylistViewScreen::selectFromList(const std::string& title, const std::vector<std::string>& options) {
    if (options.empty()) return -1;
    int idx = 0;
    while (true) {
        Terminal::clearScreen();
        Terminal::printAt(0, 0, title, Terminal::CP_DEFAULT, true);
        for (size_t i = 0; i < options.size(); ++i) {
            std::string label = std::to_string(i + 1) + ". " + options[i];
            Terminal::printHighlighted(2 + (int)i, 0, label, (int)i == idx);
        }
        Terminal::printAt(3 + (int)options.size(), 0,
            "UP/DOWN + ENTER, a number key, or 0/ESC to cancel.", Terminal::CP_CYAN);
        Terminal::refresh();

        int key = Terminal::getKey();
        if (key == '0' || key == Terminal::KEY_ESC_) return -1;
        if (key == Terminal::KEY_UP_) {
            idx = (idx - 1 + (int)options.size()) % (int)options.size();
            continue;
        }
        if (key == Terminal::KEY_DOWN_) {
            idx = (idx + 1) % (int)options.size();
            continue;
        }
        if (key == Terminal::KEY_ENTER_ || key == '\r') return idx;
        if (key >= '1' && key <= '9') {
            int n = key - '0';
            if (n >= 1 && n <= (int)options.size()) return n - 1;
        }
    }
}

void PlaylistViewScreen::handleInput() {
    int key = Terminal::getKey();

    if (key == '0') {
        app_.goBack();
        return;
    }

    if (key == Terminal::KEY_UP_) {
        if (!displayedSongs_.empty())
            selectedIndex_ = (selectedIndex_ - 1 + (int)displayedSongs_.size()) % (int)displayedSongs_.size();
        return;
    }
    if (key == Terminal::KEY_DOWN_) {
        if (!displayedSongs_.empty())
            selectedIndex_ = (selectedIndex_ + 1) % (int)displayedSongs_.size();
        return;
    }
    if (key == Terminal::KEY_ENTER_ || key == '\r') {
        if (!displayedSongs_.empty())
            app_.getPlayer().playSong(displayedSongs_[selectedIndex_]);
        return;
    }
    if (key >= '1' && key <= '9') {
        int num = key - '0';
        if (num >= 1 && num <= (int)displayedSongs_.size()) {
            selectedIndex_ = num - 1;
            app_.getPlayer().playSong(displayedSongs_[selectedIndex_]);
        }
        return;
    }

    if (key == 's' || key == 'S') {
        int sel = selectFromList("Sort by:", {"Title", "Artist", "Album", "Year", "Duration"});
        if (sel == -1) return;
        static const std::vector<std::string> keys = {"title", "artist", "album", "year", "duration"};
        sortKey_ = keys[sel];

        int order = selectFromList("Order:", {"Ascending", "Descending"});
        if (order == -1) return;
        sortAscending_ = (order == 0);
        rebuildDisplay();
    } else if (key == 'f' || key == 'F') {
        int typeSel = selectFromList("Filter by:", {"Artist", "Album"});
        if (typeSel == -1) return;
        filterType_ = (typeSel == 0) ? "artist" : "album";

        Playlist* pl = app_.getActivePlaylist();
        if (!pl) return;
        std::vector<std::string> values;
        for (Song* s : pl->getSongs()) {
            std::string val = (filterType_ == "artist") ? s->getArtist() : s->getAlbum();
            if (std::find(values.begin(), values.end(), val) == values.end())
                values.push_back(val);
        }
        int valSel = selectFromList("Available " + filterType_ + "s:", values);
        if (valSel == -1) return;
        filterValue_ = values[valSel];
        rebuildDisplay();
    } else if (key == 'r' || key == 'R') {
        searchQuery_.clear();
        filterValue_.clear();
        filterType_.clear();
        sortKey_.clear();
        rebuildDisplay();
    } else if (key == 'c' || key == 'C') {
        searchQuery_.clear();
        rebuildDisplay();
    } else if (key == '/') {
        std::string q = Terminal::readLine(Terminal::height() - 2, 0, "Search: ");
        if (!q.empty()) {
            searchQuery_ = q;
            rebuildDisplay();
        }
    }
    else if (key == 'a') {
        if (displayedSongs_.empty()) return;
        Song* song = displayedSongs_[selectedIndex_];
        // Build list of playlists (excluding current)
        std::vector<std::string> names;
        std::vector<Playlist*> playlists;
        Playlist* current = app_.getActivePlaylist();
        for (auto& pl : app_.getPlaylists()) {
            if (&pl == current) continue;
            names.push_back(pl.getName());
            playlists.push_back(&pl);
        }
        if (names.empty()) {
            // no other playlists
            Terminal::printAt(Terminal::height()-1, 0, "No other playlists to add to.", Terminal::CP_RED);
            Terminal::refresh();
            usleep(1000000);
            return;
        }
        int sel = selectFromList("Add to playlist:", names);
        if (sel == -1) return;
        Playlist* target = playlists[sel];
        app_.addSongToPlaylist(target, song);
        // Maybe show a message
        Terminal::printAt(Terminal::height()-1, 0, "Added to " + target->getName(), Terminal::CP_GREEN);
        Terminal::refresh();
        usleep(800000);
    }

    else if (key == 'A'){
        auto& songs = app_.getLibrary().getAllSongs();
        std::vector<std::string> names;
        Playlist* current = app_.getActivePlaylist();
        displayedSongs_.clear();
        for(const auto& song_ptr : songs){
            displayedSongs_.push_back(song_ptr.get());
            names.push_back(displayedSongs_.back()->getTitle());
        }
        int sel = selectFromList("Add Song to this playlist:", names);
        if(sel == -1) return;
        app_.addSongToPlaylist(current, displayedSongs_[sel]);

        Terminal::printAt(Terminal::height()-1, 0, names[sel] + " Added to " + current->getName(), Terminal::CP_GREEN);
        Terminal::refresh();
        usleep(800000);
    }
    // New: remove from current playlist
    else if (key == 'x' || key == 'X') {
        if (displayedSongs_.empty()) return;
        Song* song = displayedSongs_[selectedIndex_];
        Playlist* current = app_.getActivePlaylist();
        if (!current) return;
        // Confirm? We'll just remove.
        app_.removeSongFromPlaylist(current, song);
        rebuildDisplay();
    }
}