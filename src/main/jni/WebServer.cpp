#include "WebServer.h"
#include "GlobalState.h"
#include <thread>
#include <memory>
#include <atomic>
#include "include/httplib.h"
#include <android/log.h>
#include "include/nlohmann/json.hpp"
#include "PanelHTML.h" 
#include "ConfigManager.h"

// Global server instance and thread
std::unique_ptr<httplib::Server> svr;
std::thread server_thread;
std::atomic<bool> is_running(false);

// Fungsi untuk menghentikan server
void StopWebServer() {
    if (svr && is_running) {
        __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "Stopping web server...");
        svr->stop();
        is_running = false;
        if (server_thread.joinable()) {
            server_thread.join();
        }
        __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "Web server stopped.");
        g_IsWebServerReady = false;
    }
}

// Implementasi StateToJson
std::string StateToJson() {
    __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "StateToJson: Acquiring lock...");
    std::lock_guard<std::mutex> lock(g_State.stateMutex);
    __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "StateToJson: Lock acquired. Serializing state.");
    
    nlohmann::json j;
    j["battleState"] = g_State.battleState;
    j["roomInfoEnabled"] = g_State.roomInfoEnabled;
    j["webServerEnabled"] = g_State.webServerEnabled;

    for (const auto& p : g_State.players) {
        j["players"].push_back({
            {"name", p.name},
            {"uid", p.uid},
            {"rank", p.rank},
            {"spell", p.spell},
            {"hero", p.heroName},
            {"camp", p.camp},
            {"heroId", p.heroId},
            {"spellId", p.spellId},
            {"rankLevel", p.rankLevel}
        });
    }
    
    __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "StateToJson: Serialization complete. Releasing lock.");
    return j.dump(4);
}


// Fungsi untuk menjalankan server di thread terpisah
void RunServerLoop() {
    // Endpoint untuk mendapatkan state game saat ini
    svr->Get("/state", [](const httplib::Request &, httplib::Response &res) {
        __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "Received /state request");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(StateToJson(), "application/json");
        __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "/state request handled");
    });

    // Endpoint untuk mengkonfigurasi mod
    svr->Post("/config", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        nlohmann::json j = nlohmann::json::parse(req.body, nullptr, false);
        if (j.is_discarded()) {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON\"}", "application/json");
            return;
        }

        bool stateChanged = false;
        if (j.contains("roomInfoEnabled") && j["roomInfoEnabled"].is_boolean()) {
            __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "Received /config request for room info");
            std::lock_guard<std::mutex> lock(g_State.stateMutex);
            g_State.roomInfoEnabled = j["roomInfoEnabled"].get<bool>();
            stateChanged = true;
        }

        if (stateChanged) {
            SaveConfig(g_State);
            res.set_content("{\"status\": \"success\"}", "application/json");
        } else {
            res.status = 400;
            res.set_content("{\"status\": \"error\", \"message\": \"Invalid parameter\"}", "application/json");
        }
    });

    // Handle OPTIONS request for CORS preflight on /config
    svr->Options("/config", [](const httplib::Request &, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.status = 204;
    });

    // Endpoint untuk menyajikan panel HTML dari string yang disematkan
    svr->Get("/panel", [](const httplib::Request &, httplib::Response &res) {
        res.set_content(panel_html_content, "text/html");
    });
    
    is_running = true;
    __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "Starting web server on 0.0.0.0:2626...");
    g_IsWebServerReady = svr->listen("0.0.0.0", 2626);
    is_running = false; // listen has returned, so it's not running anymore

    if (g_IsWebServerReady) {
        __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "Web server started successfully on port 2626.");
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "MLBS_WEB_SERVER", "Failed to start web server on port 2626.");
    }
}

// Fungsi utama yang dipanggil untuk memulai server
void StartWebServer() {
    if (is_running) {
        __android_log_print(ANDROID_LOG_INFO, "MLBS_WEB_SERVER", "Web server is already running.");
        return;
    }
    
    // Hentikan server lama jika ada
    StopWebServer(); 

    // Buat instance server baru
    svr = std::make_unique<httplib::Server>();

    // Jalankan di thread baru
    server_thread = std::thread(RunServerLoop);
}