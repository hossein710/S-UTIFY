#include "Song.hpp"

Song::Song(std::string title, std::string artist, std::string album,
           std::string genre, int year, int durationSec, std::string filePath)
    : title_(std::move(title)), artist_(std::move(artist)),
      album_(std::move(album)), genre_(std::move(genre)),
      year_(year), durationSec_(durationSec), filePath_(std::move(filePath)) {}

const std::string& Song::getTitle() const { return title_; }
const std::string& Song::getArtist() const { return artist_; }
const std::string& Song::getAlbum() const { return album_; }
const std::string& Song::getGenre() const { return genre_; }
int Song::getYear() const { return year_; }
int Song::getDurationSec() const { return durationSec_; }
const std::string& Song::getFilePath() const { return filePath_; }

void Song::setTitle(const std::string& t) { title_ = t; }
void Song::setArtist(const std::string& a) { artist_ = a; }
void Song::setAlbum(const std::string& a) { album_ = a; }
void Song::setGenre(const std::string& g) { genre_ = g; }
void Song::setYear(int y) { year_ = y; }
void Song::setDurationSec(int d) { durationSec_ = d; }
void Song::setFilePath(const std::string& f) { filePath_ = f; }

bool Song::isFavorite() const { return favorite_; }
void Song::setFavorite(bool fav) { favorite_ = fav; }
int Song::getPlayCount() const { return playCount_; }
void Song::setPlayCount(int count) { playCount_ = count; }
void Song::incrementPlayCount() { ++playCount_; }