#pragma once

#include "miniaudio.h"
#include "Song.hpp"
#include "Playlist.hpp"
#include "MusicLibrary.hpp"
#include <string>
#include <vector>
#include <functional>

class Player {
public:
    enum State { STOPPED, PLAYING, PAUSED };
    enum PlaybackMode { NO_REPEAT, REPEAT_ONE, REPEAT_ALL, SHUFFLE };

    explicit Player(MusicLibrary& library);
    ~Player();

    void play();
    void pause();
    void resume();
    void stop();
    void next();
    void previous();
    void seekBy(int seconds);
    void playSong(Song* song);
    void tick();

    void setPlaylist(Playlist* playlist);
    void setPlaybackMode(PlaybackMode mode);
    PlaybackMode getPlaybackMode() const;
    std::string getPlaybackModeStr() const;

    State getState() const;
    Song* getCurrentSong() const;
    int getCurrentIndex() const;
    float getCurrentTimeSec() const;
    float getTotalTimeSec() const;

    // New: resume a specific song at given position (sets state to PAUSED)
    bool setCurrentSong(Song* song, float position = 0.0f);

    // Callback for play count increment
    void setOnPlayCountIncrement(std::function<void(Song*)> cb) { onPlayCountIncrement = cb; }

private:
    void loadSong(int index);
    void unloadSound();
    void advance();
    void advanceWithLimit(int attempts);
    int getNextIndex() const;
    void updateShuffleOrder();

    MusicLibrary& library_;

    ma_engine* engine_  = nullptr;
    ma_sound*  sound_   = nullptr;
    bool soundLoaded_   = false;

    Playlist*    playlist_    = nullptr;
    int          currentIndex_ = -1;
    State        state_       = STOPPED;
    PlaybackMode mode_        = NO_REPEAT;
    bool needAdvance_ = false;

    std::vector<int> shuffleOrder_;
    int              shufflePos_ = 0;

    // New for play count tracking
    bool countedForThisPlay_ = false;
    std::function<void(Song*)> onPlayCountIncrement;
};