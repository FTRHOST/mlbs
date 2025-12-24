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

// Info untuk satu event draft
struct DraftEvent {
    std::string playerName;
    std::string heroName;
    std::string eventType; // "PICK" or "BAN"
};

// State global aplikasi
struct GlobalState {
    int battleState = -1;
    std::vector<PlayerData> players;
    std::vector<DraftEvent> draftEvents;
    bool bypassEnabled = true;
    bool roomInfoEnabled = true;

    // Mutex untuk melindungi akses data dari thread yang berbeda
    mutable std::mutex stateMutex;
};

// Deklarasi instance global
extern GlobalState g_State;
extern bool g_IsWebServerReady;

// Fungsi untuk mengonversi state ke JSON
std::string StateToJson();
