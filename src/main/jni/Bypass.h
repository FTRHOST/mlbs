#pragma once

#include "GlobalState.h"
#include "dobby.h"
#include "feature/GameClass.h"

// Original function pointer
void (*origSendRawData)(void*, uint, MonoArray<uint8_t>*, int, int, int, bool, int);

// Hook function for SendRawData
void mySendRawData(void* thiz, uint uiMsgID, MonoArray<uint8_t>* rawData, int rawDataSize, int eSocketType, int packReliType, bool bNeedLockRequestIndex, int expectedSize) {
    // Daftar ID pesan yang akan diblokir (diambil dari Unlock.h asli)
    if (g_State.bypassEnabled) {
        if (uiMsgID == 1015  || uiMsgID == 1016 || uiMsgID == 1019 || uiMsgID == 1020
            || uiMsgID == 1031  || uiMsgID == 1032  || uiMsgID == 1035 || uiMsgID == 1036
            || uiMsgID == 1160  || uiMsgID == 1161  || uiMsgID == 1162  || uiMsgID == 1163
            || uiMsgID == 1208  || uiMsgID == 10017  || uiMsgID == 10018 || uiMsgID == 10192
            || uiMsgID == 10193 || uiMsgID == 10603  || uiMsgID == 10604 || uiMsgID == 19457
            || uiMsgID == 19458  || uiMsgID == 19459 || uiMsgID == 19460 || uiMsgID == 19156
            || uiMsgID == 19157  || uiMsgID == 19150  || uiMsgID == 19151 || uiMsgID == 19131
            || uiMsgID == 19132 || uiMsgID == 1150  || uiMsgID == 1151  || uiMsgID == 1152
            || uiMsgID == 1153  || uiMsgID == 1154 || uiMsgID == 1155 || uiMsgID == 10103
            || uiMsgID == 10104  || uiMsgID == 19461  || uiMsgID == 19462 || uiMsgID == 11502
            || uiMsgID == 11503  || uiMsgID == 11149 || uiMsgID == 11150 || uiMsgID == 40047
            || uiMsgID == 40048  || uiMsgID == 31859  || uiMsgID == 31860  || uiMsgID == 50087
            || uiMsgID == 50088) {
            return; // Blokir pengiriman data
        }
    }
    // Jika tidak diblokir, panggil fungsi aslinya
    return origSendRawData(thiz, uiMsgID, rawData, rawDataSize, eSocketType, packReliType, bNeedLockRequestIndex, expectedSize);
}

// Fungsi untuk menerapkan atau menghapus hook berdasarkan state
void ApplyBypass() {
    static bool isBypassHooked = false;
    
    // Jika bypass diaktifkan dan hook belum terpasang
    if (g_State.bypassEnabled && !isBypassHooked) {
        void* sendRawDataAddr = (void*)GameServerConfig_SendRawData;
        if (sendRawDataAddr) {
            DobbyHook(sendRawDataAddr, (void*)mySendRawData, (void**)&origSendRawData);
            isBypassHooked = true;
        }
    } 
    // Jika bypass dinonaktifkan dan hook sudah terpasang, hapus hook
    else if (!g_State.bypassEnabled && isBypassHooked) {
        void* sendRawDataAddr = (void*)GameServerConfig_SendRawData;
        if (sendRawDataAddr) {
            DobbyDestroy(sendRawDataAddr);
            isBypassHooked = false;
        }
    }
}
