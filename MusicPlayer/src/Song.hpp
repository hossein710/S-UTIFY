#pragma once
#include <string>

class Song {
public:
    Song() = default;
    Song(std::string title, std::string artist, std::string album,
         std::string genre, int year, int durationSec, std::string filePath);

    // Getters
    const std::string& getTitle() const;
    const std::string& getArtist() const;
    const std::string& getAlbum() const;
    const std::string& getGenre() const;
    int getYear() const;
    int getDurationSec() const;
    const std::string& getFilePath() const;

    // Setters
    void setTitle(const std::string& t);
    void setArtist(const std::string& a);
    void setAlbum(const std::string& a);
    void setGenre(const std::string& g);
    void setYear(int y);
    void setDurationSec(int d);
    void setFilePath(const std::string& f);


    
    bool isFavorite() const;
    void setFavorite(bool fav);
    int getPlayCount() const;
    void setPlayCount(int count);
    void incrementPlayCount();

private:
    std::string title_;
    std::string artist_;
    std::string album_;
    std::string genre_;
    int year_ = 0;
    int durationSec_ = 0;
    std::string filePath_;
    bool favorite_ = false;
    int playCount_ = 0;
};