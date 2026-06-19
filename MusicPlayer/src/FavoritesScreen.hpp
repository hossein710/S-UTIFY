#pragma once
#include "Screen.hpp"
#include "Application.hpp"
#include <vector>
#include <string>

class FavoritesScreen : public Screen {
public:
    explicit FavoritesScreen(Application& app);
    void render() override;
    void handleInput() override;
    void onEnter() override;

private:
    Application& app_;
    std::vector<Song*> favoriteSongs_;
    int selectedIndex_ = 0;

    void refreshList();
};