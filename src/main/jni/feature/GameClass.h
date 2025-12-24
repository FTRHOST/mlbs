#pragma once


#pragma once


//Class Camera
#define Camera_get_main (uintptr_t) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "get_main")
#define Camera_WorldToScreenPoint (uintptr_t) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "WorldToScreenPoint", 1)
#define Camera_get_fieldOfView (uintptr_t) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "get_fieldOfView")
#define Camera_set_fieldOfView (uintptr_t) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Camera", "set_fieldOfView", 1)

void *get_main() {
	return reinterpret_cast<void *(__fastcall *)()>(Camera_get_main)();
}

Vector3 WorldToScreen(Vector3 position) {
	return reinterpret_cast<Vector3(__fastcall *)(void *, Vector3)>(Camera_WorldToScreenPoint)(get_main(), position);
}

float get_fieldOfView() {
	return reinterpret_cast<float(__fastcall *)(void *)>(Camera_get_fieldOfView)(get_main());
}

void *set_fieldOfView(float value) {
	return reinterpret_cast<void *(__fastcall *)(void *, float)>(Camera_set_fieldOfView)(get_main(), value);
}

//Class Transform
#define Transform_get_position (uintptr_t) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Transform", "get_position")

class Transform {
public:
    Vector3 get_position() {
        auto get_position = (Vector3 (*)(Transform *)) (Transform_get_position);
        return get_position(this);
    }
};

//Class BattleManager
#define BattleManager_m_LocalPlayerShow (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_LocalPlayerShow")
#define Camera_get_Positions (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_LocalPlayerShow")
#define BattleManager_m_ShowPlayers (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_ShowPlayers")
#define BattleManager_m_ShowMonsters (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", "m_ShowMonsters")
#define BattleManager_m_LocalPlayerShow (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", OBFUSCATE("m_LocalPlayerShow"))

#define BattleManager_m_dicPlayerShow (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", OBFUSCATE("m_dicPlayerShow"))

#define BattleManager_m_dicMonsterShow (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", OBFUSCATE("m_dicMonsterShow"))

#define BattleManager_m_ShowPlayers (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleManager", OBFUSCATE("m_ShowPlayers"))

//Class BattleBridge
#define BattleBridge_bStartBattle (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "BattleBridge", "bStartBattle")


#define BattleManager_m_LocalShowPlayers (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("BattleManager"), OBFUSCATE("m_LocalShowPlayers"))
#define ShowPlayer_m_bInBattle (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("ShowPlayer"), OBFUSCATE("m_bInBattle"))

//Class ShowEntity
#define ShowEntity_get_InTransformation (uintptr_t) Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("get_InTransformation"))

#define ShowEntity__Position (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_vCachePosition"))

#define ShowEntity_bShowEntityLayer (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("bShowEntityLayer"))

#define ShowEntity_m_OwnSkillComp (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_OwnSkillComp"))

#define ShowEntity_m_RoleName (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_RoleName"))

#define ShowEntity__MoveDir (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("_MoveDir"))
//Class EntityBase
#define EntityBase_bMove (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "Battle", "EntityBase", OBFUSCATE("bMove"))

#define EntityBase_m_ID (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_ID"))

#define EntityBase_m_Level (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_Level"))

#define EntityBase_m_bDeath (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_bDeath"))

#define EntityBase_m_IsDeathProgress (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_IsDeathProgress"))

#define EntityBase_m_bSameCampType (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_bSameCampType"))

#define EntityBase_m_Hp (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_Hp"))

#define EntityBase_m_HpMax (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_HpMax"))
#define EntityBase_m_EntityCampType (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_EntityCampType"))
#define EntityBase_m_iType (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_iType"))
#define EntityBase_canSight (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("canSight"))
#define EntityBase_m_uGuid (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_uGuid"))
#define EntityBase_m_bSameCampType (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_bSameCampType")
#define EntityBase_m_EntityCampType (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", OBFUSCATE("m_EntityCampType"))
#define EntityBase_m_IsDeathProgress (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_IsDeathProgress")
#define EntityBase_m_bDeath (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_bDeath")
#define EntityBase_m_Hp (uintptr_t)  Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_Hp")
#define EntityBase_m_HpMax (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_HpMax")
#define EntityBase_m_ID (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_ID")
#define EntityBase_m_uGuid (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_uGuid")
#define EntityBase_m_Level (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_Level")
#define EntityBase_m_iType (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "m_iType")
#define EntityBase_canSight (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowEntity", "canSight")

//Class ShowPlayer
#define ShowPlayer_m_HeroName (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowPlayer", OBFUSCATE("m_HeroName"))
#define ShowPlayer_m_HeroName (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "ShowPlayer", "m_HeroName")

//Class LogicFighter
#define LogicFighter_m_SkillComp (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "Battle", OBFUSCATE("LogicFighter"), OBFUSCATE("m_SkillComp"))

#define LogicBattleManager_GetBattleState (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("LogicBattleManager"), OBFUSCATE("GetBattleState"))

int GetBattleState(void* instance) {
    return reinterpret_cast<int(__fastcall *)(void*)>(LogicBattleManager_GetBattleState)(instance);
}

//Class SystemData
#define SystemData_GetBattlePlayerInfo (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("GetBattlePlayerInfo"))

//Class SystemData.RoomData
#define SystemData_RoomData_iCamp (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("iCamp"))
#define SystemData_RoomData__sName (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("_sName"))
#define SystemData_RoomData__steamSimpleName (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("_steamSimpleName"))
#define SystemData_RoomData__steamName (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("_steamName"))
#define SystemData_RoomData_uiRankLevel (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("uiRankLevel"))
#define SystemData_RoomData_iMythPoint (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("iMythPoint"))
#define SystemData_RoomData_heroid (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("heroid"))
#define SystemData_RoomData_summonSkillId (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("summonSkillId"))
#define SystemData_RoomData_lUid (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("lUid"))
#define SystemData_RoomData_uiZoneId (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("uiZoneId"))
#define SystemData_RoomData_iPos (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData/RoomData"), OBFUSCATE("iPos"))
#define SystemData_StopCheckMD5 (uintptr_t) Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", OBFUSCATE("StopCheckMD5"))

//Duplikasi Room InfoTable
#define RoomData_steamSimpleName (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", "_steamSimpleName")
#define RoomData_steamName (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", "_steamName")
#define RoomData_lUid (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("lUid"))
#define RoomData_uiZoneId (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("uiZoneId"))
#define RoomData_iCamp (uintptr_t) Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("iCamp"))
#define RoomData_heroid (uintptr_t) Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("heroid"))
#define RoomData_uiHeroIDChoose (uintptr_t) Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("uiHeroIDChoose"))
#define RoomData_iCertify (uintptr_t) Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("iCertify"))
#define RoomData_bRoomLeader (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("bRoomLeader"))
#define RoomData_summonSkillId (uintptr_t) Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("summonSkillId"))
#define RoomData_sName (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("_sName"))
#define RoomData_uiRankLevel (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("uiRankLevel"))
#define RoomData_iMythPoint (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("iMythPoint"))
#define RoomData_uiSex (uintptr_t) Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("uiSex"))
#define RoomData_sCreateRoleCountry (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("sCreateRoleCountry"))
#define RoomData_Flag (uintptr_t) Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("country"))
#define RoomData_iActCamp (uintptr_t) Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "SystemData/RoomData", OBFUSCATE("iActCamp"))

//class SystemData
#define SystemData_GetBattlePlayerInfo (uintptr_t) Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", OBFUSCATE("GetBattlePlayerInfo"), 0)
#define SystemData_m_uiID (uintptr_t) Il2CppGetStaticFieldOffset("Assembly-CSharp.dll","","SystemData", OBFUSCATE("m_uiID"))


MonoList<uintptr_t> *GetBattlePlayerInfo() {
    return reinterpret_cast<MonoList<uintptr_t> *(*)(void *)>(SystemData_GetBattlePlayerInfo)(nullptr);
}

void StopMD5Check(){
    return reinterpret_cast<void (__fastcall *)(void *)>(SystemData_StopCheckMD5)((void *)0);
}


//Unlock Skin -> Mencoba
//Class UIRankHero.ChangeShow
#define UIRankHero_ChangeShow_iSelfHero (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIRankHero/ChangeShow"), OBFUSCATE("iSelfHero"))

//Class BattlePlayerInfo
#define BattlePlayerInfo_lUid (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("MTTDProto"), OBFUSCATE("BattlePlayerInfo"), OBFUSCATE("lUid"))
#define BattlePlayerInfo_uiSelHero (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("MTTDProto"), OBFUSCATE("BattlePlayerInfo"), OBFUSCATE("uiSelHero"))
#define BattlePlayerInfo_uiSkinId (uintptr_t) Il2CppGetFieldOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("MTTDProto"), OBFUSCATE("BattlePlayerInfo"), OBFUSCATE("uiSkinId"))

//Class CmdHeroSkin
#define CmdHeroSkin__ctor (uintptr_t) (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("MTTDProto"), OBFUSCATE("CmdHeroSkin"), OBFUSCATE(".ctor"))

//Class CmdHeroStatue
#define CmdHeroStatue__ctor (uintptr_t) (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("MTTDProto"), OBFUSCATE("CmdHeroStatue"), OBFUSCATE(".ctor"))

//Class SystemData,
#define SystemData_GetHeroSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("GetHeroSkin"), 2)
#define SystemData_GetMCLimitSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("GetMCLimitSkin"), 1)
#define SystemData_GetHeroHolyStatue (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("GetHeroHolyStatue"), 2)
#define SystemData_IsCanUseSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsCanUseSkin"), 1)
#define SystemData_IsHaveSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsHaveSkin"), 1)
#define SystemData_IsHaveSkinForever (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsHaveSkinForever"), 1)
#define SystemData_IsHaveStatue (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsHaveStatue"), 1)
#define SystemData_IsHaveStatueForever (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsHaveStatueForever"), 1)
#define SystemData_IsForbidStatue (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsForbidStatue"), 1)
#define SystemData_IsForbidSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsForbidSkin"), 1)
#define SystemData_IsForbidARSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsForbidARSkin"), 1)
#define SystemData_CheckReputationUnlockSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("CheckReputationUnlockSkin"), 1)
#define SystemData_GetStarSkinRank (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("GetStarSkinRank"),1)

#define SystemData_IsLimitActiveHero (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsLimitActiveHero"), 1)
#define SystemData_IsActivityForbidHeros (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsActivityForbidHeros"), 1)
#define SystemData_IsForbidHeroInChooseHero (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsForbidHeroInChooseHero"), 1)
#define SystemData_IsForbidHeros (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("IsForbidHeros"),1)

//Class UIChooseHero
#define UIChooseHero_SendSelectSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("ChooseHeroMgr"), OBFUSCATE("SendSelectSkin"), 2)
#define UIChooseHero_SaveSelectHeroSkinId (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("ChooseHeroMgr"), OBFUSCATE("SaveSelectHeroSkinId"), 2)
#define ChooseHeroMgr_BCustomRoomFreeSkill (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("ChooseHeroMgr"), OBFUSCATE("BCustomRoomFreeSkill"), 0)
#define ChooseHeroMgr_OnSetSelfBattlePlayerInfo (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("ChooseHeroMgr"), OBFUSCATE("OnSetSelfBattlePlayerInfo"), 1)
#define UIDataUtil_IsHeroInShop (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIDataUtil"), OBFUSCATE("IsHeroInShop"), 1)

//Class UIRankHero
#define UIRankHero_BatttleSelectSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIRankHero"), OBFUSCATE("BatttleSelectSkin"), 2)
#define UIRankHero_BatttleSelectSkin2 (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIChooseHero"), OBFUSCATE("BatttleSelectSkin"), 2)

#define UIRankHero_RefreshSkinDic (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIRankHero"), OBFUSCATE("RefreshSkinDic"), 3)
#define UIRankHero_RefreshHeroSkinShow (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIRankHero/RankingHeroShow"), OBFUSCATE("RefreshHeroSkinShow"), 3)

//Class UIRankHero.ChangeShow
#define UIRankHero_ChangeShow_TrySendUseSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIRankHero/ChangeShow"), OBFUSCATE("TrySendUseSkin"), 2)
#define UIRankHero_ChangeShow_SendUseSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIRankHero/ChangeShow"), OBFUSCATE("SendUseSkin"), 2)


//Class BattleReceiveMessage
#define BattleReceiveMessage_SetPlayerData (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("BattleReceiveMessage"), OBFUSCATE("SetPlayerData"), 2)
#define BattleReceiveMessage_SetPlayerData_ (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("BattleReceiveMessage"), OBFUSCATE("SetPlayerData"), 1)
#define BattleReceiveMessage_AddPlayerInfo (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("BattleReceiveMessage"), OBFUSCATE("AddPlayerInfo"), 4)
#define BattleReceiveMessage_AddPlayerInfo_ (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("BattleReceiveMessage"), OBFUSCATE("AddPlayerInfo"), 2)
#define BattleReceiveMessage_SendBattleData (uintptr_t) Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "BattleReceiveMessage", "SendBattleData", 4)
#define GameReceiveMessage_SendGameData (uintptr_t) Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "GameReceiveMessage", "SendGameData", 3)
#define GameReceiveMessage_SendGameRawData (uintptr_t) Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "GameReceiveMessage", "_SendGameRawData", 6)

//Class GameServerConfig
#define GameServerConfig_SendRawData (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("GameServerConfig"), OBFUSCATE("SendRawData"), 8)
#define GameServerConfig_SendData2 (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("GameServerConfig"), OBFUSCATE("SendData"), 6)
#define GameServerConfig_IsChannel_dfjz (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("GameServerConfig"), OBFUSCATE("IsChannel_dfjz"), 0)
#define SystemData_GetExpiredCardSkinLimitTime (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("GetExpiredCardSkinLimitTime"), 1)
#define SystemData_GetLeaderSkinBForbid (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("SystemData"), OBFUSCATE("GetLeaderSkinBForbid"), 1)
#define UIRankHero_BRankHeroCanUse (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIRankHero"), OBFUSCATE("BRankHeroCanUse"), 1)
#define StarMemberData_ContainSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("StarMemberData"), OBFUSCATE("ContainSkin"), 1)
#define UIChooseHero_CheckHeroDefaultSkin (uintptr_t) Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("UIChooseHero"), OBFUSCATE("CheckHeroDefaultSkin"),1)

