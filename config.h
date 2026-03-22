#ifndef DANARC_CONFIG_H
#define DANARC_CONFIG_H
#include <cstdint>

//-------------------Config-------------------// btw m stands of meters
inline double maxPlayerDist = 400/*<-Dist in m*/ * 100;
inline double maxArcDist = 200  /*<-Dist in m*/  * 100;
inline double maxLootDist = 70 /*<-Dist in m*/   * 100;
inline bool enableRadar = true;

inline int SCREEN_H = 1440;
inline int SCREEN_W = 2560;

inline constexpr bool isDebugMode = true;
//----------------------------------------------

#define pr inline uintptr_t
#define ptr uintptr_t
#define DBG if(!isDebugMode) {} else

namespace off {
    // From UWorld : "class World : public Object", "struct World : public Object"
    pr PERSISTENT_LEVEL = 0xF0; //0x110; //0x108; //0x130; //0xD0;//0xF8; //same on 1/20 and 1/27
    pr CACHED_VIEW_INFO_PTR = 0x1e8; //0x1A0; //0x220; //was 0x198
    pr CACHED_POS_PTR = 0x0;

    // From PersistentLevel (ULevel) "Level* PersistentLevel;"
    // Known as AActor or Actor Array "struct Actor : public Object"
    pr ACTORS_PTR = 0x108; //0x100; //0x108; //same on 1/20 and 1/27, 1/28 // "Actor* OwningActor; // 0x108"

    // From Actor
    pr ROOT_COMPONENT_PTR = 0x228; //0x0228;//0x220; //0x228; //0x238; //0x228;

    // From SceneComponent (RootComponent) "struct SceneComponent : public ActorComponent, public Object"
    pr POS_PTR = 0x208; //0x290; //0x258; //0x248;//0x1D0; //Vector RelativeLocation;
    pr VIEW_MATRIX = 0xCA8;

    //"struct PioneerPlayerCharacter : public PioneerPlayerCharacterInterface, public PioneerCharacterBase, public EmbarkCharacterBase, public Character, public Pawn, public Actor, public Object"
    pr HEALTH_COMPONENT = 0xd28;
    pr CACHED_HEALTH = 0x678; //"struct HealthComponent : public HealthComponentInterface, public ActorComponent, public Object"
}

namespace vtabels {

    pr ARC = 0x14bd8c7b0;

    pr SEARCH = 0x14bd79000;
    pr PICKUP = 0x14bd3f240;

    pr PLAYER = 0x14be90130;

    //Player 14bf34d40
    //Cam Manager 14b233b10
}

#endif //DANARC_CONFIG_H