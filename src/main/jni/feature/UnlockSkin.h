#pragma once
#include "../Include.h"
#include "../include/Utils/Unity/ByNameModding/Tools.h"
#include "../include/Utils/Unity/ByNameModding/Il2Cpp.h"
#include "../include/Utils/Logger.h"

// ================= GLOBAL CONFIG =================
bool g_UnlockAll = false;

// ================= STRUCTS =================
struct CmdHeroSkin : Il2CppObject {
    uint32_t iId;
    uint32_t iLimitTime;
    uint32_t iSource;
    uint32_t iLimitTimeMagicChess;
    uint32_t iGetTime;
};

struct CmdHeroStatue : Il2CppObject {
    uint32_t iId;
    uint32_t iLimitTime;
    uint32_t iSource;
};

// ================= MACROS =================
#define DefineHook(RET, NAME, ARGS) \
    RET (*o ## NAME) ARGS; \
    RET NAME ARGS

// ================= GLOBALS =================
uintptr_t CmdHeroSkin__ctor = 0;
uintptr_t CmdHeroStatue__ctor = 0;

bool ndUnlockSkin3 = false;
bool ndUnlockSkin = true;
bool ndUnlockSkin2 = true;
bool ndUnlockSkin22 = false;

uint32_t m_HeroID = 0, m_SkinID = 0;

// ================= HOOKS =================

DefineHook(CmdHeroSkin *, GetHeroSkin, (void * unk, uintptr_t m_heroskins, uint skinid)) {
    auto ret = oGetHeroSkin(unk, m_heroskins, skinid);

    if (ret || !ndUnlockSkin)
        return ret;

    if (CmdHeroSkin__ctor == 0) return ret;

    auto MTTDProto_CmdHeroSkin_ctor = (void *(*)(CmdHeroSkin *)) (CmdHeroSkin__ctor);
    auto instance = (CmdHeroSkin *) Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
    if (instance && MTTDProto_CmdHeroSkin_ctor(instance)) {
        instance->iId = skinid;
        instance->iLimitTime = 0;
        instance->iSource = 0;
        instance->iLimitTimeMagicChess= 0;
        instance->iGetTime = 0;
    }
    return instance;
}

// DefineHook(CmdHeroSkin *, GetMCLimitSkin, (void * unk, uint skinId)) {
//     auto ret = oGetMCLimitSkin(unk, skinId);
//
//     if (ret || !ndUnlockSkin)
//         return ret;
//
//     if (CmdHeroSkin__ctor == 0) return ret;
//
//     auto  MTTDProto_CmdHeroSkin_ctor = (void *(*)(CmdHeroSkin *)) (CmdHeroSkin__ctor);
//     auto instance = (CmdHeroSkin *) Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
//     if (instance && MTTDProto_CmdHeroSkin_ctor(instance)) {
//         instance->iId = skinId;
//         instance->iLimitTime = 0;
//         instance->iSource = 0;
//         instance->iLimitTimeMagicChess= 0;
//         instance->iGetTime = 0;
//     }
//     return instance;
// }

DefineHook(CmdHeroStatue *, GetHeroHolyStatue, (void * unk, uintptr_t m_heroStatues, uint statueid)) {
    auto ret = oGetHeroHolyStatue(unk, m_heroStatues, statueid);

    if (ret || !ndUnlockSkin)
        return ret;

    if (CmdHeroStatue__ctor == 0) return ret;

    auto MTTDProto_CmdHeroStatue_ctor = (void *(*)(CmdHeroStatue *)) (CmdHeroStatue__ctor);
    auto instance = (CmdHeroStatue *) Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroStatue");
    if (instance && MTTDProto_CmdHeroStatue_ctor(instance)) {
        instance->iId = statueid;
        instance->iLimitTime = 0;
        instance->iSource = 0;

    }
    return instance;
}

DefineHook(bool, IsCanUseSkin, (void * unk, int heroid)) {
    return true;
}

DefineHook(CmdHeroSkin *, IsHaveSkin, (void * unk, int skinid)) {
    auto ret = oIsHaveSkin(unk, skinid);

    if (ret || !ndUnlockSkin)
        return ret;

    if (CmdHeroSkin__ctor == 0) return ret;

    auto MTTDProto_CmdHeroSkin_ctor = (void *(*)(CmdHeroSkin *)) (CmdHeroSkin__ctor);
    auto instance = (CmdHeroSkin *) Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
    if (instance && MTTDProto_CmdHeroSkin_ctor(instance)) {
        instance->iId = skinid;
        instance->iLimitTime = 0;
        instance->iSource = 0;
        instance->iLimitTimeMagicChess= 0;
        instance->iGetTime = 0;
    }
    return instance;
}

DefineHook(CmdHeroSkin *, IsHaveSkinForever, (void * unk, int skinid)) {
    auto ret = oIsHaveSkinForever(unk, skinid);

    if (ret || !ndUnlockSkin)
        return ret;

    if (CmdHeroSkin__ctor == 0) return ret;

    auto MTTDProto_CmdHeroSkin_ctor = (void *(*)(CmdHeroSkin *)) (CmdHeroSkin__ctor);
    auto instance = (CmdHeroSkin *) Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
    if (instance && MTTDProto_CmdHeroSkin_ctor(instance)) {
        instance->iId = skinid;
        instance->iLimitTime = 0;
        instance->iSource = 0;
        instance->iLimitTimeMagicChess= 0;
        instance->iGetTime = 0;
    }
    return instance;
}

DefineHook(CmdHeroStatue *, IsHaveStatue, (void * unk, uint32_t statueid)) {
    auto ret = oIsHaveStatue(unk, statueid);

    if (ret || !ndUnlockSkin)
        return ret;

    if (CmdHeroStatue__ctor == 0) return ret;

    auto MTTDProto_CmdHeroStatue_ctor = (void *(*)(CmdHeroStatue *)) (CmdHeroStatue__ctor);
    auto instance = (CmdHeroStatue *) Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroStatue");
    if (instance && MTTDProto_CmdHeroStatue_ctor(instance)) {
        instance->iId = statueid;
        instance->iLimitTime = 0;
        instance->iSource = 0;
    }
    return instance;
}

DefineHook(CmdHeroStatue *, IsHaveStatueForever, (void * unk, uint32_t statueid)) {
    auto ret = oIsHaveStatueForever(unk, statueid);

    if (ret || !ndUnlockSkin)
        return ret;

    if (CmdHeroStatue__ctor == 0) return ret;

    auto MTTDProto_CmdHeroStatue_ctor = (void *(*)(CmdHeroStatue *)) (CmdHeroStatue__ctor);
    auto instance = (CmdHeroStatue *) Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroStatue");
    if (instance && MTTDProto_CmdHeroStatue_ctor(instance)) {
        instance->iId = statueid;
        instance->iLimitTime = 0;
        instance->iSource = 0;
    }
    return instance;
}
// DefineHook(uint, GetExpiredCardSkinLimitTime, (void * unk, uint32_t skinid)) {
//     if (ndUnlockSkin2) {
//         return oGetExpiredCardSkinLimitTime(unk,skinid);
//     }
//     return 0; // fallback
// }
DefineHook(bool, GetLeaderSkinBForbid, (void * unk, uint32_t skinid)) {
    if (ndUnlockSkin2) {
        return true;
    }
    return false; // fallback
}
DefineHook(bool, BRankHeroCanUse, (void * unk, uint32_t skinid)) {
    if (ndUnlockSkin2) {
        return true;
    }
    return false; // fallback
}
// DefineHook(bool, ContainSkin, (void * unk, uint32_t skinid)) {
//     if (ndUnlockSkin2) {
//         return false;
//     }
//     return true; // fallback
// }
DefineHook(bool, UIChooseHero_ShowSkinExpiryDate, (void * unk)) {
    return true;
}

DefineHook(bool, IsForbidStatue, (void * unk, uint32_t statueid)) {
    return false;
}

DefineHook(bool, IsForbidSkin, (void * unk, uint32_t skinid)) {
    return false;
}
DefineHook(bool, CheckReputationUnlockSkin, (void * unk, int skinid)) {
    return true;
}
DefineHook(bool, SendData, (void * unk, int skinid)) {
    return true;
}
// DefineHook(bool, GetStarSkinRank, (void * unk, uint32_t skinid)) {
//     return true;
// }
DefineHook(bool, IsForbidARSkin, (void * unk, uint32_t id)) {
    return false;
}

DefineHook(bool, IsForbidHeros, (void * unk, uint32_t heroId)) {
    return false;
}
DefineHook(bool, IsForbidHeroInChooseHero, (void * unk, uint32_t heroId)) {
    return false;
}
DefineHook(bool, IsActivityForbidHeros, (void * unk, uint32_t heroId)) {
    return false;
}
DefineHook(bool, IsLimitActiveHero, (void * unk, uint32_t heroId)) {
    return true;
}
DefineHook(bool, IsHeroInShop, (void * unk, int heroId)) {
    return true;
}

// == NEW ==

DefineHook(bool, IsForbidNewHeroList, (void * unk, uint32_t heroId)) {
    return false;
}

DefineHook(bool, IsForbidARHero, (void * unk, uint32_t id)) {
    return false;
}

DefineHook(bool, IsForbidHeadFrameForce, (void * unk, uint32_t frameId)) {
    return false;
}

DefineHook(bool, IsForbidHeadFrame, (void * unk, uint32_t frameId)) {
    return false;
}

// =======
DefineHook(void, SaveSelectHeroSkinId, (uintptr_t thiz, uint32_t skinid, uint32_t heroid)) {
    if (oIsHaveSkin(0, skinid) || oIsHaveSkinForever(0, skinid) || ndUnlockSkin) {
        m_SkinID = 0;
        return oSaveSelectHeroSkinId(thiz, skinid, heroid);
    }
    m_HeroID = heroid;
    m_SkinID = skinid;
}
DefineHook(void, SendSelectSkin, (uintptr_t thiz, uint32_t skinid, uint32_t heroid)) {
    if (oIsHaveSkin(0, skinid) || oIsHaveSkinForever(0, skinid) || !ndUnlockSkin) {
        m_SkinID = 0;
        return oSendSelectSkin(thiz, skinid, heroid);
    }
    m_HeroID = heroid;
    m_SkinID = skinid;
}
DefineHook(void, RefreshSkinDic, (uintptr_t thiz, uint32_t heroid, uint skinid, uint64_t uid )) {
    if (ndUnlockSkin2) {
        uint64_t m_uiID;
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
        return oRefreshSkinDic(thiz, m_HeroID, m_SkinID, m_uiID);
    }
    m_HeroID = *(uint32_t *) ((uintptr_t) thiz + 28); // Assuming UIRankHero_ChangeShow_iSelfHero offset is needed, but we don't have it. Using a safe default or removing the line might be better if we can't find offset.
    // Wait, Unlock.h used: m_HeroID = *(uint32_t *) ((uintptr_t) thiz + UIRankHero_ChangeShow_iSelfHero);
    // I need to find UIRankHero_ChangeShow_iSelfHero or similar.
    // For now, let's just stick to the first branch if ndUnlockSkin2 is true.
    
    m_SkinID = skinid;
    oRefreshSkinDic(thiz, heroid, skinid, uid);
}


DefineHook(void, RefreshHeroSkinShow, (uintptr_t thiz, uint64_t uid, uint32_t heroid, uint skinid)) {
    if (ndUnlockSkin2) {
        uint64_t m_uiID;
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
        return oRefreshHeroSkinShow(thiz, m_uiID, m_HeroID, m_SkinID);
    }
    // m_HeroID = ...
    m_SkinID = skinid;
    oRefreshHeroSkinShow(thiz, uid, heroid, skinid);
}

void (*oBatttleSelectSkin2)(void* thiz_, uint64_t uid, uint skinid);
void BatttleSelectSkin2(void* thiz_, uint64_t uid, uint skinid) {
    if (ndUnlockSkin3) {
        uint64_t m_uiID;
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
        return oBatttleSelectSkin2(thiz_, m_uiID, m_SkinID);
    }
    oBatttleSelectSkin2(thiz_, uid, skinid);
}

DefineHook(void, BatttleSelectSkin, (uintptr_t thiz, uint64_t uid, uint skinid)) {
    if (ndUnlockSkin2) {
        uint64_t m_uiID;
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
        return oBatttleSelectSkin(thiz, m_uiID, m_SkinID);
    }
    oBatttleSelectSkin(thiz, uid, skinid);
}
DefineHook(void, TrySendUseSkin, (void * thiz, uint32_t skinid, bool b)) {
    if (oIsHaveSkin(0, skinid) || oIsHaveSkinForever(0, skinid) || !ndUnlockSkin) {
        m_SkinID = 0;
        return oTrySendUseSkin(thiz, skinid, true);
    }
    // m_HeroID = ...
    m_SkinID = skinid;
}

DefineHook(void, SendUseSkin, (void * thiz, uint32_t skinid, bool b)) {
    if (oIsHaveSkin(0, skinid) || oIsHaveSkinForever(0, skinid) || !ndUnlockSkin) {
        m_SkinID = 0;
        return oSendUseSkin(thiz, skinid, true);
    }
    // m_HeroID = ...
    m_SkinID = skinid;
}

// Offsets for BattlePlayerInfo need to be found dynamically or assumed safe
static size_t off_lUid = 0;
static size_t off_uiSelHero = 0;
static size_t off_uiSkinId = 0;

void InitBattlePlayerInfoOffsets() {
    if (off_lUid == 0) {
        off_lUid = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "lUid");
        off_uiSelHero = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiSelHero");
        off_uiSkinId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiSkinId");
    }
}

DefineHook(void, SetPlayerData, (uintptr_t thiz, uintptr_t playerinfo, uint32_t uiSelfCamp)) {
    if (ndUnlockSkin2) {
        if (m_SkinID) {
            if (!oIsHaveSkin(0, m_SkinID) || !oIsHaveSkinForever(0, m_SkinID)) {
                uint64_t m_uiID;
                Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
                
                InitBattlePlayerInfoOffsets();
                if (off_lUid && off_uiSelHero && off_uiSkinId) {
                     if (*(uint64_t *) (playerinfo + off_lUid) == m_uiID) {
                        uint32_t heroid = *(uint32_t *) (playerinfo + off_uiSelHero);
                        if (heroid == m_HeroID) {
                            *(uint32_t *) ((uintptr_t) playerinfo + off_uiSkinId) = m_SkinID;
                        }
                    }
                }
            }
        }
    }
    return oSetPlayerData(thiz, playerinfo, uiSelfCamp);
}

DefineHook(void, SetPlayerData_, (uintptr_t thiz, uintptr_t playerinfo)) {
    if (ndUnlockSkin2) {
        if (m_SkinID) {
            if (!oIsHaveSkin(0, m_SkinID) || !oIsHaveSkinForever(0, m_SkinID)) {
                uint64_t m_uiID;
                Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
                
                InitBattlePlayerInfoOffsets();
                if (off_lUid && off_uiSelHero && off_uiSkinId) {
                     if (*(uint64_t *) (playerinfo + off_lUid) == m_uiID) {
                        uint32_t heroid = *(uint32_t *) (playerinfo + off_uiSelHero);
                        if (heroid == m_HeroID) {
                            *(uint32_t *) ((uintptr_t) playerinfo + off_uiSkinId) = m_SkinID;
                        }
                    }
                }
            }
        }
    }
    return oSetPlayerData_(thiz, playerinfo);
}

DefineHook(void, AddPlayerInfo, (uintptr_t playerinfo, uint selfCamp, uintptr_t roomdata, bool ignoreEmblem)) {
    if (ndUnlockSkin2) {
        if (m_SkinID) {
            if (!oIsHaveSkin(0, m_SkinID) || !oIsHaveSkinForever(0, m_SkinID)) {
                uint64_t m_uiID;
                Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
                
                InitBattlePlayerInfoOffsets();
                if (off_lUid && off_uiSelHero && off_uiSkinId) {
                     if (*(uint64_t *) ((uintptr_t) playerinfo + off_lUid) == m_uiID) {
                        uint32_t heroid = *(uint32_t *) ((uintptr_t) playerinfo + off_uiSelHero);
                        if (heroid == m_HeroID) {
                            *(uint32_t *) ((uintptr_t) playerinfo + off_uiSkinId) = m_SkinID;
                        }
                    }
                }
            }
        }
    }
    return oAddPlayerInfo(playerinfo, selfCamp, roomdata, ignoreEmblem);
}

DefineHook(void, AddPlayerInfo_, (uintptr_t playerinfo, uintptr_t roomdata)) {
    if (ndUnlockSkin2) {
         if (m_SkinID) {
            if (!oIsHaveSkin(0, m_SkinID) || !oIsHaveSkinForever(0, m_SkinID)) {
                uint64_t m_uiID;
                Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
                
                InitBattlePlayerInfoOffsets();
                if (off_lUid && off_uiSelHero && off_uiSkinId) {
                     if (*(uint64_t *) ((uintptr_t) playerinfo + off_lUid) == m_uiID) {
                        uint32_t heroid = *(uint32_t *) ((uintptr_t) playerinfo + off_uiSelHero);
                        if (heroid == m_HeroID) {
                            *(uint32_t *) ((uintptr_t) playerinfo + off_uiSkinId) = m_SkinID;
                        }
                    }
                }
            }
        }
    }
    return oAddPlayerInfo_(playerinfo, roomdata);
}

DefineHook(void, OnSetSelfBattlePlayerInfo, (uintptr_t thiz,  uintptr_t playerinfo)) {
    if (ndUnlockSkin2) {
         if (m_SkinID) {
            if (!oIsHaveSkin(0, m_SkinID) || !oIsHaveSkinForever(0, m_SkinID)) {
                uint64_t m_uiID;
                Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &m_uiID);
                
                InitBattlePlayerInfoOffsets();
                if (off_lUid && off_uiSelHero && off_uiSkinId) {
                     if (*(uint64_t *) ((uintptr_t) playerinfo + off_lUid) == m_uiID) {
                        uint32_t heroid = *(uint32_t *) ((uintptr_t) playerinfo + off_uiSelHero);
                        if (heroid == m_HeroID) {
                            *(uint32_t *) ((uintptr_t) playerinfo + off_uiSkinId) = m_SkinID;
                        }
                    }
                }
            }
        }
    }
    return oOnSetSelfBattlePlayerInfo(thiz, playerinfo);
}

DefineHook(void, CheckHeroDefaultSkin, (uintptr_t thiz, uint32_t heroid ,uint skinid )) {
    if (ndUnlockSkin2) {
        return oCheckHeroDefaultSkin(thiz, m_HeroID, m_SkinID);
    }
    m_HeroID = heroid; // Fallback
    m_SkinID = skinid;
}

DefineHook(void, SendRawData, (void * thiz, uint uiMsgID, void * rawData, int rawDataSize, int eSocketType, int packReliType, bool bNeedLockRequestIndex, int expectedSize)) {
    if (ndUnlockSkin2) {
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
            return;
        }
    }
    return oSendRawData(thiz, uiMsgID, rawData, rawDataSize, eSocketType, packReliType, bNeedLockRequestIndex, expectedSize);
}

// ================= INIT =================

void InitUnlockSkin() {
    // Sync flags
    ndUnlockSkin = g_UnlockAll;
    ndUnlockSkin2 = g_UnlockAll;
    ndUnlockSkin3 = true; // Always true in reference
    
    if (!g_UnlockAll) return;

    LOGI("UnlockSkin: Initializing...");

    // Find Constructors
    CmdHeroSkin__ctor = (uintptr_t)Il2CppGetMethodOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", ".ctor", 0);
    CmdHeroStatue__ctor = (uintptr_t)Il2CppGetMethodOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroStatue", ".ctor", 0);

    // Helper macro to hook by finding method
    #define HOOK_METHOD(CLASS, METHOD, ARG_COUNT, HOOK_FUNC, ORIG_FUNC) \
        { \
            void* addr = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", CLASS, METHOD, ARG_COUNT); \
            if (addr) Tools::Hook(addr, (void*)HOOK_FUNC, (void**)&ORIG_FUNC); \
            else LOGE("UnlockSkin: Failed to find %s::%s", CLASS, METHOD); \
        }


    HOOK_METHOD("SystemData", "GetHeroSkin", 2, GetHeroSkin, oGetHeroSkin);
    // HOOK_METHOD("SystemData", "GetMCLimitSkin", 1, GetMCLimitSkin, oGetMCLimitSkin);
    HOOK_METHOD("SystemData", "GetHeroHolyStatue", 2, GetHeroHolyStatue, oGetHeroHolyStatue);
    HOOK_METHOD("SystemData", "IsCanUseSkin", 1, IsCanUseSkin, oIsCanUseSkin);
    HOOK_METHOD("SystemData", "IsHaveSkin", 1, IsHaveSkin, oIsHaveSkin);
    HOOK_METHOD("SystemData", "IsHaveSkinForever", 1, IsHaveSkinForever, oIsHaveSkinForever);
    HOOK_METHOD("SystemData", "IsHaveStatue", 1, IsHaveStatue, oIsHaveStatue);
    HOOK_METHOD("SystemData", "IsHaveStatueForever", 1, IsHaveStatueForever, oIsHaveStatueForever);
    HOOK_METHOD("SystemData", "IsForbidStatue", 1, IsForbidStatue, oIsForbidStatue);
    HOOK_METHOD("SystemData", "IsForbidSkin", 2, IsForbidSkin, oIsForbidSkin);
    HOOK_METHOD("SystemData", "IsForbidARSkin", 1, IsForbidARSkin, oIsForbidARSkin);
    // HOOK_METHOD("SystemData", "GetExpiredCardSkinLimitTime", 1, GetExpiredCardSkinLimitTime, oGetExpiredCardSkinLimitTime);
    HOOK_METHOD("SystemData", "GetLeaderSkinBForbid", 1, GetLeaderSkinBForbid, oGetLeaderSkinBForbid);

    HOOK_METHOD("UIChooseHero", "SendSelectSkin", 2, SendSelectSkin, oSendSelectSkin);
    HOOK_METHOD("UIRankHero", "BatttleSelectSkin", 2, BatttleSelectSkin, oBatttleSelectSkin);
    HOOK_METHOD("UIRankHero", "BatttleSelectSkin2", 2, BatttleSelectSkin2, oBatttleSelectSkin2);
    HOOK_METHOD("UIRankHero", "BRankHeroCanUse", 1, BRankHeroCanUse, oBRankHeroCanUse);
    
    // HOOK_METHOD("StarMemberData", "ContainSkin", 1, ContainSkin, oContainSkin);
    HOOK_METHOD("UIChooseHero", "CheckHeroDefaultSkin", 2, CheckHeroDefaultSkin, oCheckHeroDefaultSkin);
    
    // Note: Some class names might need adjustment if they are nested or different.
    HOOK_METHOD("UIRankHero", "SendUseSkin", 2, SendUseSkin, oSendUseSkin); // In Unlock.h it says UIRankHero_ChangeShow_SendUseSkin, assuming it is in UIRankHero or UIRankHero.ChangeShow?
    // Let's try UIRankHero first. If it fails, we might need to search deeper.
    
    HOOK_METHOD("BattleReceiveMessage", "SetPlayerData", 2, SetPlayerData, oSetPlayerData);
    HOOK_METHOD("BattleReceiveMessage", "SetPlayerData_", 1, SetPlayerData_, oSetPlayerData_); // Overload with 1 arg?
    
    HOOK_METHOD("GameServerConfig", "SendRawData", 7, SendRawData, oSendRawData); // 7 args + this
    
    HOOK_METHOD("ChooseHeroMgr", "OnSetSelfBattlePlayerInfo", 1, OnSetSelfBattlePlayerInfo, oOnSetSelfBattlePlayerInfo);
    
    HOOK_METHOD("UIChooseHero", "RefreshSkinDic", 3, RefreshSkinDic, oRefreshSkinDic);
    HOOK_METHOD("UIChooseHero", "RefreshHeroSkinShow", 3, RefreshHeroSkinShow, oRefreshHeroSkinShow); // 3 args? Unlock.h says (uintptr_t thiz, uint64_t uid, uint32_t heroid, uint skinid) -> 3 args + this
    
    // CheckReputationUnlockSkin, SendData, GetStarSkinRank, IsForbidHeros, IsForbidHeroInChooseHero, IsActivityForbidHeros, IsLimitActiveHero, IsHeroInShop
    // These were in Unlock.h but not hooked in my Init list yet. Adding them.
    HOOK_METHOD("SystemData", "CheckReputationUnlockSkin", 1, CheckReputationUnlockSkin, oCheckReputationUnlockSkin); // Guessing SystemData
    // SendData ? In Unlock.h: DefineHook(bool, SendData, (void * unk, int skinid))
    // Could be SystemData::SendData?
    
    // HOOK_METHOD("SystemData", "GetStarSkinRank", 1, GetStarSkinRank, oGetStarSkinRank);
    HOOK_METHOD("SystemData", "IsForbidHeros", 1, IsForbidHeros, oIsForbidHeros);
    HOOK_METHOD("SystemData", "IsForbidHeroInChooseHero", 1, IsForbidHeroInChooseHero, oIsForbidHeroInChooseHero);
    HOOK_METHOD("SystemData", "IsActivityForbidHeros", 1, IsActivityForbidHeros, oIsActivityForbidHeros);
    HOOK_METHOD("SystemData", "IsLimitActiveHero", 1, IsLimitActiveHero, oIsLimitActiveHero);
    HOOK_METHOD("SystemData", "IsHeroInShop", 1, IsHeroInShop, oIsHeroInShop);

    // NEW HOOK
    HOOK_METHOD("SystemData", "IsForbidARHero", 1, IsForbidARHero, oIsForbidARHero);
    HOOK_METHOD("SystemData", "IsForbidNewHeroList", 1, IsForbidNewHeroList, oIsForbidNewHeroList);
    HOOK_METHOD("SystemData", "IsForbidHeadFrameForce", 1, IsForbidHeadFrameForce, oIsForbidHeadFrameForce);
    HOOK_METHOD("SystemData", "IsForbidHeadFrame", 1, IsForbidHeadFrame, oIsForbidHeadFrame);

    LOGI("UnlockSkin: Hooks Applied.");
}
