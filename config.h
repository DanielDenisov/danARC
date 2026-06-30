#ifndef DANARC_CONFIG_H
#define DANARC_CONFIG_H
#include <cstdint>

//-------------------Config-------------------// btw m stands of meters

//ESP Preferences
inline bool makeLootESPTranslucent = true;
inline bool makeDeadBotESPTranslucent = true;
inline bool makeDeadPlayerESPTranslucent = true;

//ESP Distance Config
inline double maxPlayerDist = 400/*<-Dist in m*/;
inline double maxArcDist = 150  /*<-Dist in m*/ ;
inline double maxLootDist = 40 /*<-Dist in m*/  ;

//If You want Radar
inline bool enableRadar = true;

//Set this to your monitor resolution
inline int SCREEN_H = 1440 /*<-Make you monitor height*/ * 0.94/*just trust me, this helps with distortion around screen edge*/;
inline int SCREEN_W = 2560 /*<-Make you monitor width*/;

//For updating the program
inline constexpr bool isDebugMode = false;
//----------------------------------------------


#define pr inline uintptr_t
#define ptr uintptr_t
#define DBG if(!isDebugMode) {} else

namespace off {
    pr uworld = 0xE83FC58;

    // From UWorld : "class World : public Object", "struct World : public Object", "struct UWorld : public UObject"
    //Typically found between 0xF0 and 0150
    pr PERSISTENT_LEVEL = 0x130;

    // From PersistentLevel (ULevel) "Level* PersistentLevel;"
    // Known as AActor or Actor Array "struct Actor : public Object". "struct AActor : public UObject"
    //Typically around 0x108
    pr ACTORS_PTR = 0x108;

    pr ROOT_COMPONENT_PTR = 0x228;

    //Find by uncommenting getViewMatrix(actor); in GameState.cpp and finding were it matches the pattern.
    pr VIEW_MATRIX = 0xc88;
    pr POS_PTR = 0x210;

    // "namespace EmbarkCharacterBase"
    pr TEAM_ID = 0x812; //TeamId

    //find under // Class /Script/Engine.Character, "namespace Character"
    pr MESH = 0x428;

    //im just guessing for this one
    // find under "namespace EmbarkCharacterMovementComponentBase" OR "namespace CharacterMovementComponent"
    pr LAST_SUBMIT_TIME = 0x4CC; //NetworkMinTimeBetweenClientAckGoodMoves

    pr IS_DESTROYED = 0x1210; //bIsDestroyed
}

namespace vtabels {

    pr ARC = 0x14c7b6d20;

    pr SEARCH = 0x14c7a3000;
    pr PICKUP = 0x14c767570;

    pr PLAYER = 0x14c8ce680;
    pr CAMMANAGER = 0x14bb3c190;
}

#endif //DANARC_CONFIG_H