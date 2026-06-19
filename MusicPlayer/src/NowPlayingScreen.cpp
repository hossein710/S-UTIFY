#include "NowPlayingScreen.hpp"
#include "Terminal.hpp"
#include <iomanip>
#include <sstream>

NowPlayingScreen::NowPlayingScreen(Application& app) : app_(app) {}
NowPlayingScreen::~NowPlayingScreen() {}

void NowPlayingScreen::onEnter() { Terminal::init(); }
void NowPlayingScreen::onExit() {}

namespace {
std::string formatTime(float seconds) {
    int s = (int)seconds;
    int m = s / 60;
    s %= 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << m << ":"
        << std::setw(2) << std::setfill('0') << s;
    return oss.str();
}
}

void NowPlayingScreen::render() {
    Terminal::clearScreen();
    Terminal::printAt(0, 0, "===== ♪ Now Playing ♪ =====", Terminal::CP_DEFAULT, true);

    Player& player = app_.getPlayer();
    Song* song = player.getCurrentSong();
    int row = 2;

    if (!song) {
        Terminal::printAt(row, 0, "No song playing.");
        row += 1;
    } else {
        Terminal::printAt(row++, 0, "Title:  " + song->getTitle());
        Terminal::printAt(row++, 0, "Artist: " + song->getArtist());
        Terminal::printAt(row++, 0, "Album:  " + song->getAlbum());
        // Show favorite status
        std::string fav = song->isFavorite() ? "♥ Favourite" : "♡";
        Terminal::printAt(row++, 0, fav, Terminal::CP_RED);

        float cur = player.getCurrentTimeSec();
        float total = player.getTotalTimeSec();

        std::string state;
        int stateColor = Terminal::CP_DEFAULT;
        switch (player.getState()) {
            case Player::PLAYING: state = "Playing"; stateColor = Terminal::CP_GREEN; break;
            case Player::PAUSED:  state = "Paused";  stateColor = Terminal::CP_YELLOW; break;
            case Player::STOPPED: state = "Stopped"; stateColor = Terminal::CP_RED; break;
        }
        Terminal::printAt(row++, 0, "State:  " + state, stateColor, true);
        row++;

        // progress bar
        int barWidth = Terminal::width() - 14;
        if (barWidth < 10) barWidth = 10;
        float ratio = (total > 0.0f) ? (cur / total) : 0.0f;
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        int filled = (int)(ratio * barWidth);

        std::string bar = "[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) bar += "=";
            else if (i == filled) bar += ">";
            else bar += " ";
        }
        bar += "]";

        Terminal::printAt(row, 0, formatTime(cur));
        Terminal::printAt(row, 6, bar, Terminal::CP_GREEN);
        Terminal::printAt(row, 6 + barWidth + 2, formatTime(total));
        row += 2;
    }

    row += 1;
    Terminal::printAt(row++, 0, "Controls:", Terminal::CP_CYAN, true);
    Terminal::printAt(row++, 0, "[p] play/resume    [s] stop    [SPACE] pause");
    Terminal::printAt(row++, 0, "[n] next           [b] prev");
    Terminal::printAt(row++, 0, "[<-] -10s          [->] +10s");
    Terminal::printAt(row++, 0, "[f] toggle favorite ♥", Terminal::CP_RED);

    row++;
    Terminal::printAt(row++, 0, "[0] Back");
    Terminal::refresh();
}

void NowPlayingScreen::handleInput() {
    int ch = Terminal::getKeyTimeout(1000);
    if (ch == -1) return;
    handleKey(ch);
}

void NowPlayingScreen::handleKey(int key) {
    Player& player = app_.getPlayer();
    if (key == 'p' || key == 'P') {
        player.play();
    } else if (key == 's' || key == 'S') {
        player.stop();
    } else if (key == ' ') {
        if (player.getState() == Player::PLAYING)
            player.pause();
        else if (player.getState() == Player::PAUSED)
            player.resume();
        else
            player.play();
    } else if (key == 'n' || key == 'N') {
        player.next();
    } else if (key == 'b' || key == 'B') {
        player.previous();
    } else if (key == Terminal::KEY_LEFT_) {
        player.seekBy(-10);
    } else if (key == Terminal::KEY_RIGHT_) {
        player.seekBy(10);
    } else if (key == 'f' || key == 'F') {
        Song* s = player.getCurrentSong();
        if (s) app_.toggleFavorite(s);
    } else if (key == '0') {
        app_.goBack();
    }
}