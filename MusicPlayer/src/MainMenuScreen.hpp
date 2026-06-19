#pragma once
#include "Screen.hpp"
#include "Application.hpp"

class MainMenuScreen : public Screen {
public:
    explicit MainMenuScreen(Application& app);
    void render() override;
    void handleInput() override;
    void onEnter() override;

private:
    Application& app_;
    int selectedIndex_ = 0;

    void executeChoice(int choice);
};
