#pragma once

#ifndef DANARC_LOCALUTIL_H
#define DANARC_LOCALUTIL_H
#include <cmath>
#include <cstdint>
#include <deque>
#include <fstream>

#include "../memory/memory.h"
#include "gameUtil.h"

inline bool isValidPtr(ptr addr) {
    if (addr < 0x10000 || addr > 0x800000000000) return false;
    return true;
}

enum Object {
    ARC,
    SEARCH,
    PICKUP,
    PLAYER,
    NONE
};

struct RenderEntity {
    ptr actor;
    Vector3 pos;         //Position
    double dist{};       //Dist, may not be accurate until "filterEntities(...)"
    double playerHealth = 100;
    ptr vt;
    enum Object type = Object::NONE; //vt enum
    bool isDead = false; //Is dead, may not be accurate until "filterEntities(...)"

    void Print() {
        pos.Print();
        std::cout << "Type: " << type << std::endl;
    }
};

#endif //DANARC_LOCALUTIL_H