#include "Player.hpp"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

Player::Player(MusicLibrary& library) : library_(library) {
    engine_ = new ma_engine;
    if (ma_engine_init(nullptr, engine_) != MA_SUCCESS) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
        delete engine_;
        engine_ = nullptr;
    }
    sound_ = new ma_sound;
}

Player::~Player() {
    unloadSound();
    if (sound_) {
        delete sound_;
        sound_ = nullptr;
    }
    if (engine_) {
        ma_engine_uninit(engine_);
        delete engine_;
        engine_ = nullptr;
    }
}

void Player::unloadSound() {
    if (soundLoaded_) {
        ma_sound_uninit(sound_);
        soundLoaded_ = false;
    }
}

void Player::setPlaylist(Playlist* playlist) {
    if (playlist != playlist_) {
        stop();
        playlist_ = playlist;
        currentIndex_ = -1;
        shuffleOrder_.clear();
        shufflePos_ = 0;
        countedForThisPlay_ = false;
        if (playlist_ && playlist_->size() > 0) {
            currentIndex_ = 0;
            if (mode_ == SHUFFLE) updateShuffleOrder();
        }
    }
}

void Player::play() {
    if (!playlist_ || playlist_->size() == 0) {
        std::cerr << "Playlist is empty." << std::endl;
        return;
    }
    if (state_ == PLAYING) return;
    if (currentIndex_ < 0 || currentIndex_ >= (int)playlist_->size()) {
        currentIndex_ = 0;
    }
    if (state_ == PAUSED) {
        if (soundLoaded_) {
            ma_sound_start(sound_);
            state_ = PLAYING;
        } else {
            loadSong(currentIndex_);
            if (soundLoaded_) {
                ma_sound_start(sound_);
                state_ = PLAYING;
            }
        }
        return;
    }
    loadSong(currentIndex_);
    if (soundLoaded_) {
        ma_sound_start(sound_);
        state_ = PLAYING;
    }
}

void Player::pause() {
    if (state_ == PLAYING && soundLoaded_) {
        ma_sound_stop(sound_);
        state_ = PAUSED;
    }
}

void Player::resume() {
    if (state_ == PAUSED && soundLoaded_) {
        ma_sound_start(sound_);
        state_ = PLAYING;
    } else if (state_ == STOPPED) {
        play();
    }
}

void Player::stop() {
    if (soundLoaded_) {
        ma_sound_stop(sound_);
        ma_sound_seek_to_pcm_frame(sound_, 0);
    }
    state_ = STOPPED;
}

void Player::loadSong(int index) {
    if (!playlist_ || index < 0 || index >= (int)playlist_->size()) {
        unloadSound();
        currentIndex_ = -1;
        countedForThisPlay_ = false;
        return;
    }
    Song* song = playlist_->getSong(index);
    if (!song) {
        unloadSound();
        currentIndex_ = -1;
        countedForThisPlay_ = false;
        return;
    }
    unloadSound();
    std::string path = "../Data/" + song->getFilePath();
    if (ma_sound_init_from_file(engine_, path.c_str(), 0, nullptr, nullptr, sound_) != MA_SUCCESS) {
        std::cerr << "Failed to load sound: " << path << std::endl;
        soundLoaded_ = false;
        currentIndex_ = index;
        countedForThisPlay_ = false;
        return;
    }
    soundLoaded_ = true;
    currentIndex_ = index;
    countedForThisPlay_ = false;  // reset for new song
}

void Player::next() {
    if (!playlist_ || playlist_->size() == 0) return;
    int nextIdx = getNextIndex();
    if (nextIdx < 0) {
        if (mode_ != REPEAT_ALL) {
            stop();
        }
        return;
    }
    if (mode_ == SHUFFLE) {
        int pos = shufflePos_ + 1;
        if (pos >= (int)shuffleOrder_.size()) pos = 0;
        shufflePos_ = pos;
    }
    loadSong(nextIdx);
    if (soundLoaded_ && state_ != STOPPED) {
        ma_sound_start(sound_);
        state_ = PLAYING;
    }
}

void Player::previous() {
    if (!playlist_ || playlist_->size() == 0) return;

    if (mode_ == SHUFFLE) {
        if (!shuffleOrder_.empty()) {
            int prevPos = shufflePos_ - 1;
            if (prevPos < 0) prevPos = (int)shuffleOrder_.size() - 1;
            shufflePos_ = prevPos;
            int prev = shuffleOrder_[shufflePos_];
            loadSong(prev);
            if (soundLoaded_ && state_ != STOPPED) {
                ma_sound_start(sound_);
                state_ = PLAYING;
            }
        }
        return;
    }

    if (currentIndex_ <= 0) {
        if (mode_ == REPEAT_ALL || mode_ == REPEAT_ONE) {
            int last = (int)playlist_->size() - 1;
            loadSong(last);
            if (soundLoaded_ && state_ != STOPPED) {
                ma_sound_start(sound_);
                state_ = PLAYING;
            }
        }
        return;
    }

    int prev = currentIndex_ - 1;
    loadSong(prev);
    if (soundLoaded_ && state_ != STOPPED) {
        ma_sound_start(sound_);
        state_ = PLAYING;
    }
}

void Player::playSong(Song* song) {
    if (!playlist_ || !song) return;
    for (size_t i = 0; i < playlist_->size(); ++i) {
        if (playlist_->getSong(i) == song) {
            loadSong(i);
            if (soundLoaded_) {
                ma_sound_start(sound_);
                state_ = PLAYING;
            } else {
                advanceWithLimit(0);
            }
            return;
        }
    }
    std::cerr << "Song not found in current playlist." << std::endl;
}

void Player::advance() {
    advanceWithLimit(0);
}

void Player::advanceWithLimit(int attempts) {
    if (!playlist_ || playlist_->size() == 0) return;
    if (attempts >= (int)playlist_->size()) {
        std::cerr << "No playable songs found in playlist." << std::endl;
        stop();
        return;
    }

    int nextIdx = getNextIndex();
    if (nextIdx < 0) {
        stop();
        return;
    }

    if (mode_ == SHUFFLE) {
        int pos = shufflePos_ + 1;
        if (pos >= (int)shuffleOrder_.size()) pos = 0;
        shufflePos_ = pos;
    }

    loadSong(nextIdx);
    if (soundLoaded_) {
        if (state_ == PLAYING || state_ == PAUSED) {
            ma_sound_start(sound_);
            state_ = PLAYING;
        }
    } else {
        advanceWithLimit(attempts + 1);
    }
}

int Player::getNextIndex() const {
    if (!playlist_ || playlist_->size() == 0) return -1;
    if (mode_ == REPEAT_ONE) return currentIndex_;
    if (mode_ == SHUFFLE) {
        if (shuffleOrder_.empty()) return -1;
        int pos = shufflePos_ + 1;
        if (pos >= (int)shuffleOrder_.size()) {
            if (mode_ == REPEAT_ALL) pos = 0;
            else return -1;
        }
        return shuffleOrder_[pos];
    }
    int next = currentIndex_ + 1;
    if (next >= (int)playlist_->size()) {
        if (mode_ == REPEAT_ALL) next = 0;
        else return -1;
    }
    return next;
}

void Player::updateShuffleOrder() {
    if (!playlist_) return;
    shuffleOrder_.resize(playlist_->size());
    for (size_t i = 0; i < playlist_->size(); ++i) shuffleOrder_[i] = (int)i;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(shuffleOrder_.begin(), shuffleOrder_.end(), std::default_random_engine(seed));
    shufflePos_ = 0;
    if (currentIndex_ >= 0) {
        auto it = std::find(shuffleOrder_.begin(), shuffleOrder_.end(), currentIndex_);
        if (it != shuffleOrder_.end()) {
            shufflePos_ = std::distance(shuffleOrder_.begin(), it);
        }
    }
}

void Player::tick() {
    if (state_ != PLAYING) return;
    if (!soundLoaded_) return;

    // Play count: if played at least 10 seconds and not yet counted
    if (!countedForThisPlay_ && getCurrentTimeSec() >= 10.0f) {
        if (onPlayCountIncrement && getCurrentSong()) {
            onPlayCountIncrement(getCurrentSong());
        }
        countedForThisPlay_ = true;
    }

    if (ma_sound_at_end(sound_)) {
        if (mode_ == REPEAT_ONE) {
            ma_sound_seek_to_pcm_frame(sound_, 0);
            ma_sound_start(sound_);
        } else {
            advance();
        }
    }
}

void Player::seekBy(int seconds) {
    if (!soundLoaded_) return;
    ma_uint64 cursor, length;
    ma_sound_get_cursor_in_pcm_frames(sound_, &cursor);
    ma_sound_get_length_in_pcm_frames(sound_, &length);
    ma_uint32 sampleRate = ma_engine_get_sample_rate(engine_);
    ma_int64 newFrame = (ma_int64)cursor + (ma_int64)seconds * sampleRate;
    if (newFrame < 0) newFrame = 0;
    if ((ma_uint64)newFrame >= length) {
        next();
        return;
    }
    ma_sound_seek_to_pcm_frame(sound_, (ma_uint64)newFrame);
}

bool Player::setCurrentSong(Song* song, float position) {
    if (!playlist_) return false;
    int idx = -1;
    for (size_t i = 0; i < playlist_->size(); ++i) {
        if (playlist_->getSong(i) == song) { idx = (int)i; break; }
    }
    if (idx < 0) return false;
    loadSong(idx);
    if (!soundLoaded_) return false;
    // Seek to position
    ma_uint64 totalFrames;
    ma_sound_get_length_in_pcm_frames(sound_, &totalFrames);
    ma_uint32 sampleRate = ma_engine_get_sample_rate(engine_);
    ma_uint64 targetFrame = (ma_uint64)(position * sampleRate);
    if (targetFrame >= totalFrames) targetFrame = totalFrames - 1;
    ma_sound_seek_to_pcm_frame(sound_, targetFrame);
    // Set state to PAUSED so user can resume
    state_ = PAUSED;
    return true;
}

Player::State Player::getState() const { return state_; }
Player::PlaybackMode Player::getPlaybackMode() const { return mode_; }
std::string Player::getPlaybackModeStr() const {
    switch (mode_) {
        case NO_REPEAT: return "NO_REPEAT";
        case REPEAT_ONE: return "REPEAT_ONE";
        case REPEAT_ALL: return "REPEAT_ALL";
        case SHUFFLE: return "SHUFFLE";
    }
    return "NO_REPEAT";
}
void Player::setPlaybackMode(PlaybackMode mode) {
    mode_ = mode;
    if (mode_ == SHUFFLE) {
        updateShuffleOrder();
    }
}
Song* Player::getCurrentSong() const {
    if (!playlist_ || currentIndex_ < 0 || currentIndex_ >= (int)playlist_->size())
        return nullptr;
    return playlist_->getSong(currentIndex_);
}
int Player::getCurrentIndex() const { return currentIndex_; }
float Player::getCurrentTimeSec() const {
    if (!soundLoaded_) return 0.0f;
    ma_uint64 frames = 0;
    ma_sound_get_cursor_in_pcm_frames(sound_, &frames);
    ma_uint32 rate = ma_engine_get_sample_rate(engine_);
    return (float)frames / (float)rate;
}
float Player::getTotalTimeSec() const {
    if (!soundLoaded_) return 0.0f;
    ma_uint64 frames = 0;
    ma_sound_get_length_in_pcm_frames(sound_, &frames);
    ma_uint32 rate = ma_engine_get_sample_rate(engine_);
    return (float)frames / (float)rate;
}