#pragma once
#include "Screen.hpp"
#include "Application.hpp"
#include <vector>
#include <string>

class TopSongsScreen : public Screen {
public:
    explicit TopSongsScreen(Application& app);
    void render() override;
    void handleInput() override;
    void onEnter() override;

private:
    Application& app_;
    std::vector<Song*> topSongs_;
    int selectedIndex_ = 0;

    void refreshList();
};