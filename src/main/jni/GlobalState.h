#pragma once

#include <string>
#include <vector>
#include <mutex>

// Info untuk satu pemain
struct PlayerData {
    std::string name;
    std::string uid;
    std::string rank;
    std::string spell;
    std::string heroName;
    int camp;
    // Raw data fields
    int heroId;
    int spellId;
    int rankLevel;
};

// State global aplikasi
struct GlobalState {
    std::mutex stateMutex;
    int battleState = 0; // 0: Lobby, 2: Draft, 3: In-Game
    bool roomInfoEnabled = true;
    bool webServerEnabled = true;
    bool showMenu = true;
    std::vector<PlayerData> players;
};

// Deklarasi instance global
extern GlobalState g_State;
extern bool g_IsWebServerReady;

// Fungsi untuk mengonversi state ke JSON
std::string StateToJson();
