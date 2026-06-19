#include "Terminal.hpp"
#include <ncurses.h>

const int Terminal::KEY_ENTER_ = '\n';
const int Terminal::KEY_ESC_   = 27;
const int Terminal::KEY_UP_    = KEY_UP;
const int Terminal::KEY_DOWN_  = KEY_DOWN;
const int Terminal::KEY_LEFT_  = KEY_LEFT;
const int Terminal::KEY_RIGHT_ = KEY_RIGHT;

namespace {
bool g_initialized = false;
}

void Terminal::init() {
    if (g_initialized) return;
    initscr();
    cbreak();              // no line buffering, but signals (^C) still work
    noecho();
    keypad(stdscr, true);  // decode arrow/function keys
    curs_set(0);           // hide the cursor by default

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(CP_DEFAULT,   COLOR_WHITE,   -1);
        init_pair(CP_RED,       COLOR_RED,     -1);
        init_pair(CP_GREEN,     COLOR_GREEN,   -1);
        init_pair(CP_YELLOW,    COLOR_YELLOW,  -1);
        init_pair(CP_BLUE,      COLOR_BLUE,    -1);
        init_pair(CP_MAGENTA,   COLOR_MAGENTA, -1);
        init_pair(CP_CYAN,      COLOR_CYAN,    -1);
        init_pair(CP_WHITE,     COLOR_WHITE,   -1);
        init_pair(CP_HIGHLIGHT, COLOR_WHITE, COLOR_BLUE);
        init_pair(CP_PROGRESS,  COLOR_BLACK,   COLOR_GREEN);
    }
    g_initialized = true;
}

void Terminal::shutdown() {
    if (!g_initialized) return;
    endwin();
    g_initialized = false;
}

void Terminal::clearScreen() { erase(); }
void Terminal::refresh() { ::refresh(); }
int Terminal::width() { return COLS; }
int Terminal::height() { return LINES; }

int Terminal::getKey() {
    timeout(-1); // block indefinitely
    return getch();
}

int Terminal::getKeyTimeout(int ms) {
    timeout(ms);
    int ch = getch(); // returns ERR (-1) on timeout
    timeout(-1);
    return ch;
}

void Terminal::printAt(int y, int x, const std::string& text, int colorPair, bool bold) {
    int attrs = 0;
    if (colorPair > 0) attrs |= COLOR_PAIR(colorPair);
    if (bold) attrs |= A_BOLD;
    if (attrs) attron(attrs);
    mvprintw(y, x, "%s", text.c_str());
    if (attrs) attroff(attrs);
}

void Terminal::printHighlighted(int y, int x, const std::string& text, bool active) {
    if (active) attron(COLOR_PAIR(CP_HIGHLIGHT) | A_BOLD);
    mvprintw(y, x, "%s", text.c_str());
    if (active) attroff(COLOR_PAIR(CP_HIGHLIGHT) | A_BOLD);
}

std::string Terminal::readLine(int y, int x, const std::string& prompt) {
    mvprintw(y, x, "%s", prompt.c_str());
    echo();
    curs_set(1);
    char buf[256] = {0};
    mvgetnstr(y, x + (int)prompt.size(), buf, sizeof(buf) - 1);
    noecho();
    curs_set(0);
    return std::string(buf);
}
