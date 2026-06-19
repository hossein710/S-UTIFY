#pragma once
#include "Screen.hpp"
#include "Application.hpp"

class PlaylistListScreen : public Screen {
public:
    explicit PlaylistListScreen(Application& app);
    void render() override;
    void handleInput() override;
    void onEnter() override;

private:
    Application& app_;
    int selectedIndex_ = 0;
};
