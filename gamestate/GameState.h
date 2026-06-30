#ifndef DANARC_GAMESTATE_H
#define DANARC_GAMESTATE_H

#include "config.h"
#include "utils/localUtil.h"

#include <thread>
#include<vector>
#include "memory/memory.h"


struct gsRet {
    std::vector<RenderEntity> entities{};
    FminimalViewInfo vm{};
    char localPlayerTeam{};
};

class GameState {
private:
    ptr baseAddr{};

public:
    GameState(ptr baseAddr = 0x140000000);

    gsRet tick();

private:
    ptr getUworld();

    struct RawInfo {
        std::vector<RenderEntity> rawEnt{};
        FminimalViewInfo VM{};
    };
    RawInfo getRawEnt(ptr uworld);

    struct FilterRet {
        std::vector<RenderEntity> filtEnt{};
        char localPlayerTeam = 'l';
    };
    FilterRet filterEnt(std::vector<RenderEntity> rawEnt, FminimalViewInfo vm);

};

#endif //DANARC_GAMESTATE_H