#pragma once
#include <string>

// Thin wrapper around ncurses giving the rest of the app a simple,
// screen-agnostic API: colored/highlighted printing, single-keypress
// input (blocking or with a timeout so a screen can auto-refresh),
// and free-text line entry for the few places that genuinely need it
// (search queries, etc).
class Terminal {
public:
    // Idempotent: safe to call from every screen's onEnter().
    static void init();
    // Tears down ncurses. Call once, right before the process exits.
    static void shutdown();

    static void clearScreen();
    static void refresh();
    static int width();
    static int height();

    // Blocking key read. Arrow keys / function keys come back as the
    // ncurses KEY_* constants (see KEY_UP_ etc below) thanks to keypad mode.
    static int getKey();

    // Like getKey(), but returns -1 (no key) after `ms` milliseconds.
    // Used by NowPlayingScreen so it can re-render (and tick the
    // progress bar) once a second even with nobody pressing anything.
    static int getKeyTimeout(int ms);

    static void printAt(int y, int x, const std::string& text,
                         int colorPair = CP_DEFAULT, bool bold = false);

    // Draws `text` at (y,x); if `active` is true it's drawn in reverse
    // video so it reads as "the currently selected row".
    static void printHighlighted(int y, int x, const std::string& text, bool active);

    // Temporarily turns on echo + cursor for a single line of free-text
    // input (e.g. a search query) and returns what the user typed.
    static std::string readLine(int y, int x, const std::string& prompt);

    enum {
        CP_DEFAULT = 1,
        CP_RED,
        CP_GREEN,
        CP_YELLOW,
        CP_BLUE,
        CP_MAGENTA,
        CP_CYAN,
        CP_WHITE,
        CP_HIGHLIGHT,
        CP_PROGRESS
    };

    static const int KEY_ENTER_;
    static const int KEY_ESC_;
    static const int KEY_UP_;
    static const int KEY_DOWN_;
    static const int KEY_LEFT_;
    static const int KEY_RIGHT_;
};
