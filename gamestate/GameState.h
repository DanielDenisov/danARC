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
    int localPlayerTeam{};
};

class GameState {
private:
    ptr baseAddr{};
    int lastTeamID = -1; //needed because local player is based off dist from camera,
    //if you roll, you go father, outside reasonable range, so need to save to memory

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
        int localPlayerTeam = -1;
    };
    FilterRet filterEnt(std::vector<RenderEntity> rawEnt, FminimalViewInfo vm);

};

#endif //DANARC_GAMESTATE_H