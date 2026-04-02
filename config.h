#ifndef DANARC_CONFIG_H
#define DANARC_CONFIG_H
#include <cstdint>

//-------------------Config-------------------// btw m stands of meters
inline double maxPlayerDist = 400/*<-Dist in m*/ * 100;
inline double maxArcDist = 150  /*<-Dist in m*/  * 100;
inline double maxLootDist = 40 /*<-Dist in m*/   * 100;
inline bool enableRadar = true;

inline int SCREEN_H = 1440;
inline int SCREEN_W = 2560;

inline constexpr bool isDebugMode = false;
//----------------------------------------------

#define pr inline uintptr_t
#define ptr uintptr_t
#define DBG if(!isDebugMode) {} else

namespace off {
    // From UWorld : "class World : public Object", "struct World : public Object", "struct UWorld : public UObject"
    pr PERSISTENT_LEVEL = 0x110; //0xF0; //0x110; //0x108; //0x130; //0xD0;//0xF8; //same on 1/20 and 1/27

    // From PersistentLevel (ULevel) "Level* PersistentLevel;"
    // Known as AActor or Actor Array "struct Actor : public Object". "struct AActor : public UObject"
    pr ACTORS_PTR = 0x108; // same on 2/26, 0x100; //0x108; //same on 1/20 and 1/27, 1/28 // "Actor* OwningActor; // 0x108"

    // From Actor
    pr ROOT_COMPONENT_PTR = 0x238; //0x0228;//0x220; //0x228; //0x238; //0x228;

    // From SceneComponent (RootComponent) "struct SceneComponent : public ActorComponent, public Object"
    pr POS_PTR = 0x220; //0x290; //0x258; //0x248;//0x1D0; //Vector RelativeLocation;
    pr VIEW_MATRIX = 0xcc8;

    //"struct PioneerPlayerCharacter : public PioneerPlayerCharacterInterface, public PioneerCharacterBase, public EmbarkCharacterBase, public Character, public Pawn, public Actor, public Object"
    pr HEALTH_COMPONENT = 0xd28;
    pr CACHED_HEALTH = 0x6a8; //"struct HealthComponent : public HealthComponentInterface, public ActorComponent, public Object"
}

namespace vtabels {

    pr ARC = 0x14bde0210;

    pr SEARCH = 0x14bdcc840;
    pr PICKUP = 0x14bd92fc0;

    pr PLAYER = 0x14bee4880;

    //Player 14bf34d40
    //Cam Manager 14b233b10
}

#endif //DANARC_CONFIG_H