#pragma once
#include "Screen.hpp"
#include "Application.hpp"

class SettingsScreen : public Screen {
public:
    explicit SettingsScreen(Application& app);
    void render() override;
    void handleInput() override;
    void onEnter() override;

private:
    Application& app_;
    int selectedIndex_ = 0;
};
