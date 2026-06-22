SUTIFY: main.o Application.o ConfigManager.o CsvLoader.o M3uLoader.o MainMenuScreen.o MusicLibrary.o NowPlayingScreen.o Player.o Playlist.o PlaylistListScreen.o PlaylistViewScreen.o SettingsScreen.o Song.o Terminal.o TopSongsScreen.o FavoritesScreen.o -lncurses
	g++ -o SUTIFY main.o Application.o ConfigManager.o CsvLoader.o M3uLoader.o MainMenuScreen.o MusicLibrary.o NowPlayingScreen.o Player.o Playlist.o PlaylistListScreen.o PlaylistViewScreen.o SettingsScreen.o Song.o Terminal.o TopSongsScreen.o FavoritesScreen.o -lncurses

main.o: main.cpp Application.hpp
	g++ -c main.cpp

Application.o: Application.cpp Application.hpp MainMenuScreen.hpp NowPlayingScreen.hpp CsvLoader.hpp M3uLoader.hpp Terminal.hpp
	g++ -c Application.cpp

ConfigManager.o: ConfigManager.cpp ConfigManager.hpp
	g++ -c ConfigManager.cpp

CsvLoader.o: CsvLoader.cpp CsvLoader.hpp
	g++ -c CsvLoader.cpp

M3uLoader.o: M3uLoader.cpp M3uLoader.hpp
	g++ -c M3uLoader.cpp

MainMenuScreen.o: MainMenuScreen.cpp MainMenuScreen.hpp NowPlayingScreen.hpp PlaylistListScreen.hpp PlaylistViewScreen.hpp SettingsScreen.hpp TopSongsScreen.hpp FavoritesScreen.hpp Terminal.hpp
	g++ -c MainMenuScreen.cpp

MusicLibrary.o: MusicLibrary.cpp MusicLibrary.hpp
	g++ -c MusicLibrary.cpp

NowPlayingScreen.o: NowPlayingScreen.cpp NowPlayingScreen.hpp Terminal.hpp
	g++ -c NowPlayingScreen.cpp

Player.o: Player.cpp Player.hpp miniaudio.h
	g++ -c Player.cpp

Playlist.o: Playlist.cpp Playlist.hpp
	g++ -c Playlist.cpp

PlaylistListScreen.o: PlaylistListScreen.cpp PlaylistListScreen.hpp Terminal.hpp PlaylistViewScreen.hpp
	g++ -c PlaylistListScreen.cpp

PlaylistViewScreen.o: PlaylistViewScreen.cpp PlaylistViewScreen.hpp Terminal.hpp
	g++ -c PlaylistViewScreen.cpp

SettingsScreen.o: SettingsScreen.cpp SettingsScreen.hpp Terminal.hpp
	g++ -c SettingsScreen.cpp

FavoritesScreen.o: FavoritesScreen.cpp FavoritesScreen.hpp Terminal.hpp
	g++ -c FavoritesScreen.cpp

TopSongsScreen.o: TopSongsScreen.cpp TopSongsScreen.hpp Terminal.hpp
	g++ -c TopSongsScreen.cpp

Song.o: Song.cpp Song.hpp
	g++ -c Song.cpp

Terminal.o: Terminal.cpp Terminal.hpp
	g++ -c Terminal.cpp
clean:
	rm *.o SUTIFY