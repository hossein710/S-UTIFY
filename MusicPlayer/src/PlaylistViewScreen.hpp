#pragma once
#include "Screen.hpp"
#include "Application.hpp"
#include <vector>
#include <string>

class PlaylistViewScreen : public Screen {
public:
    explicit PlaylistViewScreen(Application& app);
    void render() override;
    void handleInput() override;
    void onEnter() override;

private:
    Application& app_;
    std::vector<Song*> displayedSongs_;  // current view (filtered/sorted)
    std::string searchQuery_;
    std::string sortKey_;  // "title", "artist", "album", "year", "duration"
    bool sortAscending_ = true;
    std::string filterType_; // "artist" or "album"
    std::string filterValue_;
    int selectedIndex_ = 0;  // currently highlighted row

    void rebuildDisplay();
    void applySort();
    void applyFilter();
    void applySearch();

    // Renders `options` as a highlighted, arrow-navigable list under `title`
    // and returns the chosen index, or -1 if the user cancelled (0/ESC).
    int selectFromList(const std::string& title, const std::vector<std::string>& options);
};
