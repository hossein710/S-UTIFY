#pragma once

#include "Screen.hpp"
#include "Application.hpp"

class NowPlayingScreen : public Screen {
public:
    explicit NowPlayingScreen(Application& app);
    ~NowPlayingScreen();
    void render() override;
    void handleInput() override;
    void onEnter() override;
    void onExit() override;

private:
    Application& app_;
    void handleKey(int key);
};
