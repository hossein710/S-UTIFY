#pragma once

class Screen {
public:
    virtual ~Screen() = default;
    virtual void render() = 0;
    virtual void handleInput() = 0;
    // Called when screen becomes active
    virtual void onEnter() {}
    // Called when screen is about to be left
    virtual void onExit() {}
};